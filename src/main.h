#include <math.h>

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include "COM_IO.h"
#include "Sensors.h"


struct Frame
{
    float X_coord;
    float Y_coord;
    float Z_coord;
};

struct Data
{
    struct Frame Acc;
    struct Frame Gyro;
    struct Frame Mag;
};

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

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

