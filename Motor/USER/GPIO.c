////////////////////////////////////////////////////////////////////////////////
/// @file    led.c
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////


// Define to prevent recursive inclusion
#define _LED_C_

// Files includes

#include "led.h"
////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup LED
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup LED_Exported_Constants
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @brief  initialize LED GPIO pin
/// @note   if use jtag/swd interface GPIO PIN as LED, need to be careful,
///         can not debug or program.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  GPIO_InitStructure.GPIO_Pin  =  MotorEN_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    LED2_OFF();
  	MotorEN_OFF();
    //MotorEN_ON();

	
	
}
//电机脉冲与方向
void Motor_StepDir_GPIO_init(void)
{
			GPIO_InitTypeDef  GPIO_InitStructure;

			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
			GPIO_StructInit(&GPIO_InitStructure);
			GPIO_InitStructure.GPIO_Pin  = MotorStep_Pin |MotorDir_Pin;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
}



void Input_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOA, ENABLE);
	
	  GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA ,&GPIO_InitStructure);
}
/// @}

/// @}

/// @}
