
// Define to prevent recursive inclusion
#define _MAIN_C_

#include "delay.h"

#include "led.h"
#include "uart.h"
#include "tim3_pwm_input.h"
#include "spi.h"
#include "can.h"

u8 SPI_sencallk[5]={0};

CanTxMsg McanPeliTxBuff={TX_CANID, CAN_ID_STD, CAN_DATA_FRAME, 8, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
int main()
{
	  int i;
		DELAY_Init();
		LED_Init();
		UART1_ITRX_InIt(115200);
		TIM3_PWM_Input_Test();
	 // TIM2_Init();
		SPI_1_32bit_Init(SPI,32);
   //	  sendData(SPI_sencallk,0x81, 0x01);
		CAN_Filter_20GroupInit(CAN_1M);
		//Motor_StepDir_GPIO_init();
		while(1)
		{
				if(gRxFlag)
			 {
          gRxFlag = 0;
          printf("CANID:0x%x  Data", gCanPeliRxBuff.ID);
          for(i = 0; i < 8; i++) 
					{
							// Print CAN message contents.
							printf("%x", gCanPeliRxBuff.Data[i]);
							McanPeliTxBuff.Data[i]=gCanPeliRxBuff.Data[i];
          }
					printf("\r\n");
					Send_CANFrame(&McanPeliTxBuff);
				}
				//DELAY_Ms(1000);
		}
}

//u8 SPI_send_data1[5]={0x6c,0x00,0x01,0x00,0xc3};
//u8 SPI_send_data2[5]={0x10,0x00,0x06,0x0f,0x0a};
//u8 SPI_send_data3[5]={0x11,0x00,0x00,0x00,0x0a};
//u8 SPI_send_data4[5]={0x00,0x00,0x00,0x00,0x04};
//u8 SPI_send_data5[5]={0x13,0x00,0x00,0x01,0xf4};
//				for(i=0;i<5;i++)
//				{
//						printf("%x ",SPI_sencallk[i]);
//				}
//				printf("\n");


//		SPI1_write(SPI_sencallk,SPI_send_data1,sizeof(SPI_send_data1));
//		SPI1_write(SPI_sencallk,SPI_send_data2,sizeof(SPI_send_data2));
//		SPI1_write(SPI_sencallk,SPI_send_data3,sizeof(SPI_send_data3));
//		SPI1_write(SPI_sencallk,SPI_send_data4,sizeof(SPI_send_data4));
//		SPI1_write(SPI_sencallk,SPI_send_data5,sizeof(SPI_send_data5));