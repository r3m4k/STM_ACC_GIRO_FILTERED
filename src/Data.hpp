#include "Frame.hpp"
#include "VCP_F3.h"


class Data
{
public:
    Frame Acc;  // Класс для хранения данных с акселерометра
    Frame Gyro; // Класс для хранения данных с гироскопа

    // Буферы, в которые будет сохраняться временная информация
    Frame Acc_Buffer, Gyro_Buffer;

    // ########################################################################
    // Перегрузка операторов

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer первого слагаемого
    void operator+(Data &data)
    {
        Acc + data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag первого слагаемого
    void operator+=(Data &data)
    {
        Acc + data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro = Gyro.frame_Buffer;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer уменьшаемого
    void operator-(Data &data)
    {
        Acc - data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag уменьшаемого
    void operator-=(Data &data)
    {
        Acc - data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro = Gyro.frame_Buffer;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer делимого
    void operator/(float num)
    {
        Acc / num;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro / num;
        Gyro_Buffer = Gyro.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag делимого
    void operator/=(float num)
    {
        Acc / num;
        Acc = Acc.frame_Buffer;

        Gyro / num;
        Gyro = Gyro.frame_Buffer;
    }

    void operator=(Data &data)
    {
        Acc = data.Acc;
        Gyro = data.Gyro;
    }

    Frame &operator[](int index)
    {
        if (index == 0)
            return Acc;
        else if (index == 1)
            return Gyro;
    }

    // ########################################################################
    // Чтение данных с датчиков
    void Read_Data()
    {
        Acc.Read_Acc();
        Gyro.Read_Gyro();
    }

    // ########################################################################
    // Функционал для Data

    // Установка нулевых значений
    void set_zero_Values()
    {
        Acc.set_zero_Frame();
        Gyro.set_zero_Frame();
    }

    friend void UsartSend(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t maxValue1, uint16_t maxValue2, uint16_t maxValue3, uint16_t DPPValue1, uint16_t DPPValue2, uint16_t DPPValue3, uint16_t DPPValue4);
    // Отправка элементов Data по COM порту
    void sending_Usart()
    {
        UsartSend(Acc.X_coord, Acc.Y_coord, Acc.Z_coord, Gyro.X_coord, Gyro.Y_coord, Gyro.Z_coord, 0, 0, 0, 0);
    }

    void sending_USB()
    {
        uint8_t bufoutP[21] = {0};
        bufoutP[0] = 126;
        bufoutP[1] = 17;
        bufoutP[2] = 255;
        bufoutP[3] = 201; // 201 (C9), standard - 200 (C8)

        bufoutP[4] = (uint8_t)Acc.X_coord;
        bufoutP[5] = ((uint8_t)((uint16_t)(((int16_t)Acc.X_coord) >> 8)));
        bufoutP[6] = (uint8_t)Acc.Y_coord;
        bufoutP[7] = ((uint8_t)((uint16_t)(((int16_t)Acc.Y_coord) >> 8)));
        bufoutP[8] = (uint8_t)Acc.Z_coord;
        bufoutP[9] = ((uint8_t)((uint16_t)(((int16_t)Acc.Z_coord) >> 8)));

        bufoutP[10] = (uint8_t)Gyro.X_coord;
        bufoutP[11] = ((uint8_t)((uint16_t)(((int16_t)Gyro.X_coord) >> 8)));
        bufoutP[12] = (uint8_t)Gyro.Y_coord;
        bufoutP[13] = ((uint8_t)((uint16_t)(((int16_t)Gyro.Y_coord) >> 8)));
        bufoutP[14] = (uint8_t)Gyro.Z_coord;
        bufoutP[15] = ((uint8_t)((uint16_t)(((int16_t)Gyro.Z_coord) >> 8)));

        bufoutP[16] = 0;
        bufoutP[17] = 0;
        bufoutP[18] = 0;
        bufoutP[19] = 0;

        bufoutP[20] = (bufoutP[0] + bufoutP[1] + bufoutP[2] + bufoutP[3] + bufoutP[4] + bufoutP[5] + bufoutP[6] + bufoutP[7] + bufoutP[8] + bufoutP[9] + bufoutP[10] + bufoutP[11] + bufoutP[12] + bufoutP[13] + bufoutP[14] + bufoutP[15] + bufoutP[16] + bufoutP[17] + bufoutP[18] + bufoutP[19]);
        // sprintf(bufoutP, "\n\r");
        CDC_Send_DATA(bufoutP, 21);
    }
};
