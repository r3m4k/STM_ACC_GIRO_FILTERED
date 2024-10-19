  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include "COM_IO.h"
#include "Sensors.h"
//#include "stm32f30x_iwdg.h"

// ===============================================================================

/* Exported functions ------------------------------------------------------- */
#ifdef __cplusplus
 extern "C" {
#endif 

struct MeasureFrame
{
	uint16_t X;
	uint16_t Y;
	uint16_t Z;
};

void Read_Acc(struct MeasureFrame *Frame);
void Read_Gyro(struct MeasureFrame *Frame);
void Read_Mag(struct MeasureFrame *Frame);

void LedsInit(void);
void Toggle_Leds(void);	
	 
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

