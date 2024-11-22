class Measure;

class Frame
{
public:

    float X_coord;
    float Y_coord;
    float Z_coord;

    float buffer[3] = {0.0f};               // Буффер, который будет использоваться при чтении данных с датчиков

    Measure measure;

    Frame(Measure _measure) {measure = _measure}

    // ########################################################################
    // Перегрузка операторов
    void operator+(const Frame& frame2){
        measure.temp_Frame.X_coord = X_coord + frame2.X_coord;
        measure.temp_Frame.X_coord = Y_coord + frame2.Y_coord;
        measure.temp_Frame.X_coord = Z_coord + frame2.Z_coord;
    }

    void operator-(const Frame& frame2){
        measure.temp_Frame.X_coord = X_coord - frame2.X_coord;
        measure.temp_Frame.X_coord = Y_coord - frame2.Y_coord;
        measure.temp_Frame.X_coord = Z_coord - frame2.Z_coord;
    }

    void operator/(const float& num){
        measure.temp_Frame.X_coord = X_coord / num;
        measure.temp_Frame.X_coord = Y_coord / num;
        measure.temp_Frame.X_coord = Z_coord / num;
    }
    
    void operator=(const Frame& frame){
        X_coord = frame.X_coord;
        Y_coord = frame.Y_coord;
        Z_coord = frame.Z_coord;    
    }

    float& operator[](int index){
        if (index == 0) return X_coord;
        else if (index == 1) return Y_coord;
        else if (index == 2) return Z_coord;
    }    

    // Присваение значений элементов frame к значению элементов массива vector
    void to_Vector(float vector[]){
        X_coord = vector[0];
        Y_coord = vector[1];
        Z_coord = vector[2];
    }

    // ########################################################################
    // Чтение данных с датчиков
    
    friend void ReadAcc(float *pfData);
    friend void ReadGyro(float *pfData);
    friend void ReadMag(float *pfData);

    void Read_Acc()
    {
        ReadAcc((float *)&buffer);
        X_coord = buffer[0];
        Y_coord = buffer[1];
        Z_coord = buffer[2];
    }

    void Read_Gyro()
    {
        ReadGyro((float *)&buffer);
        X_coord = buffer[0];
        Y_coord = buffer[1];
        Z_coord = buffer[2];
    }

    void Read_Mag()
    {
        ReadMag((float *)&buffer);
        X_coord = buffer[0];
        Y_coord = buffer[1];
        Z_coord = buffer[2];
    }
};
