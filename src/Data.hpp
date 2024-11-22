#include "Frame.hpp"


class Data : public Frame
{
public:
    Frame Acc;
    Frame Gyro;
    Frame Mag;

    // Frame, который будет меняться в вызываемых функциях
    Frame temp_Frame;    
    // Буферы, в которые будет сохраняться временная информация
    Frame x_Buffer, y_Buffer, z_Buffer; 

    // ########################################################################
    // Перегрузка операторов

    void operator+(const Data& data){
        // Сохранение результата в x_Buffer, y_Buffer, z_Buffer первого слагаемого
        Acc + data.Acc;
        x_Buffer = Acc.frame_Buffer;

        Gyro + data.Gyro;
        y_Buffer = Gyro.frame_Buffer;

        Mag + data.Mag;
        z_Buffer = Mag.frame_Buffer;
    }

    void operator+=(const Data& data){
        // Изменение значений Acc, Gyro, Mag первого слагаемого
        Acc + data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro + data.Gyro;
        Gyro = Gyro.frame_Buffer;

        Mag + data.Mag;
        Mag = Mag.frame_Buffer;
    }

    void operator-(const Data& data){
        // Сохранение результата в x_Buffer, y_Buffer, z_Buffer уменьшаемого
        Acc - data.Acc;
        x_Buffer = Acc.frame_Buffer;

        Gyro - data.Gyro;
        y_Buffer = Gyro.frame_Buffer;

        Mag - data.Mag;
        z_Buffer = Mag.frame_Buffer;
    }

    void operator-=(const Data& data){
        // Изменение значений Acc, Gyro, Mag уменьшаемого
        Acc - data.Acc;
        Acc = Acc.frame_Buffer;

        Gyro - data.Gyro;
        Gyro = Gyro.frame_Buffer;

        Mag - data.Mag;
        Mag = Mag.frame_Buffer;
    }

    void operator/(const float& num){
        // Сохранение результата в x_Buffer, y_Buffer, z_Buffer делимого
        Acc / num;
        x_Buffer = Acc.frame_Buffer;
        
        Gyro / num;
        y_Buffer = Gyro.frame_Buffer;
        
        Mag / num;
        z_Buffer = Mag.frame_Buffer;
    }   

    void operator/=(const float& num){
        // Изменение значений Acc, Gyro, Mag делимого
        Acc / num;
        Acc = Acc.frame_Buffer;
        
        Gyro / num;
        Gyro = Gyro.frame_Buffer;
        
        Mag / num;
        Mag = Mag.frame_Buffer;
    }  

    void operator=(const Data& data){
        Acc =  data.Acc;
        Gyro = data.Gyro;
        Mag =  data.Mag;
    } 

    Frame& operator[](int index){
        if (index == 0) return Acc;
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
    void set_zero_Data()
    {
        Acc.set_zero_Frame();
        Gyro.set_zero_Frame();
        Mag.set_zero_Frame();
    }
};
