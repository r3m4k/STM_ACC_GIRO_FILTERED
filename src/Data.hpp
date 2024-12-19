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
        typedef struct 
        {
            uint8_t Header[4] = {126, 17, 255, 201};      // Начало пакета передачи данных {0x7E, 0x11, 0xFF, 0xC9}
            uint8_t Acc_X[2];
            uint8_t Acc_Y[2];
            uint8_t Acc_Z[2];
            uint8_t Gyro_X[2];
            uint8_t Gyro_Y[2];            
            uint8_t Gyro_Z[2];
            uint8_t contol_sum;
        }outbuf;

        outbuf Out_Buf;
        int16_t tmp;
        uint8_t i;
        
/* 
    //     typedef struct 
    //     {
    //         char Header[4];      // Начало пакета передачи данных
    //         unsigned char Acc_X_lowBit;
    //         unsigned char Acc_X_highBit;

    //         unsigned char Acc_Y_lowBit;
    //         unsigned char Acc_Y_highBit;

    //         unsigned char Acc_Z_lowBit;
    //         unsigned char Acc_Z_highBit;

    //         unsigned char Gyro_X_lowBit;
    //         unsigned char Gyro_X_highBit;

    //         unsigned char Gyro_Y_lowBit;
    //         unsigned char Gyro_Y_highBit;
            
    //         unsigned char Gyro_Z_lowBit;
    //         unsigned char Gyro_Z_highBit;

    //         unsigned char contol_sum;
    //     }outbuf;
        // ((unsigned char*)&Out_Buf)[0] = 0x7E; 
        // ((unsigned char*)&Out_Buf)[1] = 0x11;
        // ((unsigned char*)&Out_Buf)[2] = 0xFF;
        // ((unsigned char*)&Out_Buf)[3] = 0xC9;
        Out_Buf.Header[0] = 126;
        Out_Buf.Header[1] = 17;
        Out_Buf.Header[2] = 255;
        Out_Buf.Header[3] = 201;

        // Запишем в буфер данные ускорения
        for (int i = 0; i < 2; i++){
            // ((unsigned char*)&Out_Buf)[1 + 2 * i] = Out_Buf.Acc_X_lowBit  (т.е младший разряд Acc.X_coord)
            // ((unsigned char*)&Out_Buf)[2 + 2 * i] = Out_Buf.Acc_X_highBit (т.е старший разряд Acc.X_coord)
            tmp = round(Acc[i]);
            ((unsigned char*)&Out_Buf)[1 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[2 + 2 * i] = tmp >> 8;        // Старший разряд
        }

        // Запишем в буфер данные угловой скорости
        for (int i = 0; i < 2; i++){
            tmp = round(Gyro[i]);
            ((unsigned char*)&Out_Buf)[1 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[1 + 2 * i + 1] = tmp >> 8;        // Старший разряд
        }
*/        

        for (i = 0; i < 2; i++){
            // ((unsigned char*)&Out_Buf)[1 + 2 * i] = Out_Buf.Acc_XYZ_lowBit  (т.е младший разряд Acc.XYZ_coord)
            // ((unsigned char*)&Out_Buf)[2 + 2 * i] = Out_Buf.Acc_XYZ_highBit (т.е старший разряд Acc.XYZ_coord)
            tmp = round(Acc[i]);
            ((unsigned char*)&Out_Buf)[1 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[2 + 2 * i] = tmp >> 8;            // Старший разряд
        }

        for (i = 0; i < 2; i++){
            // ((unsigned char*)&Out_Buf)[1 + 2 * i] = Out_Buf.Gyro_XYZ_lowBit  (т.е младший разряд Gyro.XYZ_coord)
            // ((unsigned char*)&Out_Buf)[2 + 2 * i] = Out_Buf.Gyro_XYZ_highBit (т.е старший разряд Gyro.XYZ_coord)
            tmp = round(Gyro[i]);
            ((unsigned char*)&Out_Buf)[1 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[2 + 2 * i] = tmp >> 8;            // Старший разряд
        }

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf); i++){
            tmp += ((unsigned char*)&Out_Buf)[i];       
        }
        Out_Buf.contol_sum = tmp;

        CDC_Send_DATA((unsigned char*)&Out_Buf, sizeof(Out_Buf));
    }
};
