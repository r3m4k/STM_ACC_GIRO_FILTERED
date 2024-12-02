#include "Matrix.hpp"

extern float sqrt2;

class Measure 
{
public:
    // Данные, которые будут меняться в вызываемых функциях
    // Вынесим их в статические переменные для избежания переполнения стека процессора
    Data current_Data, zero_Data, buffer_Data;
    Matrix temp_matrix;

    Matrix rotation_matrix;         // Матрица перехода от СК платы к глобальной СК, у которой ось OY направлена на север, а OZ перпендикулярно поверхности
    float longitude;                // Широта места, где будет находиться плата

    Measure(float phi) { longitude = phi; }

    // ########################################################################
    // Начальная выставка датчиков
    void initial_setting(){
        set_zero_Data();
        set_rotationMatrix();
    }

    // Нахождение нулевых значений
    void set_zero_Data()
    {
        int degree = 12;
        int jump_mean_degree = 6; // Степень глубины прыгающего среднего

        int max = pow(2, degree - jump_mean_degree);
        int jm_max = pow(2, jump_mean_degree);
        // int max = 1024;
        // int jm_max = 64;

        for (int index = 0; index < max; index++)
        {
            buffer_Data.set_zero_Values();

            for (int _index = 0; _index < jm_max; _index++)
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
    // Чтение данных и перевод их в СК Земли
    // void measuring(){
    //     current_Data.Read_Data();
    //     current_Data - zero_Data;
    //     current_Data = temp_Data;

    //     Matrix_Frame_mult(rotation_matrix, current_Data.Acc);
    //     current_Data.Acc = temp_Frame;

    //     Matrix_Frame_mult(rotation_matrix, current_Data.Gyro);
    //     current_Data.Gyro = temp_Frame;
        
    //     Matrix_Frame_mult(rotation_matrix, current_Data.Mag);
    //     current_Data.Mag = temp_Frame;
    // }

    // ########################################################################
    friend void UsartSend(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t maxValue1, uint16_t maxValue2, uint16_t maxValue3, uint16_t DPPValue1, uint16_t DPPValue2, uint16_t DPPValue3, uint16_t DPPValue4);

    void send_rotation_matrix() {
        UsartSend(rotation_matrix[0], rotation_matrix[1], rotation_matrix[2], rotation_matrix[3], rotation_matrix[4], rotation_matrix[5], rotation_matrix[6], rotation_matrix[7], rotation_matrix[8], 0);
    }

    // ########################################################################
    // Нахождение матрицы перехода от СК датчиков к СК, связанную с Землей, у которой OY направлена на север, а OZ вертикально
    // Вывод коэффицентов матрицы поворота, а также описание используемых систем координат можно посмотреть в файле Documentation/Rotation_matrix.pdf
    void set_rotationMatrix()
    {        
        /* 
        Координаты векторов G, W, H в системе координат, связанной с платой

        G_x = zero_Data.Acc.X_coord;
        G_y = zero_Data.Acc.Y_coord;
        G_z = zero_Data.Acc.Z_coord;

        W_x = zero_Data.Gyro.X_coord;
        W_y = zero_Data.Gyro.Y_coord;
        W_z = zero_Data.Gyro.Z_coord;

        H_x = zero_Data.Mag.X_coord;
        H_y = zero_Data.Mag.Y_coord;
        H_z = zero_Data.Mag.Z_coord;
        */

        // Получаем значение всемирных констант способом ниже для того, чтобы не переводить значение этих констант из системы СИ в систему измерений датчиков.
        float G = sqrt(zero_Data.Acc.X_coord * zero_Data.Acc.X_coord + zero_Data.Acc.Y_coord * zero_Data.Acc.Y_coord + zero_Data.Acc.Z_coord * zero_Data.Acc.Z_coord); 
        G = 1;      // Тк пока что акселерометр считывает только нули, поэтому во избежание деления на нуль зададим значение G = 1
        float W = sqrt(zero_Data.Gyro.X_coord * zero_Data.Gyro.X_coord + zero_Data.Gyro.Y_coord * zero_Data.Gyro.Y_coord + zero_Data.Gyro.Z_coord * zero_Data.Gyro.Z_coord); 
        float H = sqrt(zero_Data.Mag.X_coord * zero_Data.Mag.X_coord + zero_Data.Acc.Y_coord * zero_Data.Mag.Y_coord + zero_Data.Mag.Z_coord * zero_Data.Mag.Z_coord);

        float W_X, W_Y, W_Z, H_X, H_Y;  // Координаты векторов W, H в системе координат, связанной с Землёй
                                        // и повёрнутой относительно направления на север на -45 градусов вокруг оси OZ (т.е. по часовой стрелке)

        W_X = W * cos(longitude) / sqrt2;
        W_Y = W * cos(longitude) / sqrt2;
        W_Z = W * sin(longitude);

        H_X = H / sqrt2;
        H_Y = H / sqrt2;

        rotation_matrix[0] = (H_Y * (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) - W_Y * zero_Data.Mag.X_coord) / (W_X * H_Y - W_Y * H_X);     // rotation_matrix[0][0]
        rotation_matrix[1] = (H_Y * (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) - W_Y * zero_Data.Mag.Y_coord) / (W_X * H_Y - W_Y * H_X);     // rotation_matrix[0][1]
        rotation_matrix[2] = (H_Y * (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) - W_Y * zero_Data.Mag.Z_coord) / (W_X * H_Y - W_Y * H_X);     // rotation_matrix[0][2]

        rotation_matrix[3] = (H_X * (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) - W_X * zero_Data.Mag.X_coord) / (W_X * H_Y - W_Y * H_X);     // rotation_matrix[1][0]
        rotation_matrix[4] = (H_X * (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) - W_X * zero_Data.Mag.Y_coord) / (W_X * H_Y - W_Y * H_X);     // rotation_matrix[1][1]
        rotation_matrix[5] = (H_X * (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) - W_X * zero_Data.Mag.Z_coord) / (W_X * H_Y - W_Y * H_X);     // rotation_matrix[1][2]

        rotation_matrix[6] = zero_Data.Gyro.X_coord / G;     // temp_matrix[2][0]
        rotation_matrix[7] = zero_Data.Gyro.Y_coord / G;     // temp_matrix[2][1]
        rotation_matrix[8] = zero_Data.Gyro.Z_coord / G;     // temp_matrix[2][2]
        
        rotation_matrix.Mreverse();
        rotation_matrix.copying_from_Buffer();
        
        temp_matrix[0] = 1 / sqrt2;      // temp_matrix[0][0]
        temp_matrix[1] = -1 / sqrt2;     // temp_matrix[0][1]
        temp_matrix[2] = 0.0f;           // temp_matrix[0][2]

        temp_matrix[3] = 1 / sqrt2;      // temp_matrix[1][0]
        temp_matrix[4] = 1 / sqrt2;      // temp_matrix[1][1]
        temp_matrix[5] = 0.0f;           // temp_matrix[1][2]

        temp_matrix[6] = 0.0f;           // temp_matrix[2][0]
        temp_matrix[7] = 0.0f;           // temp_matrix[2][1]
        temp_matrix[8] = 1.0f;           // temp_matrix[2][2]
        
        
        rotation_matrix * temp_matrix;      
        rotation_matrix.copying_from_Buffer();
    }
};