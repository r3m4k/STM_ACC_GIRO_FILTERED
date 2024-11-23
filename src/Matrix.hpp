#include "Data.hpp"

class Matrix{
public:
    float matrix[9] = {0.0f};
    float matrix_Buffer[9] = {0.0f};
    short matrix_Index = 0;

    // ########################################################################
    // Перегрузка операторов

    // Умножение data.Acc, data.Gyro, data.Mag на матрицу с сохранением 
    // результата в data.Acc_Buffer, data.Gyro_Buffer, data.Mag_Buffer
    void operator*(const Data& data){
        data.Acc_Buffer = matrix[0] * data.Acc[0] + matrix[1] * data.Acc[1] + matrix[2] * data.Acc[2];
        data.Gyro_Buffer = matrix[0] * data.Gyro[0] + matrix[1] * data.Gyro[1] + matrix[2] * data.Gyro[2];
        data.Mag_Buffer = matrix[0] * data.Mag[0] + matrix[1] * data.Mag[1] + matrix[2] * data.Mag[2];
    }

    // Умножение data.Acc, data.Gyro, data.Mag на матрицу с изменением 
    // значений data.Acc, data.Gyro, data.Mag
    void operator*=(const Data& data){
        data.Acc_Buffer = matrix[0] * data.Acc[0] + matrix[1] * data.Acc[1] + matrix[2] * data.Acc[2];
        data.Gyro_Buffer = matrix[0] * data.Gyro[0] + matrix[1] * data.Gyro[1] + matrix[2] * data.Gyro[2];
        data.Mag_Buffer = matrix[0] * data.Mag[0] + matrix[1] * data.Mag[1] + matrix[2] * data.Mag[2];

        data.Acc = data.Acc_Buffer;
        data.Gyro = data.Gyro_Buffer;
        data.Mag = data.Mag_Buffer;
    }

    // Умножение матриц с сохренением информации в matrix_Buffer первой матрицы
    void operator*(const Matrix& mult_matrix){
        matrix_Buffer[0] = matrix[0] * mult_matrix.matrix[0] + matrix[1] * mult_matrix.matrix[3] + matrix[2] * mult_matrix.matrix[6];
        matrix_Buffer[1] = matrix[0] * mult_matrix.matrix[1] + matrix[1] * mult_matrix.matrix[4] + matrix[2] * mult_matrix.matrix[7];
        matrix_Buffer[2] = matrix[0] * mult_matrix.matrix[2] + matrix[1] * mult_matrix.matrix[5] + matrix[2] * mult_matrix.matrix[8];
        
        matrix_Buffer[3] = matrix[3] * mult_matrix.matrix[0] + matrix[4] * mult_matrix.matrix[3] + matrix[5] * mult_matrix.matrix[6];
        matrix_Buffer[4] = matrix[3] * mult_matrix.matrix[1] + matrix[4] * mult_matrix.matrix[4] + matrix[5] * mult_matrix.matrix[7];
        matrix_Buffer[5] = matrix[3] * mult_matrix.matrix[2] + matrix[4] * mult_matrix.matrix[5] + matrix[5] * mult_matrix.matrix[8];

        matrix_Buffer[6] = matrix[6] * mult_matrix.matrix[0] + matrix[7] * mult_matrix.matrix[3] + matrix[8] * mult_matrix.matrix[6];
        matrix_Buffer[7] = matrix[6] * mult_matrix.matrix[1] + matrix[7] * mult_matrix.matrix[4] + matrix[8] * mult_matrix.matrix[7];
        matrix_Buffer[8] = matrix[6] * mult_matrix.matrix[2] + matrix[7] * mult_matrix.matrix[5] + matrix[8] * mult_matrix.matrix[8];
    }

    // Умножение матриц с изменением значений элементов первой матрицы
    void operator*=(const Matrix& mult_matrix){
        matrix_Buffer[0] = matrix[0] * mult_matrix.matrix[0] + matrix[1] * mult_matrix.matrix[3] + matrix[2] * mult_matrix.matrix[6];
        matrix_Buffer[1] = matrix[0] * mult_matrix.matrix[1] + matrix[1] * mult_matrix.matrix[4] + matrix[2] * mult_matrix.matrix[7];
        matrix_Buffer[2] = matrix[0] * mult_matrix.matrix[2] + matrix[1] * mult_matrix.matrix[5] + matrix[2] * mult_matrix.matrix[8];
        
        matrix_Buffer[3] = matrix[3] * mult_matrix.matrix[0] + matrix[4] * mult_matrix.matrix[3] + matrix[5] * mult_matrix.matrix[6];
        matrix_Buffer[4] = matrix[3] * mult_matrix.matrix[1] + matrix[4] * mult_matrix.matrix[4] + matrix[5] * mult_matrix.matrix[7];
        matrix_Buffer[5] = matrix[3] * mult_matrix.matrix[2] + matrix[4] * mult_matrix.matrix[5] + matrix[5] * mult_matrix.matrix[8];

        matrix_Buffer[6] = matrix[6] * mult_matrix.matrix[0] + matrix[7] * mult_matrix.matrix[3] + matrix[8] * mult_matrix.matrix[6];
        matrix_Buffer[7] = matrix[6] * mult_matrix.matrix[1] + matrix[7] * mult_matrix.matrix[4] + matrix[8] * mult_matrix.matrix[7];
        matrix_Buffer[8] = matrix[6] * mult_matrix.matrix[2] + matrix[7] * mult_matrix.matrix[5] + matrix[8] * mult_matrix.matrix[8];

        copying_from_Buffer();
    }

    void operator=(const Matrix& _matrix){
        for (matrix_Index = 0; matrix_Index < 9; matrix_Index++){ matrix[matrix_Index] = _matrix[matrix_Index]; }
    }

    void copying_from_Buffer(){
        for (matrix_Index = 0; matrix_Index < 9; matrix_Index++){ matrix[matrix_Index] = matrix_Buffer[matrix_Index]; }
    }

    // ########################################################################
    // Математические операции

    // Вычисление определителя
    float Determinant()
    {
        return matrix[0] * (matrix[4] * matrix[8] - matrix[7] * matrix[5]) - matrix[1] * (matrix[3] * matrix[8] - matrix[6] * matrix[5]) + matrix[2] * (matrix[3] * matrix[7] - matrix[6] * matrix[4]);
    }

    // Нахождение обратной матрицы и последующим сохранением в matrix_Buffer
    void Mreverse(){
        float det = Determinant();
        
        // Заполним matrix_Buffer алгебраическими дополнениями матрицы matrix и транспонируем её
        matrix_Buffer[0] =  (matrix[4] * matrix[8] - matrix[7] * matrix[5]) / det;    //matrix_Buffer[0][0]
        matrix_Buffer[3] = -(matrix[3] * matrix[8] - matrix[6] * matrix[5]) / det;    //matrix_Buffer[1][0]
        matrix_Buffer[6] =  (matrix[3] * matrix[7] - matrix[6] * matrix[4]) / det;    //matrix_Buffer[2][0]

        matrix_Buffer[1] = -(matrix[1] * matrix[8] - matrix[7] * matrix[2]) / det;    //matrix_Buffer[0][1]
        matrix_Buffer[4] =  (matrix[0] * matrix[8] - matrix[6] * matrix[2]) / det;    //matrix_Buffer[1][1]
        matrix_Buffer[7] = -(matrix[0] * matrix[7] - matrix[6] * matrix[1]) / det;    //matrix_Buffer[2][1]

        matrix_Buffer[2] =  (matrix[1] * matrix[5] - matrix[4] * matrix[2]) / det;    //matrix_Buffer[0][2]
        matrix_Buffer[5] = -(matrix[0] * matrix[5] - matrix[3] * matrix[2]) / det;    //matrix_Buffer[1][2]
        matrix_Buffer[8] =  (matrix[0] * matrix[4] - matrix[3] * matrix[1]) / det;    //matrix_Buffer[2][2]
    }

    // ########################################################################
    // Нахождение матрицы перехода от СК датчиков к СК, связанную с Землей, у которой OY направлена на север, а OZ вертикально
    void set_rotationMatrix(const Data& zero_Data)
    {
        // Вывод коэффицентов матрицы поворота, а также описание используемых систем координат можно посмотреть в файле Documentation/Rotation_matrix.pdf
        float temp_matrix[9] = {0.0f};  // матрица перехода от системы координат x'y'z' к xyz
        
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

        temp_matrix[0] = (H_Y * (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) - W_Y * zero_Data.Mag.X_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[0][0]
        temp_matrix[1] = (H_Y * (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) - W_Y * zero_Data.Mag.Y_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[0][1]
        temp_matrix[2] = (H_Y * (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) - W_Y * zero_Data.Mag.Z_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[0][2]

        temp_matrix[3] = (H_X * (zero_Data.Gyro.X_coord - W_Z * zero_Data.Acc.X_coord / G) - W_X * zero_Data.Mag.X_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[1][0]
        matrix[4] = (H_X * (zero_Data.Gyro.Y_coord - W_Z * zero_Data.Acc.Y_coord / G) - W_X * zero_Data.Mag.Y_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[1][1]
        matrix[5] = (H_X * (zero_Data.Gyro.Z_coord - W_Z * zero_Data.Acc.Z_coord / G) - W_X * zero_Data.Mag.Z_coord) / (W_X * H_Y - W_Y * H_X);     // matrix[1][2]

        temp_matrix[6] = zero_Data.Gyro.X_coord / G;     // temp_matrix[2][0]
        temp_matrix[7] = zero_Data.Gyro.Y_coord / G;     // temp_matrix[2][1]
        temp_matrix[8] = zero_Data.Gyro.Z_coord / G;     // temp_matrix[2][2]
        
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
}