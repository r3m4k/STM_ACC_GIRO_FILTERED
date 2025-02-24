#include "Matrix.hpp"

#define DATA_FILTERING
// #define DATA_PROCESSING
// #define USING_DPP
#define ERROR_CALCULATION

#define OFFSET_VALUE    0           // Фиктивный пройденный путь (в метрах) между сигналами от ДПП
// #define OFFSET_VALUE    0.2256      // Пройденный путь (в метрах) между сигналами от ДПП

/*  Фильтрация входного потока данных   */
// #define COMPLEX_FILTER
#define FilterSize          64      // ВАЖНО!!! Значение FilterSize должно нацело делиться на FilterFrameSize
#define FilterFrameSize     16
#define rolling_n           4
#define n_sigma             2.0f

#define X_COORD             (int) 0
#define Y_COORD             (int) 1
#define Z_COORD             (int) 2

#define TEMP_DELTA          1.0f                     


class Measure 
{
public:
    // -------------------------------------------------------------------------------
    // Данные, которые будут меняться в вызываемых функциях
    // Вынесем их в статические переменные для избежания переполнения стека процессора
    Data current_Data, zero_Data, buffer_Data;
    Matrix tmp_matrix;    
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
    // Переменные для фильтрации данных
    struct FilterData               // Структура для хранения данных
    {
        float Acc_Buffer[3];        // Буфер для данных с акселерометра
        float Gyro_Buffer[3];       // Буфер для данных с гироскопа
    }FilterFrame[FilterSize];
   
    uint8_t Filter_counter = 0;
    uint8_t temp_counter = 0;

    float Temp_Buffer[FilterSize];
    float Temp_Delta_sigh = 1;
    bool Full_Temp_Buffer = FALSE;  // Флаг заполнения буфера температуры
    float temp_buffer[FilterSize / FilterFrameSize];

    // Вспомогательные буферы для работы фильтра
    float tmp_Buffer[FilterSize];   
    float flt_Buffer[FilterSize];
    float tmp_frame_Buffer[FilterFrameSize];
    float flt_frame_Buffer[FilterFrameSize];
    
    int tmp_size;                   // Размер заполненных данных в tmp_Buffer / tmp_frame_Buffer в определённый момент времени
    int flt_size;                   // Размер заполненных данных в flt_Buffer / flt_frame_Buffer в определённый момент времени
    int frame_counter;              // Счётчик кусков данных

#ifdef ERROR_CALCULATION
    float Acc_std[3];               // СКО значений акселерометра
    float Gyro_std[3];              // СКО значений гироскопа
#endif      /* ERROR_CALCULATION */
    // -------------------------------------------------------------------------------
    // Вспомогательные переменные (объявляем здесь чтобы не выделять под них постоянно память в ходе программы) 
    uint16_t index1, index2;        // Индексы для циклов 
    float tmp_float;
    // -------------------------------------------------------------------------------

    // ########################################################################
    // Конструктор класса и другие функции для работы с параметрами класса
    Measure(float phi, float _period) { 
        longitude = phi; 
        period = _period;          
        buffer_matrix.IdentityMatrix();
        current_Data.Temp_counter = 255;       // Значение, при котором задаётся начальная температура
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
            new_tick_Flag = TRUE;
            
#ifdef DATA_FILTERING
            if (new_tick_Flag)       
            /*
            Результаты испытаний времени выполнения обработки новых данных без вычисления погрешности:
            440 мс при FilterSize = 128 / FilterFrameSize = 16 / rolling_n = 8
            220 мс при FilterSize = 64  / FilterFrameSize = 16 / rolling_n = 8
            110 мс при FilterSize = 32  / FilterFrameSize = 16 / rolling_n = 8
            110 мс при FilterSize = 32  / FilterFrameSize = 8  / rolling_n = 4
            55  мс при FilterSize = 16  / FilterFrameSize = 8  / rolling_n = 4
            Самой оптимальной является третья конфигурация, тк за это время человек и двигаясь со скоростью 5 км/ч, пройдёт примерно 18 см, 
            что примерно равно пройденному расстоянию между двумя сигналами от ДПП, поэтому будем работать с ней.
            Тогда можно выставить период таймера в 120 мс. В этом случае будет достаточно времени на компенсацию смещений и останется небольшой запас (наверное).
            */
            {
                LedOn(LED5);
                
                data_collecting();
                data_filtering(); 
#ifdef DATA_PROCESSING
                data_processing();
#endif      /* DATA_PROCESSING */                
                LedOff(LED5);
            }
#endif      /* DATA_FILTERING */

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
#endif      /* USING_DPP */ 

            if (Full_Temp_Buffer){
                mean(Temp_Buffer, FilterSize);
                buffer_Data.Temp = tmp_float;

                if (abs(buffer_Data.Temp - zero_Data.Temp) > TEMP_DELTA){

                    if (buffer_Data.Temp > zero_Data.Temp){
                        Temp_Delta_sigh = 1;
                    }
                    else{   
                        Temp_Delta_sigh = -1;
                    }
                    
                    zero_Data.Temp += Temp_Delta_sigh * TEMP_DELTA;
                    zero_Data.update_zero_level(Temp_Delta_sigh * TEMP_DELTA);
                }
                Full_Temp_Buffer = FALSE;
            }

            // buffer_Data -= zero_Data;
            buffer_Data.sending_USB();

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

        zero_Data.Temp_Previous = zero_Data.Temp;
        current_Data.Temp_Previous = zero_Data.Temp;
        buffer_Data.Temp_Previous = zero_Data.Temp;
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

    // ########################################################################
    // Сбор сырых данных
    void data_collecting(){

        // Заполним буферы
        current_Data.Temp_counter = 0;
        temp_counter = 0;
        for (index1 = 0; index1 < FilterSize; index1++){
            current_Data.Read_Data();
            for (index2 = 0; index2 < 3; index2++){
                FilterFrame[index1].Acc_Buffer[index2]  = current_Data(0, index2);
                FilterFrame[index1].Gyro_Buffer[index2] = current_Data(1, index2);
            }
            if (!(index1 % FilterFrameSize)){
                temp_buffer[temp_counter++] = current_Data.Temp;
            }
        }

        mean(temp_buffer, FilterSize / FilterFrameSize);
        buffer_Data.Temp = tmp_float;
        Temp_Buffer[Filter_counter++] = buffer_Data.Temp;
        if (Filter_counter == FilterSize){
            Filter_counter = 0;
            Full_Temp_Buffer = TRUE; 
        }
    }

    // ########################################################################
    // Обработка отфильтрованных данных
    void data_processing(){

        // Вычислим угол поворота за время period, опираясь на данные полученные только на части этого промежутка
        // При этом считаем, что за время period не произойдёт сильных изменений ни угловых скоростей, ни ускорений
        for (index1 = 0; index1 < 3; index1++){
            Angles[index1] = buffer_Data.Gyro[index1] * period;
        }
        
        // Далее считаем, что tmp_matrix - матрица поворота от i-го состояния в i-1 состояние, соответствущее прошлой итерации обработки данных
        // Заполним матрицу tmp_matrix углами поворота вокруг XYZ (https://ru.wikipedia.org/wiki/Матрица_поворота)
        tmp_matrix(0, 0) =  cos(Angles[1]) * cos(Angles[2]);
        tmp_matrix(0, 1) = -sin(Angles[2]) * cos(Angles[1]);
        tmp_matrix(0, 2) =  sin(Angles[1]);

        tmp_matrix(1, 0) =  sin(Angles[0]) * sin(Angles[1]) * cos(Angles[2]) + sin(Angles[2]) * cos(Angles[0]);
        tmp_matrix(1, 1) = -sin(Angles[0]) * sin(Angles[1]) * sin(Angles[2]) + cos(Angles[0]) * cos(Angles[2]);
        tmp_matrix(1, 2) = -sin(Angles[0]) * cos(Angles[1]);

        tmp_matrix(2, 0) =  sin(Angles[0]) * sin(Angles[2]) - sin(Angles[1]) * cos(Angles[0]) * cos(Angles[2]);
        tmp_matrix(2, 1) =  sin(Angles[0]) * cos(Angles[2]) + sin(Angles[1]) * sin(Angles[2]) * cos(Angles[0]);
        tmp_matrix(2, 2) =  cos(Angles[0]) * cos(Angles[1]);

        tmp_matrix *= buffer_matrix;       // Получили матрицу поворота в исходное состояние, при котором проводилась выставка
        buffer_matrix = tmp_matrix;        // Сохраним значения tmp_matrix в buffer_matrix, чтобы корректно перейти к i+1 состоянию
        
        tmp_matrix *= rotation_matrix;     // Получили матрицу поворота в СК Земли из i-го состояния

        // Вычислим приращение координат за время period в СК Земли
        tmp_matrix *= buffer_Data.Acc;     // Значение ускорений в СК Земли

        // Заполним данные о координатах и скоростях                
        for (index1 = 0; index1 < 3; index1++){
            Coordinates[index1] += Velocity[index1] * period + buffer_Data.Acc[index1] * period * period / 2 - shift;    
            Velocity[index1] += buffer_Data.Acc[index1] * period;
            displacement[index1] += Velocity[index1] * period + buffer_Data.Acc[index1] * period * period / 2; 
        }
    }

    // ########################################################################
    // Фильтрация входного потока данных
    /* Описание алгоритма фильтрации данных:
    Будем работать с частями буфера размера FilterFrameSize. Считаем, что данные на этом отрезке распределены нормально.
    Будем учитывать данные, отличающиеся от среднего значения только на n_sigma сигм (см ./Documentation/sigma_rule.jpg).
    Аналогичные действия выполняются со всеми частями буферов.
    
    Затем мы получаем данные без резких выбросов. По этому набору мы вычисляем скользящее среднее и вычисляем его среднее арифметическое.
    Это значение и будет принято за истинное значение на данном промежутке
    */
    void data_filtering(){

        // Получим истинные значения данных с акселерометра по осям
        AccXYZ_filtering(X_COORD);
        AccXYZ_filtering(Y_COORD);
        AccXYZ_filtering(Z_COORD);

        // Получим истинные значения данных с гироскопа по осям
        GyroXYZ_filtering(X_COORD);
        GyroXYZ_filtering(Y_COORD);
        GyroXYZ_filtering(Z_COORD);
    }

#ifdef COMPLEX_FILTER
    // Фильтрация данных одной координаты coord с акселерометра с последующим сохранением полученного значения в buffer_Data.Acc
    void AccXYZ_filtering(int coord){
        frame_counter = 0;
        flt_size = 0;

        // Отфильтруем резкие выбросы из Acc_Buffer[coord] и сохраним эти значения в flt_Buffer. Всего будет flt_size отфильтрованных значений
        for (index1 = 0; index1 < (FilterSize / FilterFrameSize); index1++){
            // Скопируем index1-ую часть буфера
            for (index2 = 0; index2 < FilterFrameSize; index2++){
                flt_frame_Buffer[index2] = FilterFrame[frame_counter * FilterFrameSize + index2].Acc_Buffer[coord];
            }
            tmp_size = 0;       // Размер заполненных данных в tmp_frame_Buffer
            sharp_emission_filter();

            for (index2 = 0; index2 < tmp_size; index2++){
                flt_Buffer[flt_size + index2] = tmp_frame_Buffer[index2];
            }
            
            flt_size += tmp_size;
            frame_counter++;
        }

        rolling_mean();
        mean(tmp_Buffer, tmp_size);

        buffer_Data.Acc[coord] = tmp_float;
    }

    // Фильтрация данных одной координаты coord с гироскопа с последующим сохранением полученного значения в buffer_Data.Gyro
    void GyroXYZ_filtering(int coord){
        frame_counter = 0;
        flt_size = 0;

        // Отфильтруем резкие выбросы из Gyro_Buffer[coord] и сохраним эти значения в flt_Buffer. Всего будет flt_size отфильтрованных значений
        for (index1 = 0; index1 < (FilterSize / FilterFrameSize); index1++){
            // Скопируем index1-ую часть буфера
            for (index2 = 0; index2 < FilterFrameSize; index2++){
                flt_frame_Buffer[index2] = FilterFrame[frame_counter * FilterFrameSize + index2].Gyro_Buffer[coord];
            }
            tmp_size = 0;       // Размер заполненных данных в tmp_frame_Buffer
            sharp_emission_filter();

            for (index2 = 0; index2 < tmp_size; index2++){
                flt_Buffer[flt_size + index2] = tmp_frame_Buffer[index2];
            }
            
            flt_size += tmp_size;
            frame_counter++;
        }

        rolling_mean();
        mean(tmp_Buffer, tmp_size);

        buffer_Data.Gyro[coord] = tmp_float;
    }
#endif  /*  COMPLEX_FILTER  */

#ifndef COMPLEX_FILTER
    // Фильтрация данных просто по среднему арифметическому
    void AccXYZ_filtering(int coord){
        for (index1 = 0; index1 < FilterSize; index1++){
            flt_Buffer[index1] = FilterFrame[index1].Acc_Buffer[coord];
        }
        mean(flt_Buffer, FilterSize);
        buffer_Data.Acc[coord] = tmp_float;
    }

    // Фильтрация данных просто по среднему арифметическому
    void GyroXYZ_filtering(int coord){
        for (index1 = 0; index1 < FilterSize; index1++){
            flt_Buffer[index1] = FilterFrame[index1].Gyro_Buffer[coord];
        }
        mean(flt_Buffer, FilterSize);
        buffer_Data.Gyro[coord] = tmp_float;
    }
#endif  /*  COMPLEX_FILTER  */

    // Фильтр резких выбросов значений из массива flt_frame_Buffer с сохранением в tmp_frame_Buffer
    void sharp_emission_filter(){
        mean(flt_frame_Buffer, FilterFrameSize);
        float mean_value = tmp_float;     
        
        std(flt_frame_Buffer, mean_value, FilterFrameSize);
        float std_value = tmp_float;

        for (index2 = 0; index2 < FilterFrameSize; index2++){
            if ((flt_frame_Buffer[index2] > (mean_value - n_sigma * std_value)) && (flt_frame_Buffer[index2] < (mean_value + n_sigma * std_value))){
                tmp_frame_Buffer[tmp_size++] = flt_frame_Buffer[index2];
            }
        }
    }

    // Вычисление плавающего среднего данных из flt_Buffer с сохранением в tmp_Buffer
    void rolling_mean(){
        tmp_size = flt_size - rolling_n + 1;
        tmp_float = 0;
        for (index2 = 0; index2 < tmp_size; index2++){
            for (int i = 0; i < rolling_n; i++){    tmp_float += flt_Buffer[index2 + i];    }
            tmp_float /= rolling_n;
            tmp_Buffer[index2] = tmp_float;
            tmp_float = 0;
        }
    }

    // Вычисление среднего арифметического с сохранением результата в tmp_float
    void mean(float *arr, int len){
        tmp_float = 0;
        for (index2 = 0; index2 < len; index2++){
            tmp_float += arr[index2];
        }
        tmp_float /= len;
    }

    // Вычисление среднеквадратического отклонения с сохранением результата в tmp_float
    void std(float *arr, float mean_value, int len){
        tmp_float = 0;
        for (index2 = 0; index2 < len; index2++){
            tmp_float += (arr[index2] - mean_value) * (arr[index2] - mean_value);
        }
        tmp_float = sqrt(tmp_float / len);
    }
};