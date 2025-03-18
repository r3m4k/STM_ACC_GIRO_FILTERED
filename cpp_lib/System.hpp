/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F30x_SYSTEM_HPP
#define __STM32F30x_SYSTEM_HPP

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_syscfg.h"

#include "RCC.hpp"


class System
{
private:

public:
    RCC_PERIPH RCC;
    System()
    {
        RCC_ClocksTypeDef RCC_Clocks; // structure used for setting up the SysTick Interrupt
        __disable_irq();
        __enable_irq();

        RCC_GetClocksFreq(&RCC_Clocks);
        if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
            while (1); // will end up in this infinite loop if there was an error with Systick_Config
    }
};

#endif /*   __STM32F30x_SYSTEM_HPP   */