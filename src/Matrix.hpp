#include "Data.hpp"

class Matrix{
public:
    // float matrix[9] = {0.0f};
    float matrix[9] = {1, 2, 3, 4, 5, 6, 7, 8};
    float matrix_Buffer[9] = {0.0f};
    short matrix_Index = 0;

    // ########################################################################
    // Перегрузка операторов

    // Умножение frame на матрицу, как умножение вектора на матрицу, с сохранением результата в frame.frame_buffer
    void operator*(Frame& frame){
        frame.frame_Buffer[0] = matrix[0] * frame[0] + matrix[1] * frame[1] + matrix[2] * frame[2];
        frame.frame_Buffer[1] = matrix[3] * frame[0] + matrix[4] * frame[1] + matrix[5] * frame[2];
        frame.frame_Buffer[2] = matrix[6] * frame[0] + matrix[7] * frame[1] + matrix[8] * frame[2];
    }

    // Умножение data.Acc, data.Gyro, data.Mag на матрицу, поэлементно, как умножение векторов на матрицу, с сохранением 
    // результата в data.Acc_Buffer, data.Gyro_Buffer, data.Mag_Buffer
    void operator*(Data& data){
        *this * data.Acc;
        data.Acc_Buffer = data.Acc.frame_Buffer;

        *this * data.Gyro;
        data.Gyro_Buffer = data.Gyro.frame_Buffer;
    }

    // Умножение data.Acc, data.Gyro, data.Mag на матрицу с изменением значений data.Acc, data.Gyro, data.Mag
    void operator*=(Data& data){
        *this * data;

        data.Acc = data.Acc_Buffer;
        data.Gyro = data.Gyro_Buffer;
    }

    // Умножение матриц с сохренением информации в matrix_Buffer первой матрицы
    void operator*(Matrix& mult_matrix){
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
    void operator*=(Matrix& mult_matrix){
        *this * mult_matrix;
        copying_from_Buffer();
    }

    // Присваивание значений другой матрицы
    void operator=(Matrix& _matrix){
        for (matrix_Index = 0; matrix_Index < 9; matrix_Index++){ matrix[matrix_Index] = _matrix[matrix_Index]; }
    }

    // Итераторы
    float& operator[](int index){ return matrix[index]; }    
    
    float& operator()(int index_1, int index_2){ return matrix[3 * index_1 + index_2]; }

    // ########################################################################
    // Копирование значений из matrix_Buffer
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
    friend void UsartSend(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t maxValue1, uint16_t maxValue2, uint16_t maxValue3, uint16_t DPPValue1, uint16_t DPPValue2, uint16_t DPPValue3, uint16_t DPPValue4);

    void sending() {
        UsartSend(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7], matrix[8], 0);
    }
};