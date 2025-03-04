#ifndef LEDS_HPP
#define LEDS_HPP

#include "main.h"
#include "stm32f3_discovery.h"

class LEDS{
public:
    void Init(){
        // for (Led_num = LED3; Led_num != LED_last; Led_num++){    STM_EVAL_LEDInit(Led_num);    }
        STM_EVAL_LEDInit(LED4);
        STM_EVAL_LEDInit(LED3);
        STM_EVAL_LEDInit(LED5);
        STM_EVAL_LEDInit(LED7);
        STM_EVAL_LEDInit(LED9);
        STM_EVAL_LEDInit(LED10);
        STM_EVAL_LEDInit(LED8);
        STM_EVAL_LEDInit(LED6);
    }
    
    // Включение светодиода
    void LedOn(Led_TypeDef Led){    STM_EVAL_LEDOn(Led);    }
    
    // Выключение светодиода
    void LedOff(Led_TypeDef Led){    STM_EVAL_LEDOff(Led);    }

    // Включение всех светодиодов
    void LedsOn(){
        for (Led_num = LED3; Led_num != LED10; Led_num++){    LedOn(Led_num);    }
    }

    // Выключение всех светодиодов
    void LedsOff(){
        for (Led_num = LED3; Led_num != LED10; Led_num++){    LedOff(Led_num);    }
    }

    // Моргание светодиодами
    void Toggle_Leds(){
        STM_EVAL_LEDOn(LED3);
        Delay(100);
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOn(LED4);
        Delay(100);
        STM_EVAL_LEDOff(LED4);
        STM_EVAL_LEDOn(LED6);
        Delay(100);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOn(LED8);
        Delay(100);
        STM_EVAL_LEDOff(LED8);
        STM_EVAL_LEDOn(LED10);
        Delay(100);
        STM_EVAL_LEDOff(LED10);
        STM_EVAL_LEDOn(LED9);
        Delay(100);
        STM_EVAL_LEDOff(LED9);
        STM_EVAL_LEDOn(LED7);
        Delay(100);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOn(LED5);
        Delay(100);
        STM_EVAL_LEDOff(LED5);
    }
}

#endif /*   LEDS_HPP    */