class Frame
{
public:

    float X_coord;
    float Y_coord;
    float Z_coord;

    float frame_Buffer[3] = {0.0f};               // Буфер, в который будет заполняться временная информация

    Frame(){
        X_coord = 0;
        Y_coord = 0;
        Z_coord = 0;
    }

    // ########################################################################
    // Перегрузка операторов

    void operator+(const Frame& frame){
        // Сохранение результата в frame_Buffer первого слагаемого
        frame_Buffer[0] = X_coord + frame.X_coord;
        frame_Buffer[1] = Y_coord + frame.Y_coord;
        frame_Buffer[2] = Z_coord + frame.Z_coord;
    }

    void operator+=(const Frame& frame){
        // Изменение значений X_coord, Y_coord, Z_coord первого слагаемого
        X_coord += frame.X_coord;
        Y_coord += frame.X_coord;
        Z_coord += frame.X_coord;
    }

    void operator-(const Frame& frame){
        // Сохранение результата в frame_Buffer уменьшаемого
        frame_Buffer[0] = X_coord - frame.X_coord;
        frame_Buffer[1] = Y_coord - frame.Y_coord;
        frame_Buffer[2] = Z_coord - frame.Z_coord;
    }

    void operator-=(const Frame& frame){
        // Изменение значений X_coord, Y_coord, Z_coord уменьшаемого
        X_coord -= frame.X_coord;
        Y_coord -= frame.X_coord;
        Z_coord -= frame.X_coord;
    }

    void operator/(const float num){
        // Сохранение результата в frame_Buffer делимого
        frame_Buffer[0] = X_coord / num;
        frame_Buffer[1] = Y_coord / num;
        frame_Buffer[2] = Z_coord / num;
    }

    void operator=(const Frame& frame){
        X_coord = frame.X_coord;
        Y_coord = frame.Y_coord;
        Z_coord = frame.Z_coord;    
    }

    void operator=(const float arr[]){
        X_coord = arr[0];
        Y_coord = arr[1];
        Z_coord = arr[2];
    }

    float& operator[](const int index){
        if (index == 0) return X_coord;
        else if (index == 1) return Y_coord;
        else if (index == 2) return Z_coord;
    } 

    // ########################################################################
    // Чтение данных с датчиков
    
    friend void ReadAcc(float *pfData);
    friend void ReadGyro(float *pfData);
    friend void ReadMag(float *pfData);

    void Read_Acc()
    {
        ReadAcc(frame_Buffer);
        X_coord = frame_Buffer[0];
        Y_coord = frame_Buffer[1];
        Z_coord = frame_Buffer[2];
    }

    void Read_Gyro()
    {
        ReadGyro(frame_Buffer);
        X_coord = frame_Buffer[0];
        Y_coord = frame_Buffer[1];
        Z_coord = frame_Buffer[2];
    }

    void Read_Mag()
    {
        ReadMag(frame_Buffer);
        X_coord = frame_Buffer[0];
        Y_coord = frame_Buffer[1];
        Z_coord = frame_Buffer[2];
    }

    // ########################################################################
    // Дополнительный функционал

    void set_zero_frame_Buffer(){
        frame_Buffer[0] = 0;
        frame_Buffer[1] = 0;
        frame_Buffer[2] = 0;
    }

    void set_zero_Frame(){
        X_coord = 0;
        Y_coord = 0;
        Z_coord = 0;
    }

};
