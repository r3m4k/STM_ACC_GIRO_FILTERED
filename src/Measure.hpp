#include "Data.hpp"

class Measure : public Data 
{
public:
    Data current_Data, zero_Data, buffer_Data;

    // Данные, которые будут меняться в вызываемых функциях
    // Вынесим их в статические переменные для избежания переполнения стека процессора
    Data  temp_Data;
    float temp_matrix[9] = {0.0f};
    float temp_vector[3] = {0.0f};

    float rotation_matrix[9] = {0.0f};      // Матрица перехода от СК платы к глобальной СК, у которой ось OY направлена на север, а OZ перпендикулярно поверхности
                                            // Будем работать с матрицей 3х3 как с массивом из 9-ти элементов 
    float longitude;                        // Широта места, где будет находиться плата
    float vector[3] = {0.0f};               // Массив, в который будет преобразовываться структура Frame и обратно в случае необходимости 
    float sqrt2 = 1.41421356237;            // Тк sqrt(2) будет использоваться довольно часто, то вынесим его значение в отдельную переменную
    short i, j;

    Measure(float phi) { longitude = phi; }

    // ########################################################################
    // Выставка датчиков
    void average_Data()
    {
        char degree = 16;
        char jump_mean_degree = 6; // Степень глубины прыгающего среднего

        int max = my_pow(2, degree - jump_mean_degree);
        int jm_max = my_pow(2, jump_mean_degree);
        // int max = 1024;
        // int jm_max = 64;

        for (int index = 0; index < max; index++)
        {
            buffer_Data.set_zero_Data();

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

    void set_rotationMatrix()
    {
        // Вывод коэффицентов матрицы поворота, а также описание используемых систем координат можно посмотреть в файле Documentation/Rotation_matrix.pdf
        float matrix[9] = {0.0f};  // матрица перехода от системы координат x'y'z' к xyz
        
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
        float G = my_sqrt(zero_Data.Acc.X_coord * zero_Data.Acc.X_coord + zero_Data.Acc.Y_coord * zero_Data.Acc.Y_coord + zero_Data.Acc.Z_coord * zero_Data.Acc.Z_coord); 
        float W = my_sqrt(zero_Data.Gyro.X_coord * zero_Data.Gyro.X_coord + zero_Data.Gyro.Y_coord * zero_Data.Gyro.Y_coord + zero_Data.Gyro.Z_coord * zero_Data.Gyro.Z_coord); 
        float H = my_sqrt(zero_Data.Mag.X_coord * zero_Data.Mag.X_coord + zero_Data.Acc.Y_coord * zero_Data.Mag.Y_coord + zero_Data.Mag.Z_coord * zero_Data.Mag.Z_coord);

        float W_X, W_Y, W_Z, H_X, H_Y; // Координаты векторов W, H в системе координат, связанной с Землёй
                                    // и повёрнутой относительно направления на север на -45 градусов вокруг оси OZ (т.е. по часовой стрелке)

        W_X = W * my_cos(longitude) / sqrt2;
        W_Y = W * my_cos(longitude) / sqrt2;
        W_Z = W * my_sin(longitude);

        H_X = H / sqrt2;
        H_Y = H / sqrt2;

        matrix[0] = (H_Y * (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) - W_Y * zero_Data.Mag.X_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[0][0]
        matrix[1] = (H_Y * (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) - W_Y * zero_Data.Mag.Y_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[0][1]
        matrix[2] = (H_Y * (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) - W_Y * zero_Data.Mag.Z_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[0][2]

        matrix[3] = (H_X * (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) - W_X * zero_Data.Mag.X_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[1][0]
        matrix[4] = (H_X * (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) - W_X * zero_Data.Mag.Y_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[1][1]
        matrix[5] = (H_X * (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) - W_X * zero_Data.Mag.Z_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[1][2]

        matrix[6] = zero_Data.Gyro.X_coord / G;     // matrix[2][0]
        matrix[7] = zero_Data.Gyro.Y_coord / G;     // matrix[2][1]
        matrix[8] = zero_Data.Gyro.Z_coord / G;     // matrix[2][2]
        
        Mreverse(matrix);
        Matrix_copying(matrix, temp_matrix, 9);
        
        // Матрица перехода от xyz к XYZ
        float Matrix[9] = {0.0f};

        Matrix[0] = 1 / sqrt2;      // Matrix[0][0]
        Matrix[1] = -1 / sqrt2;     // Matrix[0][1]
        Matrix[2] = 0.0f;           // Matrix[0][2]

        Matrix[3] = 1 / sqrt2;      // Matrix[1][0]
        Matrix[4] = 1 / sqrt2;      // Matrix[1][1]
        Matrix[5] = 0.0f;           // Matrix[1][2]

        Matrix[6] = 0.0f;           // Matrix[2][0]
        Matrix[7] = 0.0f;           // Matrix[2][1]
        Matrix[8] = 1.0f;           // Matrix[2][2]
        
        Matrix_Matrix_mult(matrix, Matrix);        
        Matrix_copying(rotation_matrix, temp_matrix, 9);
    }

    // ########################################################################
    // Чтение данных и перевод их в СК Земли
    void measuring(){
        current_Data.Read_Data();
        current_Data - zero_Data;
        current_Data = temp_Data;

        Matrix_Frame_mult(rotation_matrix, current_Data.Acc);
        current_Data.Acc = temp_Frame;

        Matrix_Frame_mult(rotation_matrix, current_Data.Gyro);
        current_Data.Gyro = temp_Frame;
        
        Matrix_Frame_mult(rotation_matrix, current_Data.Mag);
        current_Data.Mag = temp_Frame;
    }

    // ########################################################################
    friend void UsartSend(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t maxValue1, uint16_t maxValue2, uint16_t maxValue3, uint16_t DPPValue1, uint16_t DPPValue2, uint16_t DPPValue3, uint16_t DPPValue4);

    void send_rotation_matrix() {
        UsartSend(rotation_matrix[0], rotation_matrix[1], rotation_matrix[2], rotation_matrix[3], rotation_matrix[4], rotation_matrix[5], rotation_matrix[6], rotation_matrix[7], rotation_matrix[8], 0);
    }

    // ########################################################################
    // Операции с матрицами
    
    // Копирование значений matrix2 в matrix1, у которых n элементов
    // PS. Эта же функция может быть использована и для векторов
    void Matrix_copying(float matrix1[], float matrix2[], int n){
        for (i = 0; i < n; i++){
            matrix1[i] = matrix2[2];
        }
    }

    // Математические операции
    // Вычисление определителя
    float Determinant(float matrix[])        // Не уверен, что & сработает, тк эта функция получает как бы указатель на указатель
    {
        return matrix[0] * (matrix[4] * matrix[8] - matrix[7] * matrix[5]) - matrix[1] * (matrix[3] * matrix[8] - matrix[6] * matrix[5]) + matrix[2] * (matrix[3] * matrix[7] - matrix[6] * matrix[4]);
    }

    // Нахождение обратной матрицы размерности 3х3
    void Mreverse(float matrix[]){
        float det = Determinant(matrix);
        
        // Заполним temp_matrix алгебраическими дополнениями матрицы matrix и транспонируем её
        temp_matrix[0] =  (matrix[4] * matrix[8] - matrix[7] * matrix[5]) / det;    //temp_matrix[0][0]
        temp_matrix[3] = -(matrix[3] * matrix[8] - matrix[6] * matrix[5]) / det;    //temp_matrix[1][0]
        temp_matrix[6] =  (matrix[3] * matrix[7] - matrix[6] * matrix[4]) / det;    //temp_matrix[2][0]

        temp_matrix[1] = -(matrix[1] * matrix[8] - matrix[7] * matrix[2]) / det;    //temp_matrix[0][1]
        temp_matrix[4] =  (matrix[0] * matrix[8] - matrix[6] * matrix[2]) / det;    //temp_matrix[1][1]
        temp_matrix[7] = -(matrix[0] * matrix[7] - matrix[6] * matrix[1]) / det;    //temp_matrix[2][1]

        temp_matrix[2] =  (matrix[1] * matrix[5] - matrix[4] * matrix[2]) / det;    //temp_matrix[0][2]
        temp_matrix[5] = -(matrix[0] * matrix[5] - matrix[3] * matrix[2]) / det;    //temp_matrix[1][2]
        temp_matrix[8] =  (matrix[0] * matrix[4] - matrix[3] * matrix[1]) / det;    //temp_matrix[2][2]
    }

    // Умножение  матриц размерности 3х3
    void Matrix_Matrix_mult(float matrix1[], float matrix2[]){
        temp_matrix[0] = matrix1[0] * matrix2[0] + matrix1[1] * matrix2[3] + matrix1[2] * matrix2[6];
        temp_matrix[1] = matrix1[0] * matrix2[1] + matrix1[1] * matrix2[4] + matrix1[2] * matrix2[7];
        temp_matrix[2] = matrix1[0] * matrix2[2] + matrix1[1] * matrix2[5] + matrix1[2] * matrix2[8];
        
        temp_matrix[3] = matrix1[3] * matrix2[0] + matrix1[4] * matrix2[3] + matrix1[5] * matrix2[6];
        temp_matrix[4] = matrix1[3] * matrix2[1] + matrix1[4] * matrix2[4] + matrix1[5] * matrix2[7];
        temp_matrix[5] = matrix1[3] * matrix2[2] + matrix1[4] * matrix2[5] + matrix1[5] * matrix2[8];

        temp_matrix[6] = matrix1[6] * matrix2[0] + matrix1[7] * matrix2[3] + matrix1[8] * matrix2[6];
        temp_matrix[7] = matrix1[6] * matrix2[1] + matrix1[7] * matrix2[4] + matrix1[8] * matrix2[7];
        temp_matrix[8] = matrix1[6] * matrix2[2] + matrix1[7] * matrix2[5] + matrix1[8] * matrix2[8];
    }

    // Умножение квадратной матрицы размерности 3х3 на vector
    void Matrix_Vector_mult(float matrix[], float vector[]){
        temp_vector[0] = matrix[0] * vector[0] + matrix[1] * vector[1] + matrix[2] * vector[2];
        temp_vector[1] = matrix[3] * vector[0] + matrix[4] * vector[1] + matrix[5] * vector[2];
        temp_vector[2] = matrix[6] * vector[0] + matrix[7] * vector[1] + matrix[8] * vector[2];
    }

    // Умножение квадратной матрицы размерности 3х3 на frame
    void Matrix_Frame_mult(float matrix[], Frame& frame){
        temp_Frame[0] = matrix[0] * frame[0] + matrix[1] * frame[1] + matrix[2] * frame[2];
        temp_Frame[1] = matrix[3] * frame[0] + matrix[4] * frame[1] + matrix[5] * frame[2];
        temp_Frame[2] = matrix[6] * frame[0] + matrix[7] * frame[1] + matrix[8] * frame[2];
    }
};