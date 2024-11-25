#include <math.h>

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include "COM_IO.h"
#include "Sensors.h"

// #define ARM_MATH_CM4
// #include "../system/include/cmsis/arm_math.h"


//#include "stm32f30x_iwdg.h"

// ===============================================================================

/* Exported functions ------------------------------------------------------- */
#ifdef __cplusplus
 extern "C" {
#endif 

void LedsInit(void);
void Toggle_Leds(void);	
void LedsOn(void);
void LedsOff(void);
	 
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

float my_sqrt(float num);
float my_pow(float num, int n);
float my_cos(float num);
float my_sin(float num);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

