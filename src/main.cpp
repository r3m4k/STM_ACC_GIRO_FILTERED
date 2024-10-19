#include "Drv_Gpio.h"
#include "Drv_Uart.h"
#include "main.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F3 empty sample (trace via NONE).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

/* #global variables -----------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks; // structure used for setting up the SysTick Interrupt

// Unused global variables that have to be included to ensure correct compiling
// ###### DO NOT CHANGE ######
// ===============================================================================
__IO uint32_t TimingDelay = 0;                     // used with the Delay function
__IO uint8_t DataReady = 0;
__IO uint32_t USBConnectTimeOut = 100;
__IO uint32_t UserButtonPressed = 0;
__IO uint8_t PrevXferComplete = 1;
__IO uint8_t buttonState;
// ===============================================================================
// Вынесем чувствительность акселерометра и магнитометра в отдельную переменную, чтобы максимально долго была возможность не работать с float
// Они будут заполняться при чтении данных с датчиков
float LSM_Acc_Sensitivity = 0;  
uint16_t Magn_Sensitivity_XY = 0;
uint16_t Magn_Sensitivity_Z = 0;

uint16_t cDivider; // Число на которое надо разделить значения с акселерометра
// ===============================================================================

int main()
{
    __disable_irq();
    __enable_irq();

	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);       //will end up in this infinite loop if there was an error with Systick_Config
	
    // Иницилизируем перифирию
    LedsInit();
    
    InitGPIO();
    GYRO_INIT();
    MAG_INIT();
    ACC_INIT();
    InitUart(115200);   
    
    // Поморгаем светодиодами
    Toggle_Leds();

    // Чтение данных с датчиков
    MeasureFrame Acc_Frame;
    MeasureFrame Gyro_Frame;
    MeasureFrame Mag_Frame;

    while (1) 
    {
        Read_Acc(&Acc_Frame);
        Read_Gyro(&Gyro_Frame);
        Read_Mag(&Mag_Frame);

        Toggle_Leds();
    }
}

#pragma GCC diagnostic pop

// -------------------------------------------------------------------------------
void Read_Acc(struct MeasureFrame *Frame)
{
    uint16_t AccBuffer[3];
    ReadAcc(AccBuffer);
    // AccBuffer /= сDiveder
    // Что не учитывается при хранении данных в угоду производительности
    // тк процессор медленно обрабатывает числа с плавающей точкой 
    Frame->X = AccBuffer[0];
    Frame->Y = AccBuffer[1];
    Frame->Z = AccBuffer[2];
}

void Read_Gyro(struct MeasureFrame *Frame)
{
    uint16_t GyroBuffer[3];
    ReadGyro(GyroBuffer);
    Frame->X = GyroBuffer[0];
    Frame->Y = GyroBuffer[1];
    Frame->Z = GyroBuffer[2];
}

void Read_Mag(struct MeasureFrame *Frame)
{
    uint16_t MagBuffer[3];
    ReadMag(MagBuffer);
    // MagBuffer = -1 * ((MagBuffer / 1000) / Magn_Sensitivity_XYZ)
    // Что не учитывается при хранении данных в угоду производительности
    // тк процессор медленно обрабатывает числа с плавающей точкой  
    Frame->X = MagBuffer[0];
    Frame->Y = MagBuffer[1];
    Frame->Z = MagBuffer[2];
}

void LedsInit(void)
{
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED6);
}

// -------------------------------------------------------------------------------
void Toggle_Leds(void)
{
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

void Error_Handler(void)
{
    /* Turn LED10/3 (RED) on */
    STM_EVAL_LEDOn(LED10);
    STM_EVAL_LEDOn(LED3);
    while (1)
    {
    }
}

// Function to insert a timing delay of nTime
// ###### DO NOT CHANGE ######
void Delay(__IO uint32_t nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0){}
    // for (int i = 0; i < 1000000; i++){}
}

// Function to Decrement the TimingDelay variable.
// ###### DO NOT CHANGE ######
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}

// Unused functions that have to be included to ensure correct compiling
// ###### DO NOT CHANGE ######
// =======================================================================
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
    return 0;
}

uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
    return 0;
}
// =======================================================================
