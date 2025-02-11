#include "Matrix.hpp"

// #define DATA_PROCESSING
// #define USING_DPP

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
    float shift = 0;                // Погрешность, накапливаемая при работе датчиков, которую необходимо компенсировать
    // -------------------------------------------------------------------------------
    bool new_tick_Flag = FALSE;     // Флаг, отвечающий за наличие нового прерывания от таймера
    bool new_DPP_Flag  = FALSE;     // Флаг, отвечающие за наличие нового кода от ДПП
    // -------------------------------------------------------------------------------
    // Переменные для работы с прерываниями от таймера
    float period;                   // Период изменения TickCounter в секундах
    uint32_t TickCounter;           // Счётчик тиков запущенного таймера с периодом period (см main.cpp --> TimerInit())
    uint16_t filter_depth = 1024;   // Глубина усреднения
    // -------------------------------------------------------------------------------
    // Переменные для работы с прерываниями от USART (канал связи с ДПП)
    uint32_t _DppCode;              // Код от ДПП
    float offset = OFFSET_VALUE;    // Перемещение от прошлого сигнала 
    float displacement[3] = {0};    // Изменение координат от предыдущей посылки ДПП
    // -------------------------------------------------------------------------------
    uint16_t index1, index2;        // Индексы для циклов (объявляем здесь чтобы не выделять под них постоянно память в ходе программы) 
    // -------------------------------------------------------------------------------

    // ########################################################################
    // Конструктор класса и другие функции для работы с параметрами класса
    Measure(float phi, float _period) { 
        longitude = phi; 
        period = _period;          
        buffer_matrix.IdentityMatrix();
    }

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
            // current_Data.sending_USB();

#ifdef DATA_PROCESSING
            if (new_tick_Flag)
            {
                // Считаем данные и отфильтруем их

                // Пока в качестве фильтрации будем использовать усреднение
                buffer_Data.set_zero_Values();
                for (index1 = 0; index1 < filter_depth; index1++){
                    current_Data.Read_Data();
                    buffer_Data += current_Data;
                }

                buffer_Data /= filter_depth;

                // Начнём обработку новых данных
                
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
                    Coordinates[index1] += Velocity[index1] * period + buffer_Data.Acc[index1] * period * period / 2 - shift;    
                    Velocity[index1] += buffer_Data.Acc[index1] * period;
                    displacement[index1] += Velocity[index1] * period + buffer_Data.Acc[index1] * period * period / 2; 
                }
                new_tick_Flag = FALSE;
            }
#endif      // DATA_PROCESSING

#ifdef USING_DPP
            if (new_DPP_Flag)
            {
                // Начало обработки нового кода ДПП
                /* 
                Решим уравнение s**2 = (x-a)**2 + (y-a)**2 + (z-a)**2
                где а - искомое смещение, обусловленное накапливаемой погрешностью акселерометра            ??? Как корректировать гироскоп
                s = offset
                x = displacement[0]
                y = displacement[1]
                z = displacement[2]
                a = shift 

                Решения этого уравнения: а = (x + y + z +- sqrt(-2*(x**2 + y**2 + z**2 - x*y - x*z - y*z) + 3*s**2) ) / 3
                Возьмём бОльший корень уравнения (со знаком "+")
                */

                shift = (displacement[0] + displacement[1] + displacement[2] + \
                            sqrt(-2*(displacement[0]*displacement[0] + displacement[1]*displacement[1] + displacement[2]*displacement[2] - \
                            displacement[0] * displacement[1] - displacement[0] * displacement[2] - displacement[1] * displacement[2]) + \
                            3 * offset*offset) ) / 3;

                // Завершение обработки нового кода ДПП
                for (index1 = 0; index1 < 3; index1++){
                    displacement[index1] = 0;
                }
                new_DPP_Flag = FALSE;
            }            
#endif      // USING_DPP

            LedOn(LED8);
        }
    }

    // ########################################################################
    // Начальная выставка датчиков
    void initial_setting(){
        LedOn(LED4);
        LedOn(LED9);

        set_zero_Data();
        set_rotationMatrix();
        rotation_matrix * zero_Data;
            
        LedOff(LED4);
        LedOff(LED9);
    }

    // ########################################################################
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