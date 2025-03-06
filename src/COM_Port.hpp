#ifndef __COM_PORT_HPP
#define __COM_PORT_HPP

#include "hw_config.h"
#include "VCP_F3.h"
#include "Data.hpp"
#include "Frame.hpp"

class COM_Port{

    int16_t tmp;
    uint8_t i;  

    // #############################################
    typedef struct Frame_Struct
    {
        uint8_t X_coord[2];
        uint8_t Y_coord[2];
        uint8_t Z_coord[2];
    }Frame_Struct;
    
    typedef struct Data_Struct{
        Frame_Struct Acc;
        Frame_Struct Gyro;
    }Data_Struct;
    
    typedef struct DataBuffer_Struct
    {
        Frame_Struct Acc;
        Frame_Struct Acc_Buffer;
        Frame_Struct Gyro;
        Frame_Struct Gyro_Buffer;
    }DataBuffer_Struct;
      
    //#############################################

    typedef struct outbuf_type1
    {         
        uint8_t Header[4] = {0x7E, 0x11, 0xFF, 0xC8};      // {126, 17, 255, 200}

        uint8_t Time[2];
        Data_Struct data_struct;
        uint8_t Temp[2];

        uint8_t checksum = 0;
    }outbuf_type1;

    //#############################################

    typedef struct outbuf_type2
    {         
        uint8_t Header[4] = {0x7E, 0x11, 0xFF, 0xC9};      // {126, 17, 255, 201}

        uint8_t Time[2];
        DataBuffer_Struct data_Buffer_struct;
        uint8_t Temp[2];

        uint8_t checksum = 0;
    }outbuf_type2;

    //#############################################
    
    typedef struct outbuf_type3
    {         
        uint8_t Header[4] = {0x7E, 0x11, 0xFF, 0xCA};      // {126, 17, 255, 202}

        uint8_t Time[2];
        Data_Struct data1_struct;
        Data_Struct data2_struct;
        uint8_t Temp[2];

        uint8_t checksum = 0;
    }outbuf_type3;
    
    //#############################################
    
    typedef struct outbuf_type4
    {         
        uint8_t Header[4] = {0x7E, 0x11, 0xFF, 0xCB};      // {126, 17, 255, 203}

        uint8_t Time[2];
        DataBuffer_Struct data1_Buffer_struct;
        DataBuffer_Struct data2_Buffer_struct;
        uint8_t Temp[2];

        uint8_t checksum = 0;
    }outbuf_type4;

    //#############################################
    
    typedef struct outbuf_type5
    {         
        uint8_t Header[4] = {0x7E, 0x11, 0xFF, 0xCC};      // {126, 17, 255, 204}

        uint8_t Time[2];

        Frame_Struct coordinates;
        Frame_Struct velocity;
        Data_Struct data_struct;

        uint8_t Temp[2];

        uint8_t checksum = 0;
    }outbuf_type5;

    //#############################################
    
public:
    // Отправка посылки с одним пакетом измерений data
    void sending_data(uint32_t Ticks, Data &data){

        outbuf_type1 Out_Buf;

        tmp = (uint16_t)Ticks;
        ((uint8_t*)&Out_Buf)[4] = tmp;                 // Младший разряд счётчика таймера
        ((uint8_t*)&Out_Buf)[5] = tmp >> 8;            // Старший разряд счётчика таймера

        filling_DataStruct(data, Out_Buf.data_struct);

        // ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 3] = data.Temp_lowBit  (т.е младший разряд Temp)
        // ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 2] = data.Temp_highBit (т.е старший разряд Temp)
        tmp = round(data.Temp * 100);
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 3] = tmp;                 // Младший разряд
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 2] = tmp >> 8;            // Старший разряд

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf) - 1; i++){
            tmp += ((uint8_t*)&Out_Buf)[i];       
        }
        Out_Buf.checksum = tmp;

        CDC_Send_DATA((uint8_t*)&Out_Buf, sizeof(Out_Buf));
    }

    // Отправка посылки с одним пакетом измерений data и его буфером
    void sending_data(uint32_t Ticks, Data &data, bool Buffer_Flag){
       
        tmp = Buffer_Flag;    // Чтобы избежать warning: unused parameter 'Buffer_Flag'

        outbuf_type2 Out_Buf;

        tmp = (uint16_t)Ticks;
        ((uint8_t*)&Out_Buf)[4] = tmp;                 // Младший разряд счётчика таймера
        ((uint8_t*)&Out_Buf)[5] = tmp >> 8;            // Старший разряд счётчика таймера

        filling_DataBuffer_Struct(data, Out_Buf.data_Buffer_struct);

        tmp = round(data.Temp * 100);
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 3] = tmp;                 // Младший разряд
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 2] = tmp >> 8;            // Старший разряд

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf) - 1; i++){
            tmp += ((uint8_t*)&Out_Buf)[i];       
        }
        Out_Buf.checksum = tmp;

        CDC_Send_DATA((uint8_t*)&Out_Buf, sizeof(Out_Buf));        
    }

    // Отправка посылки с двумя пакетами измерений data1, data2
    void sending_data(uint32_t Ticks, Data &data1, Data &data2){

        outbuf_type3 Out_Buf;

        tmp = (uint16_t)Ticks;
        ((uint8_t*)&Out_Buf)[4] = tmp;                 // Младший разряд счётчика таймера
        ((uint8_t*)&Out_Buf)[5] = tmp >> 8;            // Старший разряд счётчика таймера

        filling_DataStruct(data1, Out_Buf.data1_struct);
        filling_DataStruct(data2, Out_Buf.data2_struct);

        tmp = round(data1.Temp * 100);
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 3] = tmp;                 // Младший разряд
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 2] = tmp >> 8;            // Старший разряд

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf) - 1; i++){
            tmp += ((uint8_t*)&Out_Buf)[i];       
        }
        Out_Buf.checksum = tmp;

        CDC_Send_DATA((uint8_t*)&Out_Buf, sizeof(Out_Buf));        
    }

    // Отправка посылки с двумя пакетами измерений data1, data2
    // ВАЖНО! При вызове именно этой функции необходимо последним аргументом передать любое булевое значение
    // тк иначе будет выполнен другой вариант этой переполненной функции. 
    void sending_data(uint32_t Ticks, Data &data1, Data &data2, bool Buffer_Flag){

        tmp = Buffer_Flag;    // Чтобы избежать warning: unused parameter 'Buffer_Flag'

        outbuf_type4 Out_Buf;

        tmp = (uint16_t)Ticks;
        ((uint8_t*)&Out_Buf)[4] = tmp;                 // Младший разряд счётчика таймера
        ((uint8_t*)&Out_Buf)[5] = tmp >> 8;            // Старший разряд счётчика таймера

        filling_DataBuffer_Struct(data1, Out_Buf.data1_Buffer_struct);
        filling_DataBuffer_Struct(data2, Out_Buf.data2_Buffer_struct);

        tmp = round(data1.Temp * 100);
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 3] = tmp;                 // Младший разряд
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 2] = tmp >> 8;            // Старший разряд

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf) - 1; i++){
            tmp += ((uint8_t*)&Out_Buf)[i];       
        }
        Out_Buf.checksum = tmp;

        CDC_Send_DATA((uint8_t*)&Out_Buf, sizeof(Out_Buf));        
    }

    // Отправка посылки с данными координат, скоростей и одним пакетом измерений data
    void sending_data(uint32_t Ticks, float *coordinates, float *velocity, Data &data){

        outbuf_type5 Out_Buf;

        tmp = (uint16_t)Ticks;
        ((uint8_t*)&Out_Buf)[4] = tmp;                 // Младший разряд счётчика таймера
        ((uint8_t*)&Out_Buf)[5] = tmp >> 8;            // Старший разряд счётчика таймера

        filling_FrameStruct(coordinates, Out_Buf.coordinates);
        filling_FrameStruct(velocity, Out_Buf.velocity);
        filling_DataStruct(data, Out_Buf.data_struct);

        tmp = round(data.Temp * 100);
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 3] = tmp;                 // Младший разряд
        ((uint8_t*)&Out_Buf)[sizeof(Out_Buf) - 2] = tmp >> 8;            // Старший разряд

        // Посчитаем контрольную сумму
        tmp = 0;
        for (i = 0; i < sizeof(Out_Buf) - 1; i++){
            tmp += ((uint8_t*)&Out_Buf)[i];       
        }
        Out_Buf.checksum = tmp;

        CDC_Send_DATA((uint8_t*)&Out_Buf, sizeof(Out_Buf));
    }

    //#############################################
    // Заполнение структуры frame_struct данными из frame
    void filling_FrameStruct(Frame &frame, Frame_Struct &frame_struct){
        for (i = 0; i < 3; i++){
            // ((uint8_t*)&frame_struct)[0 + 2 * i] = frame_struct.XYZ_coord_lowBit  (т.е младший разряд Acc.XYZ_coord)
            // ((uint8_t*)&frame_struct)[1 + 2 * i] = frame_struct.XYZ_coord_highBit (т.е старший разряд Acc.XYZ_coord)
            tmp = round(frame[i] * 1000);
            ((uint8_t*)&frame_struct)[0 + 2 * i] = tmp;                 // Младший разряд
            ((uint8_t*)&frame_struct)[1 + 2 * i] = tmp >> 8;            // Старший разряд
        }
    }

    // Заполнение структуры frame_struct данными из array
    void filling_FrameStruct(float *array, Frame_Struct &frame_struct){
        for (i = 0; i < 3; i++){
            // ((uint8_t*)&frame_struct)[0 + 2 * i] = frame_struct.XYZ_coord_lowBit  (т.е младший разряд Acc.XYZ_coord)
            // ((uint8_t*)&frame_struct)[1 + 2 * i] = frame_struct.XYZ_coord_highBit (т.е старший разряд Acc.XYZ_coord)
            tmp = round(array[i] * 1000);
            ((uint8_t*)&frame_struct)[0 + 2 * i] = tmp;                 // Младший разряд
            ((uint8_t*)&frame_struct)[1 + 2 * i] = tmp >> 8;            // Старший разряд
        }
    }
    
    // Заполнение структуры data_struct данными из data
    void filling_DataStruct(Data &data, Data_Struct &data_struct){
        filling_FrameStruct(data.Acc, data_struct.Acc);
        filling_FrameStruct(data.Gyro, data_struct.Gyro);
    }

    // Заполнение структуры data_Buffer_struct данными из data
    void filling_DataBuffer_Struct(Data &data, DataBuffer_Struct &data_Buffer_struct){
        filling_FrameStruct(data.Acc, data_Buffer_struct.Acc);
        filling_FrameStruct(data.Acc_Buffer, data_Buffer_struct.Acc_Buffer);

        filling_FrameStruct(data.Gyro, data_Buffer_struct.Gyro);
        filling_FrameStruct(data.Gyro_Buffer, data_Buffer_struct.Gyro_Buffer);
    }
    
};

#endif /*    __COM_PORT_HPP    */