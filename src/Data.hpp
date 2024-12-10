#include "Frame.hpp"


class Data
{
public:
    Frame Acc;          // Класс для хранения данных с акселерометра
    Frame Gyro;         // Класс для хранения данных с гироскопа
  
    // Буферы, в которые будет сохраняться временная информация
    Frame Acc_Buffer, Gyro_Buffer;

    // ########################################################################
    // Перегрузка операторов

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer первого слагаемого
    void operator+(Data& data){
        Acc + data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag первого слагаемого
    void operator+=(Data& data){
        Acc + data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro = Gyro.frame_Buffer;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer уменьшаемого
    void operator-(Data& data){
        Acc - data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag уменьшаемого
    void operator-=(Data& data){
        Acc - data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro = Gyro.frame_Buffer;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer делимого
    void operator/(float num){
        Acc / num;
        Acc_Buffer = Acc.frame_Buffer;
        
        Gyro / num;
        Gyro_Buffer = Gyro.frame_Buffer;
    }   

    // Изменение значений Acc, Gyro, Mag делимого
    void operator/=(float num){
        Acc / num;
        Acc = Acc.frame_Buffer;
        
        Gyro / num;
        Gyro = Gyro.frame_Buffer;
    }  

    void operator=(Data& data){
        Acc  = data.Acc;
        Gyro = data.Gyro;
    } 

    Frame& operator[](int index){
        if      (index == 0) return Acc;
        else if (index == 1) return Gyro;
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
    void sending(){
        UsartSend(Acc.X_coord, Acc.Y_coord, Acc.Z_coord, Gyro.X_coord, Gyro.Y_coord, Gyro.Z_coord, 0, 0, 0, 0);
    }
};
