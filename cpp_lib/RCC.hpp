/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F30x_RCC_HPP
#define __STM32F30x_RCC_HPP

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"

class RCC_PERIPH
{
protected:

public:
    void RCC_AHBPeriphResetCmd(FunctionalState NewState)
    {
        /* Check the parameters */
        // assert_param(IS_RCC_AHB_RST_PERIPH(RCC_AHBPeriph));
        // assert_param(IS_FUNCTIONAL_STATE(NewState));

        if (NewState != DISABLE)
        {
            RCC->AHBRSTR |= RCC_AHBPeriph;
        }
        else
        {
            RCC->AHBRSTR &= ~RCC_AHBPeriph;
        }
    }
    
}

#endif /*   __STM32F30x_RCC_HPP    */
