#include "Matrix.hpp"

#define OFFSET_VALUE    0           // Пройденный путь (в метрах) между сигналами от ДПП
// #define OFFSET_VALUE    0.2256      // Пройденный путь (в метрах) между сигналами от ДПП

extern float gyro_multiplier;

class Measure 
{
public:
    // -------------------------------------------------------------------------------
    // Данные, которые будут меняться в вызываемых функциях
    // Вынесем их в статические переменные для избежания переполнения стека процессора
    Data current_Data, zero_Data, buffer_Data;
    Matrix temp_matrix;
    
    // -------------------------------------------------------------------------------
    Matrix rotation_matrix;         // Матрица перехода от СК платы к глобальной СК, у которой ось OY направлена на север, а OZ перпендикулярно поверхности
    Matrix buffer_matrix;           // Матрица поворота от i-1 состояния в исходное
    float longitude;                // Широта места, где будет находиться плата

    // -------------------------------------------------------------------------------
    float Coordinates[3] = {0};     // Координаты перемещения по осям XYZ
    float Velocity[3] = {0};        // Скорости по осям XYZ
    float Angles[3] = {0};          // Углы поворота вокруг XYZ
    float shift;                    // Погрешность, накапливаемая при работе датчиков, которую необходимо компенсировать

    // -------------------------------------------------------------------------------
    bool new_data_Flag = FALSE;     // Флаг, отвечающий за наличие новых данных, требующих обработки
    
    // -------------------------------------------------------------------------------
    // Переменные для работы с прерываниями от таймера
    float period;                   // Период изменения TickCounter в секундах
    uint32_t TickCounter;           // Счётчик тиков запущенного таймера с периодом period (см main.cpp --> TimerInit())
    uint16_t filter_depth = 1024;   // Глубина усреднения

    // Буферы, в которые будет заполняться временная информация с датчиков, при чтении данных во время прерывания от таймера
    int16_t Acc_Buffer[3];
    int16_t Gyro_Buffer[3];
    int16_t tmp_Buffer[3];
    float sensitivity;              // Чувствительность акселерометра
    // -------------------------------------------------------------------------------
    // Переменные для работы с прерываниями от USART (канал связи с ДПП)
    uint32_t _DppCode;              // Код от ДПП
    float offset;                   // Перемещение от прошлого сигнала 
    // -------------------------------------------------------------------------------
    uint16_t index1, index2;        // Индексы для циклов (объявляем здесь чтобы не выделять под них постоянно память в ходе программы) 
    // -------------------------------------------------------------------------------

    // ########################################################################
    // Конструктор класса и другие функции для работы с параметрами класса
    Measure(float phi, float _period) { longitude = phi; period = _period;  }

    // ########################################################################
    // Функции для управления светодиодами    
    friend void LedOn(Led_TypeDef Led);
    friend void LedOff(Led_TypeDef Led);

    // ########################################################################
    // Чтение данных и последующая обработка (основной цикл программы)
    void measuring(){
        while (1)
        {
            LedOff(LED8);
            current_Data.Read_Data();

            // Переведём данные из СК датчика в СК Земли
            // current_Data -= zero_Data;
            // rotation_matrix *= current_Data;
            current_Data.sending_USB();

            if (new_data_Flag)
            {
                // Начнём обработку новых данных
                for (index1 = 0; index1 < 3; index1++){
                    buffer_Data.Acc[index1] = Acc_Buffer[index1] / sensitivity;
                    buffer_Data.Gyro[index1] = Gyro_Buffer[index1] * gyro_multiplier;
                }
                
                // Вычислим угол поворота за время period, опираясь на данные полученные только на части этого промежутка
                // При этом считаем, что за время period не произойдёт сильных изменений ни угловых скоростей, ни ускорений
                for (index1 = 0; index1 < 3; index1++){
                    Angles[index1] = buffer_Data.Gyro[index1] * period;
                }
                
                // Далее считаем, что temp_matrix - матрица поворота от i-го состояния в i-1 состояние, соответствущее прошлой итерации обработки данных
                // Заполним матрицу temp_matrix углами поворота вокруг XYZ (https://ru.wikipedia.org/wiki/Матрица_поворота)
                temp_matrix(0, 0) =  cos(Angles[1]) * cos(Angles[2]);
                temp_matrix(0, 1) = -sin(Angles[2]) * cos(Angles[1]);
                temp_matrix(0, 2) =  sin(Angles[1]);

                temp_matrix(1, 0) =  sin(Angles[0]) * sin(Angles[1]) * cos(Angles[2]) + sin(Angles[2]) * cos(Angles[0]);
                temp_matrix(1, 1) = -sin(Angles[0]) * sin(Angles[1]) * sin(Angles[2]) + cos(Angles[0]) * cos(Angles[2]);
                temp_matrix(1, 2) = -sin(Angles[0]) * cos(Angles[1]);

                temp_matrix(2, 0) =  sin(Angles[0]) * sin(Angles[2]) - sin(Angles[1]) * cos(Angles[0]) * cos(Angles[2]);
                temp_matrix(2, 1) =  sin(Angles[0]) * cos(Angles[2]) + sin(Angles[1]) * sin(Angles[2]) * cos(Angles[0]);
                temp_matrix(2, 2) =  cos(Angles[0]) * cos(Angles[1]);

                temp_matrix *= buffer_matrix;       // Получили матрицу поворота в исходное состояние, при котором проводилась выставка
                buffer_matrix = temp_matrix;        // Сохраним значения temp_matrix в buffer_matrix, чтобы корректно перейти к i+1 состоянию
                
                temp_matrix *= rotation_matrix;     // Получили матрицу поворота в СК Земли из i-го состояния

                // Вычислим приращение координат за время period в СК Земли
                temp_matrix *= buffer_Data.Acc;     // Значение ускорений в СК Земли

                // Заполним данные о координатах и скоростях                
                for (index1 = 0; index1 < 3; index1++){
                    Coordinates[index1] = Velocity[index1] * period + buffer_Data.Acc[index1] * period * period / 2;    
                    Velocity[index1] =  buffer_Data.Acc[index1] * period;
                }
            }

            new_data_Flag = FALSE;            
            LedOn(LED8);
        }
    }

    // ########################################################################
    // Обработка прерывания таймера

    friend void ReadAcc_int16(int16_t* pfData, float* sensitivity);
    friend void ReadGyro_int16(int16_t *pfData);

    void new_tick(){    
        TickCounter++;

        // Очистим буферы
        clearing_buffer(Acc_Buffer);
        clearing_buffer(Gyro_Buffer);

        for (index1 = 0; index1 < filter_depth; index1++){
            // Прочитаем данные акселерометра        
            ReadAcc_int16(tmp_Buffer, &sensitivity);
            add_from_buffer(Acc_Buffer, tmp_Buffer);

            // Прочитаем данные гироскопа
            ReadGyro_int16(tmp_Buffer);
            add_from_buffer(Gyro_Buffer, tmp_Buffer);
        }

        for (index1 = 0; index1 < 3; index1++){
            Acc_Buffer[index1] /= filter_depth;
            Gyro_Buffer[index1] /= filter_depth;
        }

        new_data_Flag = TRUE;
    }

    // Ниже используем index2, тк эти функции будут вызываться из цикла, в котором уже используется index1
    
    // Очистка буфера
    void clearing_buffer(int16_t *buffer){
        for (index2 = 0; index2 < 3; index2++){
            buffer[index2] = 0;
        }
    }

    // Добавление к значениям из buffer значений из add_buffer 
    void add_from_buffer(int16_t *buffer, int16_t *add_buffer){
        for (index2 = 0; index2 < 3; index2++){
            buffer[index2] += add_buffer[index2];
        }
    }
    
    // ########################################################################
    // Начальная выставка датчиков
    void initial_setting(){
        buffer_matrix.IdentityMatrix();
        LedOn(LED4);
        LedOn(LED9);

        set_zero_Data();
        set_rotationMatrix();
        rotation_matrix * zero_Data;
            
        LedOff(LED4);
        LedOff(LED9);
    }

    // Нахождение нулевых значений
    void set_zero_Data()
    {
        int degree = 10;
        int jump_mean_degree = 6; // Степень глубины прыгающего среднего

        int max = pow(2, degree - jump_mean_degree);
        int jm_max = pow(2, jump_mean_degree);

        for (index1 = 0; index1 < max; index1++)
        {
            buffer_Data.set_zero_Values();

            for (index2 = 0; index2 < jm_max; index2++)
            {
                current_Data.Read_Data();
                buffer_Data += current_Data;
            }
            buffer_Data /= jm_max;
            
            zero_Data += buffer_Data;
        }
        zero_Data /= max;
    }

    // ########################################################################
    // Нахождение матрицы перехода от СК датчиков к СК, связанную с Землей, у которой OY направлена на север, а OZ вертикально
    // Вывод коэффициентов матрицы поворота, а также описание используемых систем координат можно посмотреть в файле Documentation/Rotation_matrix.pdf
    void set_rotationMatrix()
    {        
        /* 
        Координаты векторов G, W в системе координат, связанной с платой

        G_x = zero_Data.Acc.X_coord;
        G_y = zero_Data.Acc.Y_coord;
        G_z = zero_Data.Acc.Z_coord;

        */

        // Получаем значение ускорения свободного падения способом ниже для того, чтобы не переводить значения из системы СИ в систему измерений датчиков.
        float G = sqrt(zero_Data.Acc.X_coord * zero_Data.Acc.X_coord + zero_Data.Acc.Y_coord * zero_Data.Acc.Y_coord + zero_Data.Acc.Z_coord * zero_Data.Acc.Z_coord); 
        float W = sqrt(zero_Data.Gyro.X_coord * zero_Data.Gyro.X_coord + zero_Data.Gyro.Y_coord * zero_Data.Gyro.Y_coord + zero_Data.Gyro.Z_coord * zero_Data.Gyro.Z_coord); 
        
        // Введём вспомогательный вектор А, как векторное произведение векторов W и G (см. документацию)
        float A = G * W * sin(longitude);

        float W_Y, W_Z;    // Координаты вектора W в системе координат, связанной с Землёй (W_X = 0)

        W_Y = W * sin(longitude);
        W_Z = W * cos(longitude);

        // Найдём координаты вектора а в СК датчика
        float a_x, a_y, a_z;
        a_x = zero_Data.Gyro.Y_coord * zero_Data.Acc.Z_coord - zero_Data.Gyro.Z_coord * zero_Data.Acc.Y_coord;
        a_y = zero_Data.Gyro.Z_coord * zero_Data.Acc.X_coord - zero_Data.Gyro.X_coord * zero_Data.Acc.Z_coord;
        a_z = zero_Data.Gyro.X_coord * zero_Data.Acc.Y_coord - zero_Data.Gyro.Y_coord * zero_Data.Acc.X_coord;

        rotation_matrix(0, 0) = a_x / A;
        rotation_matrix(1, 0) = a_y / A;
        rotation_matrix(2, 0) = a_z / A;

        rotation_matrix(0, 1) = (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) / W_Y;
        rotation_matrix(1, 1) = (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) / W_Y;
        rotation_matrix(2, 1) = (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) / W_Y;

        rotation_matrix(0, 2) = zero_Data.Acc.X_coord / G;
        rotation_matrix(1, 2) = zero_Data.Acc.Y_coord / G;
        rotation_matrix(2, 2) = zero_Data.Acc.Z_coord / G;

        rotation_matrix.Mreverse();
        rotation_matrix.copying_from_Buffer();
    }
};