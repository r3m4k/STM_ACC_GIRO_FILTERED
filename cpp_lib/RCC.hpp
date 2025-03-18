/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F30x_RCC_HPP
#define __STM32F30x_RCC_HPP

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"

/* Defines  ------------------------------------------------------------------*/
#define __RCC_INDEX uint32_t
#define __RCC_AHB_PERIPH 0
#define __RCC_APB1_PERIPH 1
#define __RCC_APB2_PERIPH 2

class RCC_AHB_PERIPH
{
private:
    /* data */
protected:
    void _ResetCmd(FunctionalState NewState)
    {
        void RCC_AHBPeriphResetCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
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

    void _ClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
    {
        /* Check the parameters */
        assert_param(IS_RCC_AHB_PERIPH(RCC_AHBPeriph));
        assert_param(IS_FUNCTIONAL_STATE(NewState));

        if (NewState != DISABLE)
        {
            RCC->AHBENR |= RCC_AHBPeriph;
        }
        else
        {
            RCC->AHBENR &= ~RCC_AHBPeriph;
        }
    }

public:
};

class RCC_APB1_PERIPH
{
private:
    /* data */
protected:
    void _ResetCmd(FunctionalState NewState)
    {
        void _ResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
        {
            /* Check the parameters */
            assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
            assert_param(IS_FUNCTIONAL_STATE(NewState));

            if (NewState != DISABLE)
            {
                RCC->APB1RSTR |= RCC_APB1Periph;
            }
            else
            {
                RCC->APB1RSTR &= ~RCC_APB1Periph;
            }
        }
    }

    void _ClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
    {
        /* Check the parameters */
        // assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
        // assert_param(IS_FUNCTIONAL_STATE(NewState));

        if (NewState != DISABLE)
        {
            RCC->APB1ENR |= RCC_APB1Periph;
        }
        else
        {
            RCC->APB1ENR &= ~RCC_APB1Periph;
        }
    }

public:
};

class RCC_APB2_PERIPH
{
private:
    /* data */
protected:
    void _ResetCmd(FunctionalState NewState)
    {
        void _ResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
        {
            /* Check the parameters */
            // assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
            // assert_param(IS_FUNCTIONAL_STATE(NewState));

            if (NewState != DISABLE)
            {
                RCC->APB2RSTR |= RCC_APB2Periph;
            }
            else
            {
                RCC->APB2RSTR &= ~RCC_APB2Periph;
            }
        }
    }

    void _ClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
    {
        /* Check the parameters */
        // assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
        // assert_param(IS_FUNCTIONAL_STATE(NewState));

        if (NewState != DISABLE)
        {
            RCC->APB2ENR |= RCC_APB2Periph;
        }
        else
        {
            RCC->APB2ENR &= ~RCC_APB2Periph;
        }
    }

public:
};

class RCC_PERIPH : RCC_AHB_PERIPH, RCC_APB1_PERIPH, RCC_APB2_PERIPH
{
    __RCC_INDEX get_RCCIndex(uint32_t RCC_AHBPeriph) _
    {
        if (RCC_AHBPeriph == (RCC_AHBPeriph_ADC34 ||
                              RCC_AHBPeriph_ADC12 ||
                              RCC_AHBPeriph_GPIOA ||
                              RCC_AHBPeriph_GPIOB ||
                              RCC_AHBPeriph_GPIOC ||
                              RCC_AHBPeriph_GPIOD ||
                              RCC_AHBPeriph_GPIOE ||
                              RCC_AHBPeriph_GPIOF ||
                              RCC_AHBPeriph_TS ||
                              RCC_AHBPeriph_CRC ||
                              RCC_AHBPeriph_FLITF ||
                              RCC_AHBPeriph_SRAM ||
                              RCC_AHBPeriph_DMA2 ||
                              RCC_AHBPeriph_DMA1))
        {
            return __RCC_AHB_PERIPH;
        }

        if (RCC_AHBPeriph == (RCC_APB2Periph_SYSCFG ||
                              RCC_APB2Periph_TIM1 ||
                              RCC_APB2Periph_SPI1 ||
                              RCC_APB2Periph_TIM8 ||
                              RCC_APB2Periph_USART1 ||
                              RCC_APB2Periph_TIM15 ||
                              RCC_APB2Periph_TIM16 ||
                              RCC_APB2Periph_TIM17))
        {
            return __RCC_APB2_PERIPH;
        }

        if (RCC_AHBPeriph == (RCC_APB1Periph_TIM2 ||
                              RCC_APB1Periph_TIM3 ||
                              RCC_APB1Periph_TIM4 ||
                              RCC_APB1Periph_TIM6 ||
                              RCC_APB1Periph_TIM7 ||
                              RCC_APB1Periph_WWDG ||
                              RCC_APB1Periph_SPI2 ||
                              RCC_APB1Periph_SPI3 ||
                              RCC_APB1Periph_USART2 ||
                              RCC_APB1Periph_USART3 ||
                              RCC_APB1Periph_UART4 ||
                              RCC_APB1Periph_UART5 ||
                              RCC_APB1Periph_I2C1 ||
                              RCC_APB1Periph_I2C2 ||
                              RCC_APB1Periph_USB ||
                              RCC_APB1Periph_CAN1 ||
                              RCC_APB1Periph_PWR ||
                              RCC_APB1Periph_DAC))
        {
            return __RCC_APB1_PERIPH;
        }
    }

protected:

public:
    void ResetCmd(uint32_t RCC_Periph, FunctionalState NewState)
    {
        switch (get_RCCIndex(RCC_Periph))
        {
        case __RCC_AHB_PERIPH:
            RCC_AHB_PERIPH::_ResetCmd(RCC_Periph, NewState);
            break;

        case __RCC_APB1_PERIPH:
            RCC_APB1_PERIPH::_ResetCmd(RCC_Periph, NewState);
            break;

        case __RCC_APB2_PERIPH:
            RCC_APB2_PERIPH::_ResetCmd(RCC_Periph, NewState);
            break;
        }
    }

    void ClockCmd(uint32_t RCC_Periph, FunctionalState NewState){
        switch (get_RCCIndex(RCC_Periph))
        {
            case __RCC_AHB_PERIPH:
                RCC_AHB_PERIPH::_ClockCmd(RCC_Periph, NewState);
                break;
    
            case __RCC_APB1_PERIPH:
                RCC_APB1_PERIPH::_ClockCmd(RCC_Periph, NewState);
                break;
    
            case __RCC_APB2_PERIPH:
                RCC_APB2_PERIPH::_ClockCmd(RCC_Periph, NewState);
                break;
            }
    }
    
}

#endif /*   __STM32F30x_RCC_HPP    */
