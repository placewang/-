////////////////////////////////////////////////////////////////////////////////
/// @file    uart.h
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FIRMWARE FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion
#ifndef __UART_H
#define __UART_H
// Files includes
#include "mm32_device.h"
#include  "stdio.h"


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Example_Layer
/// @brief MM32 Example Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_RESOURCE
/// @brief MM32 Examples resource modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Constants
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Variables
/// @{
//#ifdef _UART_C_
//#define GLOBAL


//#else
//#define GLOBAL extern






//#endif




//#undef GLOBAL

///// @}


//////////////////////////////////////////////////////////////////////////////////
///// @defgroup MM32_Exported_Functions
///// @{
#define RXBUFFU    100



typedef struct UART1_ITREAD_QUEUE_stru
{
	char rev_buffu[RXBUFFU];           //缓存区
	u16 out_count;                     //出队下标           
	u16 in_count;								       //入队下标
}UART1_ITREAD_QUEUE_BUFFU;;

extern char CMD_buff[RXBUFFU];
extern UART1_ITREAD_QUEUE_BUFFU  uartrevQ;


void CONSOLE_Init(u32 baudrate);
void UART1_ITRX_InIt(u32 baudrate);
bool Cmd_pack( char* );
void Queue_init(void);
bool Cmd_Exe(char *,char *);
bool uart_status(void);

/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////


