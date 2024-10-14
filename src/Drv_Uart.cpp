
#include "Drv_Uart.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_misc.h"

#include "Drv_Gpio.h"

CBuffer<unsigned char> TxBuff;
unsigned char TXBUFF[256];
CBuffer<unsigned char> RxBuff;
unsigned char RXBUFF[256];

extern bool pc8;

void InitUart(int Speed) // UART configuration
{
    TxBuff.Init(256, (int)&TXBUFF);
    RxBuff.Init(256, (int)&RXBUFF);

    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    /* Enable USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_7);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // эта нога управления направлением приемопередатчика -> RTS USART1 AF7
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = Speed;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = /*USART_Mode_Rx |*/ USART_Mode_Tx;

    if (pc8)
    {
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
    }
    else
    {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    }

    /* USART configuration */
    USART_Init(USART1, &USART_InitStructure);
    // Разрешаю аппаратное управление передатчиком для RS485
    // ВНИМАНИЕ!!!! Если используется STM32F3....discovery board - необходимо закоротить
    // на ней перемычки  SB21 и SB22 для подачи сигнала RTS/DE на краевые контакты платы.
    USART_SetDEDeassertionTime(USART1, 5);
    USART_SetDEAssertionTime(USART1, 5);
    USART_DECmd(USART1, ENABLE);
    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
    Uart_irq_enable();
    return;
}

void Uart_irq_enable(void)
{
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

void Uart_irq_disable(void)
{
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

void UartSendChar(int c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    } // Loop until transmit data register is empty
}

void SendUartNibb(int N)
{
    N &= 0xf;
    if (N <= 9)
        UartSendChar('0' + N);
    else
        UartSendChar('A' + N - 10);
}

void SendUartByte(unsigned char B)
{
    SendUartNibb(B >> 4);
    SendUartNibb(B);
}

void SendUartNum(int N)
{
    for (int i = 0; i < 8; i++)
        SendUartNibb(N >> ((7 - i) * 4));
}

void UartSendString(unsigned char *Buff, int Size)
{
    for (int i = 0; i < Size; i++)
        UartSendChar(Buff[i]);
}

unsigned char UartSendBuff(unsigned char *Buff, int Size)
{
    unsigned char ConSum = 0;
    if (!Buff)
        return 0;
    if (!Size)
        return 0;
    if (TxBuff.IsEmpty()) // самое начало передачи-текущий буфер пустой
    {
        for (int i = 1; i < Size; i++) // кладу все кроме первого байта в буфер
        {
            ConSum += Buff[i];
            TxBuff.WriteTo(Buff[i]);
        }
        ConSum += Buff[0];
        USART_SendData(USART1, Buff[0]); // а первый байт передаю сразу, от него потом пойдут прерывания
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
    else // в буфере уже что-то есть от предыдущей жизни
        for (int i = 0; i < Size; i++)
        {
            ConSum += Buff[i];
            for (int j = 0; (j < 10000000) && TxBuff.IsFull(); j++)
                ;                    // если буфер полон, то ждем пока опустошится
            TxBuff.WriteTo(Buff[i]); //  - новая инфа идет в конец буфера
        }
    return ConSum;
}

extern "C" void __attribute__((weak)) ProcessInByte(unsigned char Bt) { Bt = Bt + 1; } // затычка на случай, если это не будет определено где-то еще

extern "C" void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // было прерывание от приемника
        ProcessInByte(USART_ReceiveData(USART1));

    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) // было прерывание от передатчика
    {
        if (!TxBuff.IsEmpty()) // если есть что в буфере - то передаю
            USART_SendData(USART1, TxBuff.ReadFrom());
        else // видимо текущее прерывание от передачи-последнее
        {
            while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
            {
            } // дожидаюсь завершения выдачи текущего байта и отключаю прерывания от выдачи
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
    USART_ClearITPendingBit(USART1, USART_IT_ORE);
}