#include <math.h>
#include "main.h"


struct Frame
{
    float X_coord;
    float Y_coord;
    float Z_coord;
};

struct Data
{
    Frame Acc;
    Frame Gyro;
    Frame Mag;
};

class Measure
{
protected:
    Data current_Data, zero_Data, buffer_Data;
    
    // Данные, которые будут меняться в вызываемых функциях
    // Вынесим их в статические переменные для избежания переполнения стека процессора
    Frame temp_Frame;     
    Data  temp_Data;
    float temp_matrix[9] = {0.0f};
    float temp_vector[3] = {0.0f};

    float rotation_matrix[9] = {0.0f};      // Матрица перехода от СК платы к глобальной СК, у которой ось OY направлена на север, а OZ перпендикулярно поверхности
                                            // Будем работать с матрицей 3х3 как с массивом из 9-ти элементов 
    float longitude;                        // Широта места, где будет находиться плата
    float buffer[3] = {0.0f};               // Буффер, который будет использоваться при чтении данных с датчиков
    float sqrt2 = sqrt(2);                  // Тк sqrt(2) будет использоваться довольно часто, то вынесим его значение в отдельную переменную
    short i, j;

public:
    Measure(float phi)
    {
        longitude = phi;    
        
        // Зададим значения для temp_Data
        temp_Data.Acc.X_coord = 0;
        temp_Data.Acc.Y_coord = 0;
        temp_Data.Acc.Z_coord = 0;

        temp_Data.Gyro.X_coord = 0;
        temp_Data.Gyro.Y_coord = 0;
        temp_Data.Gyro.Z_coord = 0;

        temp_Data.Mag.X_coord = 0;
        temp_Data.Mag.Y_coord = 0;
        temp_Data.Mag.Z_coord = 0;

        // Зададим значения для temp_Frame
        temp_Frame.X_coord = 0;
        temp_Frame.Y_coord = 0;
        temp_Frame.Z_coord = 0;


    }

    // ########################################################################
    // Функционал для Data
    void set_zero_Data()
    {
        temp_Data.Acc.X_coord = 0;
        temp_Data.Acc.Y_coord = 0;
        temp_Data.Acc.Z_coord = 0;

        temp_Data.Gyro.X_coord = 0;
        temp_Data.Gyro.Y_coord = 0;
        temp_Data.Gyro.Z_coord = 0;

        temp_Data.Mag.X_coord = 0;
        temp_Data.Mag.Y_coord = 0;
        temp_Data.Mag.Z_coord = 0;
    }

    // Data get_real_Data()
    // {

    // }

    // ########################################################################
    // Чтение данных с датчиков

    friend void ReadAcc(float *pfData);
    friend void ReadGyro(float *pfData);
    friend void ReadMag(float *pfData);

    void Read_Data(Data &data)
    {
        Read_Acc(data.Acc);
        Read_Gyro(data.Gyro);
        Read_Mag(data.Mag);
    }

    void Read_Acc(Frame &frame)
    {
        ReadAcc(buffer);
        frame.X_coord = buffer[0];
        frame.Y_coord = buffer[1];
        frame.Z_coord = buffer[2];
    }

    void Read_Gyro(Frame &frame)
    {
        ReadGyro(buffer);
        frame.X_coord = buffer[0];
        frame.Y_coord = buffer[1];
        frame.Z_coord = buffer[2];
    }

    void Read_Mag(Frame &frame)
    {
        ReadMag(buffer);
        frame.X_coord = buffer[0];
        frame.Y_coord = buffer[1];
        frame.Z_coord = buffer[2];
    }

    // ########################################################################
    // Математические операции с Data и Frame

    // Сложение Data
    void Data_summ(Data &data1, Data &data2)
    {
        Frame_summ(data1.Acc, data2.Acc);
        temp_Data.Acc = temp_Frame; 

        Frame_summ(data1.Gyro, data2.Gyro);
        temp_Data.Gyro = temp_Frame;
        
        Frame_summ(data1.Mag, data2.Mag);
        temp_Data.Mag = temp_Frame;
    }
    
    // Сложение Frame
    void Frame_summ(Frame &frame1, Frame &frame2)
    {
        temp_Frame.X_coord = frame1.X_coord + frame2.X_coord;
        temp_Frame.Y_coord = frame1.Y_coord + frame2.Y_coord;
        temp_Frame.Z_coord = frame1.Z_coord + frame2.Z_coord;
    }

    // Вычитание Data
    void Data_diff(Data &data1, Data &data2)
    {
        Frame_diff(data1.Acc, data2.Acc);
        temp_Data.Acc = temp_Frame; 

        Frame_diff(data1.Gyro, data2.Gyro);
        temp_Data.Gyro = temp_Frame;
        
        Frame_diff(data1.Mag, data2.Mag);
        temp_Data.Mag = temp_Frame;
    }
    
    // Вычитание Frame
    void Frame_diff(Frame &frame1, Frame &frame2)
    {
        temp_Frame.X_coord = frame1.X_coord + frame2.X_coord;
        temp_Frame.Y_coord = frame1.Y_coord + frame2.Y_coord;
        temp_Frame.Z_coord = frame1.Z_coord + frame2.Z_coord;
    }

    // Деление Data на число
    void Data_division(Data &data, float num)
    {
        Frame_division(data.Acc, num);
        Frame_division(data.Gyro, num);
        Frame_division(data.Mag, num);
    }

    // Деление Data на число
    void Frame_division(Frame &frame, float num)
    {
        frame.X_coord /= num;
        frame.Y_coord /= num;
        frame.Z_coord /= num;
    }

    // ########################################################################
    // Выставка датчиков
    void average_Data()
    {
        char degree = 16;
        char jump_mean_degree = 6; // Степень глубины прыгающего среднего

        int max = pow(2, degree - jump_mean_degree);
        int jm_max = pow(2, jump_mean_degree);

        for (int index = 0; index < max; index++)
        {
            set_zero_Data();
            buffer_Data = temp_Data;

            for (int j = 0; j < jm_max; j++)
            {
                Read_Data(current_Data);
                Data_summ(current_Data, buffer_Data);
                buffer_Data = temp_Data;
            }

            Data_division(buffer_Data, jm_max);

            Data_summ(zero_Data, buffer_Data);
            zero_Data = temp_Data; 
        }

        Data_division(zero_Data, max);
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
        float G = sqrt(zero_Data.Acc.X_coord * zero_Data.Acc.X_coord + zero_Data.Acc.Y_coord * zero_Data.Acc.Y_coord + zero_Data.Acc.Z_coord * zero_Data.Acc.Z_coord); 
        float W = sqrt(zero_Data.Gyro.X_coord * zero_Data.Gyro.X_coord + zero_Data.Gyro.Y_coord * zero_Data.Gyro.Y_coord + zero_Data.Gyro.Z_coord * zero_Data.Gyro.Z_coord); 
        float H = sqrt(zero_Data.Mag.X_coord * zero_Data.Mag.X_coord + zero_Data.Acc.Y_coord * zero_Data.Mag.Y_coord + zero_Data.Mag.Z_coord * zero_Data.Mag.Z_coord);

        float W_X, W_Y, W_Z, H_X, H_Y; // Координаты векторов W, H в системе координат, связанной с Землёй
                                       // и повёрнутой относительно направления на север на -45 градусов вокруг оси OZ (т.е. по часовой стрелке)

        W_X = W * cos(longitude) / sqrt2;
        W_Y = W * cos(longitude) / sqrt2;
        W_Z = W * sin(longitude);

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
        for (i = 0; i < 9; i++){
            matrix[i] = temp_matrix[i];
        }

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
        for (i = 0; i < 9; i++){
            rotation_matrix[i] = temp_matrix[i];
        }
    }

    // ########################################################################
    // Математические операции с матрицами

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

    // Умножение квадратной матрицы размерности 3х3 на вектор размерности 3
    void Matrix_Vector_mult(float matrix[], float vector[]){
        temp_vector[0] = matrix[0] * vector[0] + matrix[1] * vector[1] + matrix[2] * vector[2];
        temp_vector[1] = matrix[3] * vector[0] + matrix[4] * vector[1] + matrix[5] * vector[2];
        temp_vector[2] = matrix[6] * vector[0] + matrix[7] * vector[1] + matrix[8] * vector[2];
    }
    // ########################################################################
};
