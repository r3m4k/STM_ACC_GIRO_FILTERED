#include "main.h"
#include <math.h>

#define DEGREE 14       // Степень двойки для цикла

extern __IO uint32_t TimingDelay;

void Get_start_angle(){
    MeasureFrame Acc_Current, Acc_Mean;

    uint32_t Acc_Mean_Buffer[3] = {0};
    
    for(int i = 0; i < 2**DEGREE; i++)
    {
        Read_Acc(&Acc_Current);
        Acc_Mean_Buffer[0] += (uint32_t)(Acc_Current.X);
        Acc_Mean_Buffer[1] += (uint32_t)(Acc_Current.Y);
        Acc_Mean_Buffer[2] += (uint32_t)(Acc_Current.Z);
    }

    Acc_Mean.X = (uint8_t)(Acc_Mean_Buffer[0] >> DEGREE);    // Делим на COLLECTING_NUM, не переводя числа в float 
    Acc_Mean.Y = (uint8_t)(Acc_Mean_Buffer[1] >> DEGREE);
    Acc_Mean.Z = (uint8_t)(Acc_Mean_Buffer[2] >> DEGREE);
}