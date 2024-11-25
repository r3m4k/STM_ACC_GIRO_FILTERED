#include "Frame.hpp"


class Data
{
public:
    Frame Acc;
    Frame Gyro;
    Frame Mag;
  
    // Буферы, в которые будет сохраняться временная информация
    Frame Acc_Buffer, Gyro_Buffer, Mag_Buffer; 

    // ########################################################################
    // Перегрузка операторов

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer первого слагаемого
    void operator+(Data& data){
        Acc + data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;

        Mag + data.Mag;
        Mag_Buffer = Mag.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag первого слагаемого
    void operator+=(Data& data){
        Acc + data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro = Gyro.frame_Buffer;

        Mag + data.Mag;
        Mag = Mag.frame_Buffer;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer уменьшаемого
    void operator-(Data& data){
        Acc - data.Acc;
        Acc_Buffer = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro_Buffer = Gyro.frame_Buffer;

        Mag - data.Mag;
        Mag_Buffer = Mag.frame_Buffer;
    }

    // Изменение значений Acc, Gyro, Mag уменьшаемого
    void operator-=(Data& data){
        Acc - data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro = Gyro.frame_Buffer;

        Mag - data.Mag;
        Mag = Mag.frame_Buffer;
    }

    // Сохранение результата в Acc_Buffer, Gyro_Buffer, Mag_Buffer делимого
    void operator/(float num){
        Acc / num;
        Acc_Buffer = Acc.frame_Buffer;
        
        Gyro / num;
        Gyro_Buffer = Gyro.frame_Buffer;
        
        Mag / num;
        Mag_Buffer = Mag.frame_Buffer;
    }   

    // Изменение значений Acc, Gyro, Mag делимого
    void operator/=(float num){
        Acc / num;
        Acc = Acc.frame_Buffer;
        
        Gyro / num;
        Gyro = Gyro.frame_Buffer;
        
        Mag / num;
        Mag = Mag.frame_Buffer;
    }  

    void operator=(Data& data){
        Acc  = data.Acc;
        Gyro = data.Gyro;
        Mag  = data.Mag;
    } 

    Frame& operator[](int index){
        if      (index == 0) return Acc;
        else if (index == 1) return Gyro;
        else if (index == 2) return Mag;
    } 

    // ########################################################################
    // Чтение данных с датчиков
    void Read_Data()
    {
        Acc.Read_Acc();
        Gyro.Read_Gyro();
        Mag.Read_Mag();
    }

    // ########################################################################
    // Функционал для Data
    void set_zero_Values()
    {
        Acc.set_zero_Frame();
        Gyro.set_zero_Frame();
        Mag.set_zero_Frame();
    }
};
