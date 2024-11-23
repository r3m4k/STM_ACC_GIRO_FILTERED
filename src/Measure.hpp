#include "Data.hpp"
#include "Matrix.hpp"


class Measure : public Data, public Matrix
{
public:
    // Данные, которые будут меняться в вызываемых функциях
    // Вынесим их в статические переменные для избежания переполнения стека процессора
    Data current_Data, zero_Data, buffer_Data;

    Matrix rotation_matrix;         // Матрица перехода от СК платы к глобальной СК, у которой ось OY направлена на север, а OZ перпендикулярно поверхности
    float longitude;                // Широта места, где будет находиться плата
    float sqrt2 = 1.41421356237;    // Тк sqrt(2) будет использоваться довольно часто, то вынесим его значение в отдельную переменную
    short i, j;

    Measure(float phi) { longitude = phi; }

    // ########################################################################
    // Начальная выставка датчиков
    void initial_setting(){
        set_zero_Data();
        rotation_matrix.set_rotationMatrix(zero_Data);
    }

    // Нахождение нулевых значений
    void set_zero_Data()
    {
        char degree = 16;
        char jump_mean_degree = 6; // Степень глубины прыгающего среднего

        int max = my_pow(2, degree - jump_mean_degree);
        int jm_max = my_pow(2, jump_mean_degree);
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
};