#include "Frame.hpp"
#include "VCP_F3.h"

#define MAG     // Использование магнитометра
#define TEMP    // Использование температурного датчика


class Data
{
public:
    Frame Acc;  // Класс для хранения данных с акселерометра
    Frame Gyro; // Класс для хранения данных с гироскопа
    
    // Буферы, в которые будет сохраняться временная информация
    Frame Acc_Buffer, Gyro_Buffer;

#ifdef MAG
    Frame Mag, Mag_Buffer;  // Класс для хранения данных с магнитометра
#endif

#ifdef TEMP
    float Temp, Temp_buffer; // Значение температуры
    friend void ReadMagTemp(float *pfTData);
#endif

    typedef struct 
    {
        uint8_t Header[4] = {126, 17, 255, 201};      // Начало пакета передачи данных {0x7E, 0x11, 0xFF, 0xC9}
        
        uint8_t Acc_X[2] = {0};
        uint8_t Acc_Y[2] = {0};
        uint8_t Acc_Z[2] = {0};
        
        uint8_t Gyro_X[2] = {0};
        uint8_t Gyro_Y[2] = {0};
        uint8_t Gyro_Z[2] = {0};

#ifdef MAG
        uint8_t Mag_X[2] = {0};
        uint8_t Mag_Y[2] = {0};
        uint8_t Mag_Z[2] = {0};
#endif

#ifdef TEMP
        uint8_t Temp[2] = {0};
#endif

        uint8_t checksum = 0;
    }outbuf;

    int16_t tmp;
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

#ifdef MAG
        Mag + data.Mag;
        Mag_Buffer = Mag.frame_Buffer;
#endif

#ifdef TEMP
        Temp_buffer += data.Temp;
#endif

    }

    // Изменение значений Acc, Gyro, Mag первого слагаемого
    void operator+=(Data &data)
    {
        Acc + data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro = Gyro.frame_Buffer;
#ifdef MAG
        Mag + data.Mag;
        Mag = Mag.frame_Buffer;
#endif

#ifdef TEMP
        Temp += data.Temp;
#endif
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer уменьшаемого
    void operator-(Data &data)
    {
        Acc - data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;
#ifdef MAG
        Mag + data.Mag;
        Mag_Buffer = Mag.frame_Buffer;
#endif

#ifdef TEMP
        Temp_buffer -= data.Temp;
#endif
    }

    // Изменение значений Acc, Gyro, Mag уменьшаемого
    void operator-=(Data &data)
    {
        Acc - data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro = Gyro.frame_Buffer;
#ifdef MAG
        Mag - data.Mag;
        Mag = Mag.frame_Buffer;
#endif

#ifdef TEMP
        Temp -= data.Temp;
#endif
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer делимого
    void operator/(float num)
    {
        Acc / num;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro / num;
        Gyro_Buffer = Gyro.frame_Buffer;
#ifdef MAG
        Mag / num;
        Mag_Buffer = Mag.frame_Buffer;
#endif

#ifdef TEMP
        Temp_buffer /= num;
#endif
    }

    // Изменение значений Acc, Gyro, Mag делимого
    void operator/=(float num)
    {
        Acc / num;
        Acc = Acc.frame_Buffer;

        Gyro / num;
        Gyro = Gyro.frame_Buffer;
#ifdef MAG
        Mag / num;
        Mag = Mag.frame_Buffer;
#endif

#ifdef TEMP
        Temp /= num;
#endif
    }

    void operator=(Data &data)
    {
        Acc = data.Acc;
        Gyro = data.Gyro;
#ifdef MAG
        Mag = data.Mag;
#endif

#ifdef TEMP
        Temp = data.Temp;
#endif
    }

    Frame& operator[](int index)
    {
        if (index == 0)
            return Acc;
        else if (index == 1)
            return Gyro;
#ifdef MAG
        else if (index == 2)
            return Mag;
#endif
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
#ifdef MAG
        else if (index1 == 2){
            if      (index2 == 0) return Mag.X_coord;
            else if (index2 == 1) return Mag.Y_coord;
            else if (index2 == 2) return Mag.Z_coord;
    #endif
        }
    }

    // ########################################################################
    // Чтение данных с датчиков
    void Read_Data()
    {
        Acc.Read_Acc();
        Gyro.Read_Gyro();
#ifdef MAG
        Mag.Read_Mag();
#endif

#ifdef TEMP
        ReadMagTemp(&Temp);
#endif
    }

    // ########################################################################
    // Функционал для Data

    // Установка нулевых значений
    void set_zero_Values()
    {
        Acc.set_zero_Frame();
        Gyro.set_zero_Frame();
#ifdef MAG
        Mag.set_zero_Frame();
#endif

#ifdef TEMP
        Temp = 0.0f;
#endif
    }

    // friend void UsartSend(uint16_t Value1, uint16_t Value2, uint16_t Value3, uint16_t maxValue1, uint16_t maxValue2, uint16_t maxValue3, uint16_t DPPValue1, uint16_t DPPValue2, uint16_t DPPValue3, uint16_t DPPValue4);
    // // Отправка элементов Data по COM порту
    // void sending_Usart()
    // {
    //     UsartSend(Acc.X_coord, Acc.Y_coord, Acc.Z_coord, Gyro.X_coord, Gyro.Y_coord, Gyro.Z_coord, 0, 0, 0, 0);
    // }
    
    void sending_USB()
    {     
        outbuf Out_Buf;
        
        for (i = 0; i < 3; i++){
            // ((unsigned char*)&Out_Buf)[4 + 2 * i] = Out_Buf.Acc_XYZ_lowBit  (т.е младший разряд Acc.XYZ_coord)
            // ((unsigned char*)&Out_Buf)[5 + 2 * i] = Out_Buf.Acc_XYZ_highBit (т.е старший разряд Acc.XYZ_coord)
            tmp = round(Acc[i]);
            ((unsigned char*)&Out_Buf)[4 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[5 + 2 * i] = tmp >> 8;            // Старший разряд
        }

        for (i = 0; i < 3; i++){
            // ((unsigned char*)&Out_Buf)[10 + 2 * i] = Out_Buf.Gyro_XYZ_lowBit  (т.е младший разряд Gyro.XYZ_coord)
            // ((unsigned char*)&Out_Buf)[11 + 2 * i] = Out_Buf.Gyro_XYZ_highBit (т.е старший разряд Gyro.XYZ_coord)
            tmp = round(Gyro[i]);
            ((unsigned char*)&Out_Buf)[10 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[11 + 2 * i] = tmp >> 8;            // Старший разряд
        }
        
#ifdef MAG        
        for (i = 0; i < 3; i++){
            // ((unsigned char*)&Out_Buf)[16 + 2 * i] = Out_Buf.Mag_XYZ_lowBit  (т.е младший разряд Mag.XYZ_coord)
            // ((unsigned char*)&Out_Buf)[17 + 2 * i] = Out_Buf.Mag_XYZ_highBit (т.е старший разряд Mag.XYZ_coord)
            tmp = round(Mag[i]);
            ((unsigned char*)&Out_Buf)[16 + 2 * i] = tmp;                 // Младший разряд
            ((unsigned char*)&Out_Buf)[17 + 2 * i] = tmp >> 8;            // Старший разряд
        }
#endif

#ifdef TEMP
        // ((unsigned char*)&Out_Buf)[16 + 2 * i] = Temp_lowBit  (т.е младший разряд Temp)
        // ((unsigned char*)&Out_Buf)[17 + 2 * i] = Temp_highBit (т.е старший разряд Temp)
        tmp = round(Temp);
        ((unsigned char*)&Out_Buf)[sizeof(Out_Buf) - 3] = tmp;                 // Младший разряд
        ((unsigned char*)&Out_Buf)[sizeof(Out_Buf) - 2] = tmp >> 8;            // Старший разряд
#endif

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf); i++){
            tmp += ((unsigned char*)&Out_Buf)[i];       
        }
        Out_Buf.checksum = tmp;

        CDC_Send_DATA((unsigned char*)&Out_Buf, sizeof(Out_Buf));

    }
};
