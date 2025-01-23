#include "Drv_Gpio.h"
#include "Drv_Uart.h"

#include "main.h"

#include "Measure.hpp"

#define PI 3.14159265358979f


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
#define SyncroByte 0x53     // Флаг начала посылки от ДПП 
#define MaxDataSize 25      // Размер данных от ДПП                 Почему 25?

unsigned int stage, Index;
bool MessageFound;

enum { WantSyncro, WantSize, WantStatus, WantCommandCode, WantData, WantConsum};

unsigned char ConSummIn;        // Контрольная сумма полученных данных
int DataSize, DataIndex;
unsigned char DppCode[MaxDataSize] = {0};
unsigned char DataBuffer[MaxDataSize] = {0};
unsigned char OldDppCode;
unsigned int MCount = 0;
int ViewTact = 0;
// -------------------------------------------------------------------------------
uint16_t previousState = 0;            // Переменная для таймера

Measure measure(55.7522 * PI / 180);

int main()
{
    __disable_irq();
    __enable_irq();

	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);       //will end up in this infinite loop if there was an error with Systick_Config
 
    // Инициализируем периферию
    LedsInit();
    InitGPIO();

    // Инициализируем Virtual Com Port
    VCP_ResetPort();        // Подтянули ножку d+ к нулю для правильной идентификации
    VCP_Init();        

    // Инициализируем UART и датчики
    GYRO_INIT();
    ACC_INIT();

    // Инициализация таймера и его настройка
    TimerInit();  

    Toggle_Leds();      // Поморгаем светодиодами после успешной инициализации

    measure.initial_setting();

    // Запускаем таймер 
    TIM_Cmd(TIM4, ENABLE);

    // Включим зелёные светодиоды для указания корректной работы 
    LedOn(LED6);
    LedOn(LED7);

    // Начнём работу
    while (1) 
    {   
        // measure.measuring();        
        continue;
    }
}

// -------------------------------------------------------------------------------
// Настройка UART
void InitDecoder()
{
    stage = WantSyncro;
    Index = 0;
    // OldDppCode = 0;
    MessageFound = false;
};

extern "C" void ProcessInByte(unsigned char Bt) 
{
    LedOn(LED5);

	switch (stage)
	{
	case WantSyncro:
		if (Bt == SyncroByte)
			{
				stage=WantSize;
				ConSummIn=Bt;
			}	
		break;
	case WantSize:
		ConSummIn += Bt;
		DataSize = Bt;
		stage = WantData;
		break;
	case WantData:
		if ((DataIndex < DataSize) && (DataIndex < MaxDataSize))
			{
				ConSummIn+=Bt;
				DataBuffer[DataIndex]=Bt;
			}
		if (++DataIndex==DataSize) 
			stage=WantConsum;
		break;
	case WantConsum:
        if (Bt == (ConSummIn))
        {
            for (int i = 0; i < DataSize; i++)
                DppCode[i] = DataBuffer[i];
                
        //	DppCode=*((unsigned int*)(&DataBuffer));
        }	
		MessageFound = true;	
		ConSummIn = 0;
		DataIndex=0;
		stage = WantSyncro;
		break;
	}

    LedOff(LED5);
}
// -------------------------------------------------------------------------------
// Настройка светодиодов
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

void LedsOn(){
    STM_EVAL_LEDOn(LED3);
    STM_EVAL_LEDOn(LED4);
    STM_EVAL_LEDOn(LED5);
    STM_EVAL_LEDOn(LED6);
    STM_EVAL_LEDOn(LED7);
    STM_EVAL_LEDOn(LED8);
    STM_EVAL_LEDOn(LED9);
    STM_EVAL_LEDOn(LED10);
}

void LedsOff(){
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED5);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED10);
}

void LedOn(Led_TypeDef Led){   STM_EVAL_LEDOn(Led);   }

void LedOff(Led_TypeDef Led){  STM_EVAL_LEDOff(Led);  }

// -------------------------------------------------------------------------------
// Настройка таймера
void TimerInit(void){

    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable TIM clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* Enable the Tim4 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    uint16_t TIMER_PRESCALER = 10000;    // При таком предделителе получается один тик таймера на 1.25 мс

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    /* Set the default configuration */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = TIMER_PRESCALER - 1;
    TIM_TimeBaseStructure.TIM_Period = 40000;      // Выставим задержку в 5 секунд
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

void TIM4_IRQHandler(void)
{ 
    LedOn(LED9);
    // Если на выходе был 0
    if (previousState == 0)
    {
        // Выставляем единицу на выходе
        previousState = 1;
        LedOn(LED4);
    }
    else
    {
        // Выставляем ноль на выходе
        previousState = 0;
        LedOff(LED4);
    }
    
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     // Очистим регистр наличия прерывания от датчика
    LedOff(LED9);
}
// -------------------------------------------------------------------------------

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
