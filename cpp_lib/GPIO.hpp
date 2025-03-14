/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F30x_GPIO_HPP
#define __STM32F30x_GPIO_HPP

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <_stdint.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "RCC.hpp"

/* Defines  ------------------------------------------------------------------*/
#define GPIOx_TYPE GPIO_TypeDef
#define GPIOx_PIN uint32_t

#define GPIOx_MODE GPIOMode_TypeDef
#define GPIOx_SPEED GPIOSpeed_TypeDef
#define GPIOx_OTYPE GPIOOType_TypeDef
#define GPIOx_PUPD GPIOPuPd_TypeDef

#define HIGH_SPEED GPIO_Speed_50MHz
#define FAST_SPEED GPIO_Speed_10MHz
#define MEDIUM_SPEED GPIO_Speed_2MHz

class GPIOx
{

protected:
    uint32_t RCC_Periph;

    GPIOx_TYPE GPIOx_Type;

    GPIOx_MODE GPIOx_Mode;
    GPIOx_SPEED GPIOx_Speed;
    GPIOx_OTYPE GPIOx_OType;
    GPIOx_PUPD GPIOx_PuPd;

public:
    GPIOx_PIN GPIOx_Pin;

    void Init()
    {
        uint32_t pinpos = 0x00, pos = 0x00, currentpin = 0x00;

        /* Check the parameters */
        // assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
        // assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin));
        // assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
        // assert_param(IS_GPIO_PUPD(GPIO_InitStruct->GPIO_PuPd));

        /*-------------------------- Configure the port pins -----------------------*/
        /*-- GPIO Mode Configuration --*/
        for (pinpos = 0x00; pinpos < 0x10; pinpos++)
        {
            pos = ((uint32_t)0x01) << pinpos;

            /* Get the port pins position */
            currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

            if (currentpin == pos)
            {
                if ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) || (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF))
                {
                    /* Check Speed mode parameters */
                    assert_param(IS_GPIO_SPEED(GPIO_InitStruct->GPIO_Speed));

                    /* Speed mode configuration */
                    GPIOx_Type->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
                    GPIOx_Type->OSPEEDR |= ((uint32_t)(GPIO_InitStruct->GPIO_Speed) << (pinpos * 2));

                    /* Check Output mode parameters */
                    assert_param(IS_GPIO_OTYPE(GPIO_InitStruct->GPIO_OType));

                    /* Output mode configuration */
                    GPIOx_Type->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos));
                    GPIOx_Type->OTYPER |= (uint16_t)(((uint16_t)GPIO_InitStruct->GPIO_OType) << ((uint16_t)pinpos));
                }

                GPIOx_Type->MODER &= ~(GPIO_MODER_MODER0 << (pinpos * 2));

                GPIOx_Type->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));

                /* Pull-up Pull down resistor configuration */
                GPIOx_Type->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
                GPIOx_Type->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
            }
        }
    }

    void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
    {
        uint32_t pinpos = 0x00, pos = 0x00, currentpin = 0x00;

        /* Check the parameters */
        assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
        assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin));
        assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
        assert_param(IS_GPIO_PUPD(GPIO_InitStruct->GPIO_PuPd));

        /*-------------------------- Configure the port pins -----------------------*/
        /*-- GPIO Mode Configuration --*/
        for (pinpos = 0x00; pinpos < 0x10; pinpos++)
        {
            pos = ((uint32_t)0x01) << pinpos;

            /* Get the port pins position */
            currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

            if (currentpin == pos)
            {
                if ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) || (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF))
                {
                    /* Check Speed mode parameters */
                    assert_param(IS_GPIO_SPEED(GPIO_InitStruct->GPIO_Speed));

                    /* Speed mode configuration */
                    GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
                    GPIOx->OSPEEDR |= ((uint32_t)(GPIO_InitStruct->GPIO_Speed) << (pinpos * 2));

                    /* Check Output mode parameters */
                    assert_param(IS_GPIO_OTYPE(GPIO_InitStruct->GPIO_OType));

                    /* Output mode configuration */
                    GPIOx->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos));
                    GPIOx->OTYPER |= (uint16_t)(((uint16_t)GPIO_InitStruct->GPIO_OType) << ((uint16_t)pinpos));
                }

                GPIOx->MODER &= ~(GPIO_MODER_MODER0 << (pinpos * 2));

                GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));

                /* Pull-up Pull down resistor configuration */
                GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
                GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
            }
        }
    }

    void DeInit()
    {
        RCC_AHBPeriphResetCmd(ENABLE);
        RCC_AHBPeriphResetCmd(DISABLE);
    }

    void GPIO_PinLockConfig() {}

    uint8_t GPIO_ReadInputDataBit() {}

    uint16_t GPIO_ReadInputData() {}

    uint8_t GPIO_ReadOutputDataBit() {}

    uint16_t GPIO_ReadOutputData() {}

    void GPIO_SetBits() {}

    void GPIO_ResetBits() {}

    void GPIO_WriteBit(BitAction BitVal);

    void GPIO_Write(uint16_t PortVal);

    /* GPIO Alternate functions configuration functions ***************************/
    void GPIO_PinAFConfig(uint16_t GPIO_PinSource, uint8_t GPIO_AF);
}

class GPIOA : protected GPIOx
{
    GPIOA(GPIOx_PIN Pin = GPIO_Pin_All, GPIOx_MODE Mode = GPIO_Mode_IN, GPIOx_SPEED Speed = HIGH_SPEED,
          GPIOx_OTYPE OType = GPIO_OType_PP, GPIOx_PUPD PuPd = GPIO_PuPd_NOPULL)
    {

        GPIOx_Pin = Pin;
        GPIOx_Mode = Mode;
        GPIOx_Speed = Speed;
        GPIOx_OType = OType;
        GPIOx_PuPd = PuPd;

        RCC_AHBPeriph = RCC_AHBPeriph_GPIOA;
    }

protected:
public:
}

#endif /*   __STM32F30x_GPIO_HPP    */