#ifndef __DATA_HPP
#define __DATA_HPP

#include "Frame.hpp"
#include "VCP_F3.h"

#define MAX_TEMP_DIFF       500         // Максимальное изменение температуры между снятиями показаний 
                                        // (5 сигм для MAX_TEMP_DIFF = 1000 при тепловом равновесии датчика и окружающей среды)
#define MAX_TEMP_COUNTER    16          // Раз в какое количество шагов будет считываться температура. Лучше использовать FilterFrameSize


class Data
{
public:
    Frame Acc;      // Класс для хранения данных с акселерометра
    Frame Gyro;     // Класс для хранения данных с гироскопа
    
    // Буферы, в которые будет сохраняться временная информация
    Frame Acc_Buffer, Gyro_Buffer;

    float Temp, Temp_buffer, Temp_Previous;        // Значения температуры
    uint8_t Temp_counter = 0;
    friend void ReadMagTemp(float *pfTData);

    float Acc_coeff[3] = {0.0f};         // Коэффициенты пропорциональности данных ускорений от изменения температуры
    float Gyro_coeff[3] = {0.0f};        // Коэффициенты пропорциональности данных угловых скоростей от изменения температуры

    uint8_t i;  

    // ########################################################################
    // Перегрузка операторов

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer первого слагаемого
    void operator+(Data &data)
    {
        Acc + data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;

        Temp_buffer += data.Temp;

    }

    // Изменение значений Acc, Gyro, Mag первого слагаемого
    void operator+=(Data &data)
    {
        Acc + data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro = Gyro.frame_Buffer;

        Temp += data.Temp;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer уменьшаемого
    void operator-(Data &data)
    {
        Acc - data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;

        Temp_buffer -= data.Temp;
    }

    // Изменение значений Acc, Gyro, Mag уменьшаемого
    void operator-=(Data &data)
    {
        Acc - data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro = Gyro.frame_Buffer;

        Temp -= data.Temp;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer делимого
    void operator/(float num)
    {
        Acc / num;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro / num;
        Gyro_Buffer = Gyro.frame_Buffer;

        Temp_buffer /= num;
    }

    // Изменение значений Acc, Gyro, Mag делимого
    void operator/=(float num)
    {
        Acc / num;
        Acc = Acc.frame_Buffer;

        Gyro / num;
        Gyro = Gyro.frame_Buffer;

        Temp /= num;
    }

    void operator=(Data &data)
    {
        Acc = data.Acc;
        Gyro = data.Gyro;
        Temp = data.Temp;
    }

    Frame& operator[](int index)
    {
        if (index == 0)
            return Acc;
        else if (index == 1)
            return Gyro;
    }

    float operator()(int index1, int index2){
        if      (index1 == 0){
            if      (index2 == 0) return Acc.X_coord;
            else if (index2 == 1) return Acc.Y_coord;
            else if (index2 == 2) return Acc.Z_coord;
        }
        else if (index1 == 1){
            if      (index2 == 0) return Gyro.X_coord;
            else if (index2 == 1) return Gyro.Y_coord;
            else if (index2 == 2) return Gyro.Z_coord;
        }
    }

    // ########################################################################
    // Чтение данных с датчиков
    void Read_Data()
    {
        Acc.Read_Acc();
        Gyro.Read_Gyro();

        Read_Temp();
        Temp_counter++;
}

void Read_Temp(){
        if (Temp_counter == 255){
            Temp_counter -= 33;
            while (Temp_counter++ < 255){
                ReadMagTemp(&Temp);
                Temp_Previous += Temp;
                for (int j = 0; j < 100; j++){   continue;   }
            }
            Temp_Previous /= 32;
            Temp_counter = 0;
        }

        if (!(Temp_counter % MAX_TEMP_COUNTER)){
            ReadMagTemp(&Temp);
            if (abs(Temp - Temp_Previous) > MAX_TEMP_DIFF){
                // Если разница между прошлой температурой и текущей больше MAX_TEMP_DIFF единиц,
                // то считаем, что полученное значение температуры неверное и заполним его предыдущим значением
                Temp = Temp_Previous;
            }
            Temp_Previous = Temp;
            Temp_counter = 0;
        }
    }

    // ########################################################################
    // Функционал для Data

    // Установка нулевых значений
    void set_zero_Values()
    {
        Acc.set_zero_Frame();
        Gyro.set_zero_Frame();

        Temp = 0.0f;
    }

    void update_zero_level(float temp_diff){
        for (i = 0; i < 3; i++){
            Acc[i] -= Acc_coeff[i] * temp_diff;
            Gyro[i] -= Gyro_coeff[i] * temp_diff;
        }
    }

    // friend void UsartSend(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t maxValue1, uint16_t maxValue2, uint16_t maxValue3, uint16_t DPPValue1, uint16_t DPPValue2, uint16_t DPPValue3, uint16_t DPPValue4);
    // // Отправка элементов Data по COM порту
    // void sending_Usart()
    // {
    //     UsartSend(Acc.X_coord, Acc.Y_coord, Acc.Z_coord, Gyro.X_coord, Gyro.Y_coord, Gyro.Z_coord, 0, 0, 0, 0);
    // }
    
};

#endif /*    __DATA_HPP    */