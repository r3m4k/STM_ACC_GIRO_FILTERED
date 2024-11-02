#include <math.h>
#include "main.h"

struct Frame
{
    float X_coord;
    float Y_coord;
    float Z_coord;
};

struct Data
{
    Frame Acc;
    Frame Gyro;
    Frame Mag;
};

class Measure
{
protected:
    Data current, mean;
    float** rotation_matrix = (float**)malloc(3 * sizeof(float*));
    float longitude;
    float sqrt2 = sqrt(2);

public:
    Measure(float phi)
    {
        set_zero_Data(current);
        longitude = phi;
    }

    // ########################################################################
    // Дополнительный функционал для struct Data
    void set_zero_Data(Data &data)
    {

        data.Acc.X_coord = 0;
        data.Acc.Y_coord = 0;
        data.Acc.Z_coord = 0;

        data.Gyro.X_coord = 0;
        data.Gyro.Y_coord = 0;
        data.Gyro.Z_coord = 0;

        data.Mag.X_coord = 0;
        data.Mag.Y_coord = 0;
        data.Mag.Z_coord = 0;
    }

    // ########################################################################
    // Чтение данных с датчиков

    friend void ReadAcc(float *pfData);
    friend void ReadGyro(float *pfData);
    friend void ReadMag(float *pfData);    

    void Read_Data(Data &data)
    {
        Read_Acc(data.Acc);
        Read_Gyro(data.Gyro);
        Read_Mag(data.Mag);
    }

    void Read_Acc(Frame &frame)
    {
        float AccBuffer[3];
        ReadAcc(AccBuffer);
        frame.X_coord = AccBuffer[0];
        frame.Y_coord = AccBuffer[1];
        frame.Z_coord = AccBuffer[2];
    }

    void Read_Gyro(Frame &frame)
    {
        float GyroBuffer[3];
        ReadGyro(GyroBuffer);
        frame.X_coord = GyroBuffer[0];
        frame.Y_coord = GyroBuffer[1];
        frame.Z_coord = GyroBuffer[2];
    }

    void Read_Mag(Frame &frame)
    {
        float MagBuffer[3];
        ReadMag(MagBuffer);
        frame.X_coord = MagBuffer[0];
        frame.Y_coord = MagBuffer[1];
        frame.Z_coord = MagBuffer[2];
    }

    // ########################################################################
    // Математические операции с Data и Frame
    Data Data_summ(Data &data1, Data &data2)
    {
        Data result;

        result.Acc = Frame_sum(data1.Acc, data2.Acc);
        result.Gyro = Frame_sum(data1.Gyro, data2.Gyro);
        result.Mag = Frame_sum(data1.Mag, data2.Mag);

        return result;
    }

    Frame Frame_sum(Frame &frame1, Frame &frame2)
    {
        Frame result;

        result.X_coord = frame1.X_coord + frame2.X_coord;
        result.Y_coord = frame1.Y_coord + frame2.Y_coord;
        result.Z_coord = frame1.Z_coord + frame2.Z_coord;

        return result;
    }

    void Data_division(Data &data, float num)
    {
        Frame_division(data.Acc, num);
        Frame_division(data.Gyro, num);
        Frame_division(data.Mag, num);
    }

    void Frame_division(Frame &frame, float num)
    {
        frame.X_coord /= num;
        frame.Y_coord /= num;
        frame.Z_coord /= num;
    }

    // ########################################################################
    // Выставка датчиков
    void average_Data()
    {
        char degree = 16;
        char jump_mean_degree = 6; // Степень глубины прыгающего среднего

        int max = pow(2, degree - jump_mean_degree);
        int jm_max = pow(2, jump_mean_degree);

        for (int i = 0; i < max; i++)
        {
            Data buffer;
            set_zero_Data(buffer);

            for (int j = 0; j < jm_max; j++)
            {
                Read_Data(current);
                buffer = Data_summ(current, buffer);
            }

            Data_division(buffer, jm_max);
            mean = Data_summ(mean, buffer);
        }

        Data_division(mean, max);
    }

    void set_rotationMatrix()
    {
        // Вывод коэффицентов матрицы поворота, а также описание используемых систем координат можно посмотреть в файле Documentation/Rotation_matrix.pdf
        float** matrix = (float**)malloc(3 * sizeof(float*));   // матрица перехода от системы координат x'y'z' к xyz
        float *G_x, *G_y, *G_z, *W_x, *W_y, *W_z, *H_x, *H_y, *H_z; // Координаты векторов G, W, H в системе координат, связанной с платой

        G_x = &mean.Acc.X_coord;
        G_y = &mean.Acc.Y_coord;
        G_z = &mean.Acc.Z_coord;

        W_x = &mean.Gyro.X_coord;
        W_y = &mean.Gyro.Y_coord;
        W_z = &mean.Gyro.Z_coord;

        H_x = &mean.Mag.X_coord;
        H_y = &mean.Mag.Y_coord;
        H_z = &mean.Mag.Z_coord;

        float G = sqrt((*G_x) * (*G_x) + (*G_y) * (*G_y) + (*G_z) * (*G_z)); // Получаем значение всемирных констант таким способом для того, чтобы не переводить
        float W = sqrt((*W_x) * (*W_x) + (*W_y) * (*W_y) + (*W_z) * (*W_z)); // значение этих констант из системы СИ в систему измерений датчиков.
        float H = sqrt((*H_x) * (*H_x) + (*H_y) * (*H_y) + (*H_z) * (*H_z));

        float W_X, W_Y, W_Z, H_X, H_Y;      // Координаты векторов W, H в системе координат, связанной с Землёй
                                            // и повёрнутой относительно направления на север на -45 градусов вокруг оси OZ (т.е. по часовой стрелке)

        W_X = W * cos(longitude) / sqrt2;
        W_Y = W * cos(longitude) / sqrt2;
        W_Z = W * sin(longitude);

        H_X = H / sqrt2;
        H_Y = H / sqrt2;


        matrix[0][0] = (H_Y * ((*W_x) - W_Z * (*G_x) / G) - W_Y * (*H_x)) / (W_X * H_Y - W_Y * H_X);
        matrix[0][1] = (H_Y * ((*W_y) - W_Z * (*G_y) / G) - W_Y * (*H_y)) / (W_X * H_Y - W_Y * H_X);
        matrix[0][2] = (H_Y * ((*W_z) - W_Z * (*G_z) / G) - W_Y * (*H_z)) / (W_X * H_Y - W_Y * H_X);

        matrix[1][0] = (H_X * ((*W_x) - W_Z * (*G_x) / G) - W_X * (*H_x)) / (W_X * H_Y - W_Y * H_X);
        matrix[1][1] = (H_X * ((*W_y) - W_Z * (*G_y) / G) - W_X * (*H_y)) / (W_X * H_Y - W_Y * H_X);
        matrix[1][2] = (H_X * ((*W_z) - W_Z * (*G_z) / G) - W_X * (*H_z)) / (W_X * H_Y - W_Y * H_X);

        matrix[2][0] = *G_x / G;
        matrix[2][1] = *G_y / G;
        matrix[2][2] = *G_z / G;

        matrix = Mreverse(matrix, 3);

        // Матрица перехода от xyz к XYZ 
        float** Matrix = (float**)malloc(3 * sizeof(float*));

        Matrix[0][0] = 1/sqrt2;
        Matrix[0][1] = -1/sqrt2;
        Matrix[0][2] = 0.0f;

        Matrix[1][0] = 1/sqrt2;
        Matrix[1][1] = 1/sqrt2;
        Matrix[1][2] = 0.0f;

        Matrix[2][0] = 0.0f;
        Matrix[2][1] = 0.0f;
        Matrix[2][2] = 1.0f;

        rotation_matrix = M_multiplication(matrix, Matrix, 3);
    }

    // ########################################################################
    // Математические операции с матрицами

    // Транспонирование матрицы
    float **Transpone(float **matrix, int rows, int cols)
    {
        float **result;
        result = (float **)malloc(cols * sizeof(float *));
        for (int i = 0; i < cols; i++)
        {
            result[i] = (float *)malloc(rows * sizeof(float));
            for (int j = 0; j < rows; j++)
                result[i][j] = matrix[j][i];
        }
        return result;
    }

    // Функция освобождения памяти, выделенной под матрицу
    void Free(float **matrix, int rows)
    {
        if (matrix == 0)
            return; // если память не была выделена, выходим
        for (int i = 0; i < rows; i++)
            free(matrix[i]);
        free(matrix);
    }

    // Получение матрицы без i-й строки и j-го столбца
    // (функция нужна для вычисления определителя и миноров)
    float **GetMatr(float **matrix, int rows, int cols, int row, int col)
    {
        int di, dj;
        float **p = (float **)malloc((rows - 1) * sizeof(float *));
        di = 0;
        for (int i = 0; i < rows - 1; i++)
        {                 // проверка индекса строки
            if (i == row) // строка совпала с вычеркиваемой
                di = 1;   // - дальше индексы на 1 больше
            dj = 0;
            p[i] = (float *)malloc((cols - 1) * sizeof(float));
            for (int j = 0; j < cols - 1; j++)
            {                 // проверка индекса столбца
                if (j == col) // столбец совпал с вычеркиваемым
                    dj = 1;   // - дальше индексы на 1 больше
                p[i][j] = matrix[i + di][j + dj];
            }
        }
        return p;
    }

    // Рекурсивное вычисление определителя
    float Determinant(float **matrix, int m)
    {
        int k;
        float **p = 0;
        float d = 0;
        k = 1; //(-1) в степени i

        // if (m < 1)
        // {
        //     printf("Определитель вычислить невозможно!");
        //     return 0;
        // }

        if (m == 1)
        {
            d = matrix[0][0];
            return (d);
        }
        if (m == 2)
        {
            d = matrix[0][0] * matrix[1][1] - (matrix[1][0] * matrix[0][1]);
            return (d);
        }
        if (m > 2)
        {
            for (int i = 0; i < m; i++)
            {
                p = GetMatr(matrix, m, m, i, 0);
                d = d + k * matrix[i][0] * Determinant(p, m - 1);
                k = -k;
            }
        }
        Free(p, m - 1);
        return (d);
    }

    // Обратная матрица
    float **Mreverse(float **matrix, int m)
    {
        float **result = (float **)malloc(m * sizeof(float *));
        float det = Determinant(matrix, m); // находим определитель исходной матрицы
        for (int i = 0; i < m; i++)
        {
            result[i] = (float *)malloc(m * sizeof(float));
            for (int j = 0; j < m; j++)
            {
                result[i][j] = Determinant(GetMatr(matrix, m, m, i, j), m - 1);
                if ((i + j) % 2 == 1)       // если сумма индексов строки и столбца нечетная
                    result[i][j] = -result[i][j]; // меняем знак минора
                result[i][j] = result[i][j] / det;
            }
        }
        return Transpone(result, m, m);
    }

    // Умножение квадратных матриц размерности m
    float **M_multiplication(float **matrix1, float **matrix2, int m)
    {
        float **result = (float **)malloc(m * sizeof(float *));

        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                result[i][j] = 0;
                for (int k = 0; k < m; k++)
                {
                    result[i][j] += matrix1[i][k] * matrix2[k][j];
                }
            }
        }

        return result;
    }
    // ########################################################################
};
