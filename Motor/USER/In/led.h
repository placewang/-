////////////////////////////////////////////////////////////////////////////////
/// @file    led.h
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
#ifndef __LED_H
#define __LED_H

// Files includes
#include <string.h>

#include "mm32_device.h"
#include "hal_conf.h"




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

//#define LED1_Port  GPIOA
//#define LED1_Pin   GPIO_Pin_15
#define LED2_Port    GPIOB
#define LED2_Pin     GPIO_Pin_9

#define IN6_Port     GPIOB
#define IN6_Pin      GPIO_Pin_7

#define IN5_Port     GPIOB
#define IN5_Pin      GPIO_Pin_6

#define IN2T3_Port   GPIOB
#define IN2T3_Pin    GPIO_Pin_5

#define IN1T3_Port   GPIOB
#define IN1T3_Pin    GPIO_Pin_4

#define IN4T2_Port   GPIOB
#define IN4T2_Pin    GPIO_Pin_3

#define IN3T2_Port    GPIOA
#define IN3T2_Pin     GPIO_Pin_15

#define MotorEN_Port  GPIOA
#define MotorEN_Pin   GPIO_Pin_3

#define MotorStep_Port  GPIOB
#define MotorStep_Pin   GPIO_Pin_1


#define MotorDir_Port  GPIOB
#define MotorDir_Pin   GPIO_Pin_0

//#define LED1_ON()  GPIO_ResetBits(LED1_Port,LED1_Pin)
//#define LED1_OFF()  GPIO_SetBits(LED1_Port,LED1_Pin)
//#define LED1_TOGGLE()  (GPIO_ReadOutputDataBit(LED1_Port,LED1_Pin))?(GPIO_ResetBits(LED1_Port,LED1_Pin)):(GPIO_SetBits(LED1_Port,LED1_Pin))



#define LED2_ON()  GPIO_ResetBits(LED2_Port,LED2_Pin)
#define LED2_OFF()  GPIO_SetBits(LED2_Port,LED2_Pin)
#define LED2_TOGGLE()  (GPIO_ReadOutputDataBit(LED2_Port,LED2_Pin))?(GPIO_ResetBits(LED2_Port,LED2_Pin)):(GPIO_SetBits(LED2_Port,LED2_Pin))

#define MotorEN_ON()  GPIO_ResetBits(MotorEN_Port,MotorEN_Pin)
#define MotorEN_OFF() GPIO_SetBits(MotorEN_Port,MotorEN_Pin)

#define MotorStep_ON()      GPIO_ResetBits(MotorStep_Port,MotorStep_Pin)
#define MotorStep_OFF()			GPIO_SetBits(MotorStep_Port,MotorStep_Pin)
#define MotorStep_TOGGLE()  (GPIO_ReadOutputDataBit(MotorStep_Port,MotorStep_Pin))?(GPIO_ResetBits(MotorStep_Port,MotorStep_Pin)):(GPIO_SetBits(MotorStep_Port,MotorStep_Pin))

#define MotorDir_ON()       GPIO_ResetBits(MotorDir_Port,MotorDir_Pin)
#define MotorDir_OFF()			GPIO_SetBits(MotorDir_Port,MotorDir_Pin)
#define MotorDir_TOGGLE()   (GPIO_ReadOutputDataBit(MotorDir_Port,MotorDir_Pin))?(GPIO_ResetBits(MotorDir_Port,MotorDir_Pin)):(GPIO_SetBits(MotorDir_Port,MotorDir_Pin))






#define SK_IN1  GPIO_ReadInputDataBit(IN1T3_Port,IN1T3_Pin)
#define SK_IN2  GPIO_ReadInputDataBit(IN2T3_Port,IN2T3_Pin)
#define SK_IN3  GPIO_ReadInputDataBit(IN3T2_Port,IN3T2_Pin)
#define SK_IN4  GPIO_ReadInputDataBit(IN4T2_Port,IN4T2_Pin)
#define SK_IN5  GPIO_ReadInputDataBit(IN5_Port,IN5_Pin)
#define SK_IN6  GPIO_ReadInputDataBit(IN6_Port,IN5_Pin)

//#define LED3_ON()  GPIO_ResetBits(LED3_Port,LED3_Pin)
//#define LED3_OFF()  GPIO_SetBits(LED3_Port,LED3_Pin)
//#define LED3_TOGGLE()  (GPIO_ReadOutputDataBit(LED3_Port,LED3_Pin))?(GPIO_ResetBits(LED3_Port,LED3_Pin)):(GPIO_SetBits(LED3_Port,LED3_Pin))


//#define LED4_ON()  GPIO_ResetBits(LED4_Port,LED4_Pin)
//#define LED4_OFF()  GPIO_SetBits(LED4_Port,LED4_Pin)
//#define LED4_TOGGLE()  (GPIO_ReadOutputDataBit(LED4_Port,LED4_Pin))?(GPIO_ResetBits(LED4_Port,LED4_Pin)):(GPIO_SetBits(LED4_Port,LED4_Pin))

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Enumeration
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief XXXX enumerate definition.
/// @anchor XXXX
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    LED1,
    LED2,
    LED3,
    LED4
} Led_TypeDef;


/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Variables
/// @{
#ifdef _LED_C_
#define GLOBAL







#else
#define GLOBAL extern







#endif





#undef GLOBAL

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Functions
/// @{




void LED_Init(void);
void Motor_StepDir_GPIO_init(void);

/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
