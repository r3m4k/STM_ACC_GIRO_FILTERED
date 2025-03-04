/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F30x_GPIO_HPP
#define __STM32F30x_GPIO_HPP

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "RCC.hpp"

/* Defines  ------------------------------------------------------------------*/
#define GPIOx_TYPE          GPIO_TypeDef    
#define GPIOx_PIN           uint32_t    

#define GPIOx_MODE          GPIOMode_TypeDef     
#define GPIOx_SPEED         GPIOSpeed_TypeDef   
#define GPIOx_OTYPE         GPIOOType_TypeDef
#define GPIOx_PUPD          GPIOPuPd_TypeDef

#define HIGH_SPEED          GPIO_Speed_50MHz
#define FAST_SPEED          GPIO_Speed_10MHz
#define MEDIUM_SPEED        GPIO_Speed_2MHz


class GPIOx : protected RCC_PERIPH{
    
protected:
    GPIOx_TYPE  GPIOx_Type;
    
    GPIOx_MODE   GPIOx_Mode;
    GPIOx_SPEED  GPIOx_Speed;
    GPIOx_OTYPE  GPIOx_OType;
    GPIOx_PUPD   GPIOx_PuPd;
    
public:
    GPIOx_PIN    GPIOx_Pin;
    
    void Init(){}

    void DeInit(){
        RCC_AHBPeriphResetCmd(RCC_AHBPeriph, ENABLE);
        RCC_AHBPeriphResetCmd(RCC_AHBPeriph, DISABLE);
    }
    
    void GPIO_PinLockConfig(){}

    uint8_t GPIO_ReadInputDataBit(){}

    uint16_t GPIO_ReadInputData(){}

    uint8_t GPIO_ReadOutputDataBit(){}

    uint16_t GPIO_ReadOutputData(){}

    void GPIO_SetBits(){}

    void GPIO_ResetBits(){}

    void GPIO_WriteBit(BitAction BitVal);

    void GPIO_Write(uint16_t PortVal);

    /* GPIO Alternate functions configuration functions ***************************/
    void GPIO_PinAFConfig(uint16_t GPIO_PinSource, uint8_t GPIO_AF);

}


class GPIOA : protected GPIOx{
    GPIOA(GPIOx_PIN Pin = GPIO_Pin_All, GPIOx_MODE Mode = GPIO_Mode_IN, GPIOx_SPEED Speed = HIGH_SPEED, 
            GPIOx_OTYPE OType = GPIO_OType_PP, GPIOx_PUPD PuPd = GPIO_PuPd_NOPULL){

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