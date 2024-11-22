#include "Frame.hpp"

class Measure;

class Data
{
public:

    Frame Acc(measure);
    Frame Gyro(measure);
    Frame Mag(measure);

    Data(Measure& _measure){ measure = _measure; }

    // ########################################################################
    // Перегрузка операторов

    void operator+(const Data& data){
        Acc + data.Acc;
        measure.temp_Data.Acc = measure.temp_Frame;
        
        Gyro + data.Gyro;
        measure.temp_Data.Gyro = measure.temp_Frame;
        
        Mag + data.Mag;
        measure.temp_Data.Mag = measure.temp_Frame;
    }

    void operator-(const Data& data){
        Acc - data.Acc;
        measure.temp_Data.Acc = measure.temp_Frame;
        
        Gyro - data.Gyro;
        measure.temp_Data.Gyro = measure.temp_Frame;
        
        Mag - data.Mag;
        measure.temp_Data.Mag = measure.temp_Frame;
    }

    void operator/(const float& num){
        Acc / num;
        measure.temp_Data.Acc = measure.temp_Frame;
        
        Gyro / num;
        measure.temp_Data.Gyro = measure.temp_Frame;
        
        Mag / num;
        measure.temp_Data.Mag = measure.temp_Frame;
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
        Acc.X_coord = 0;
        Acc.Y_coord = 0;
        Acc.Z_coord = 0;

        Gyro.X_coord = 0;
        Gyro.Y_coord = 0;
        Gyro.Z_coord = 0;

        Mag.X_coord = 0;
        Mag.Y_coord = 0;
        Mag.Z_coord = 0;
    }
};
