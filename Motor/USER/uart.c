
#define _UART_C_

// Files includes

#include <stdio.h>
#include "uart.h"
#include "hal_conf.h"
#include "string.h"



#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE s32 __io_putchar(s32 ch)
#else
#define PUTCHAR_PROTOTYPE s32 fputc(s32 ch, FILE *f)

#endif

#ifdef USE_IAR
PUTCHAR_PROTOTYPE {
    while((UART1->CSR & UART_IT_TXIEN) == 0); //The loop is sent until it is finished
    UART1->TDR = (ch & (u16)0x00FF);
    return ch;
}
f
#else
void _sys_exit(s32 x)
{
    x = x;
}
//redefine fputcfunction
s32 fputc(s32 ch, FILE* f)
{
    while((UART1->CSR & UART_IT_TXIEN) == 0); //The loop is sent until it is finished
    UART1->TDR = (ch & (u16)0x00FF);
    return ch;
}

#endif





//数据接受队列结构体
UART1_ITREAD_QUEUE_BUFFU  uartrevQ;
////执行命提取
char CMD_buff[RXBUFFU];

void UART1_ITRX_InIt(u32 baudrate)
{
    //GPIO port set
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;
		//中断
		NVIC_InitTypeDef NVIC_InitStructure;
		//UART1 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
		RCC_APB2PeriphClockCmd(RCC_APB2ENR_UART1, ENABLE);   //enableUART1,GPIOAclock
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);  //
    //UART initialset
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
    UART_StructInit(&UART_InitStructure);
    UART_InitStructure.UART_BaudRate = baudrate;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;//one stopbit
    UART_InitStructure.UART_Parity = UART_Parity_No;//none odd-even  verify bit
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//No hardware flow control
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx; // receive and sent  mode

    UART_Init(UART1, &UART_InitStructure);       //initial uart 1
		UART_ITConfig(UART1,UART_IT_RXIEN, ENABLE);  //read IT set
		UART_Cmd(UART1, ENABLE);                     //enable uart 1

    //UART1_TX   GPIOA.9
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //UART1_RX    GPIOA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		Queue_init();
}

//read queue struct init
void Queue_init(void)
{		
		uartrevQ.in_count=0;
		uartrevQ.out_count=0;
	  memset(CMD_buff,'\0',RXBUFFU);
		memset(uartrevQ.rev_buffu,'\0',RXBUFFU);
}
u8 Size_buff(char * q)
{
		int i=0;
		int count=0;
		for(i=0;i<sizeof(q);i++)
		{
			if(q[i]!='\0')
			{
				count++;
			}
		}
	return count;
}
//接收中断是否完成对数据的入队出队
bool uart_status(void)
{
	if(!UART_GetITStatus(UART1, UART_ISR_RX) &&!UART_GetFlagStatus(UART1,UART_FLAG_RXAVL))
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

bool Cmd_pack( char* cmd)
{
		//有效字符计数
		static u8 count;
		while(1)
		{
				if(uartrevQ.out_count==0)
				{
						cmd[uartrevQ.out_count]=uartrevQ.rev_buffu[uartrevQ.out_count];
				}
				else
				{
							if(uartrevQ.out_count==RXBUFFU)
							{
								uartrevQ.out_count=0;
							}
							if(count>=50)
							{
									memset(CMD_buff,'\0',RXBUFFU);
									count=0;
									return false;
							}
							else if(uartrevQ.rev_buffu[uartrevQ.out_count]=='\0')
							{
									//Queue_init();
									//count=0;
									return false;
							}
							else if(uartrevQ.rev_buffu[uartrevQ.out_count]=='\n')
							{
								cmd[uartrevQ.out_count]=uartrevQ.rev_buffu[uartrevQ.out_count];
								uartrevQ.out_count++;
								memset(uartrevQ.rev_buffu,'\0',(uartrevQ.out_count));
								count=0;
								return true;
							}
							cmd[uartrevQ.out_count]=uartrevQ.rev_buffu[uartrevQ.out_count];	
				}

				uartrevQ.out_count++;
				memset(uartrevQ.rev_buffu,'\0',uartrevQ.out_count);
				count++;
			}
}
//取一个指令出来
bool Cmd_Exe(char * cmd,char *exe)
{
//		static u8  i;
		static u8 count1;
		static u8 count2;
		if(count1==RXBUFFU)
		{
				count1=0;
		}

		if(cmd[count1]!='\0'&&cmd[count1]!='\n')
		{
			exe[count2]=cmd[count1];
		}
		else if(cmd[count1]=='\0')
		{
			memset(cmd,'\0',count1+1);
			return false;
		}
		else if(cmd[count1]=='\n')
		{
			exe[count2]='\0';
			count2=0;
			//指向下一位
			count1++;
			memset(cmd,'\0',count1);
			return true;
		}
		memset(cmd,'\0',count1+1);
		count1++;
		count2++;
	return false;
}


//uart1 中断服务函数

void UART1_IRQHandler(void)
{
    if (UART_GetITStatus(UART1, UART_ISR_RX) != RESET) 
		{
				 UART_ClearITPendingBit(UART1, UART_ISR_RX);
				if(uartrevQ.in_count==RXBUFFU)
				{
							uartrevQ.in_count=0;
				}
				while(UART_GetFlagStatus(UART1,UART_FLAG_RXAVL))
				{
					if(uartrevQ.in_count==RXBUFFU)
					{
							uartrevQ.in_count=0;
					}
					uartrevQ.rev_buffu[uartrevQ.in_count]=UART_ReceiveData(UART1);
					uartrevQ.in_count++;
				  Cmd_pack(CMD_buff);
				}
		}
}


/// @}

/// @}

/// @}
