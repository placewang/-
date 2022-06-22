////////////////////////////////////////////////////////////////////////////////
/// @file    spi.c
/// @author  AE TEAM
/// @brief    In window comparator mode,The transformation results are detected
///           Set the threshold value from 0 to 3V, and connect PB6 and PA0 with
///           jumper cap to see the effect.
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
#define _SPI_C_

// Files includes
#include "delay.h"
#include "uart.h"
#include "spi.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup SPI
/// @{

void myspi_del(volatile u8 delay)
{
	while(--delay){ ; }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reset internal NSS pins for selected SPI software
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_CS_Low()
{
    //Spi cs assign to this pin,select
		SPI_CSInternalSelected(SPI, ENABLE);

}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reset internal NSS pins for selected SPI software
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_CS_High()
{
    //Spi cs release
	  SPI_CSInternalSelected(SPI, DISABLE);

}

////////////////////////////////////////////////////////////////////////////////
/// @brief  initialize SPI2 MODE1(master)
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void SPI2_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_PinAFConfig(SPI_MOSI_PORT, SPI_MOSI_AFSOURCE, SPI_MOSI_AFMODE);
    GPIO_PinAFConfig(SPI_NSS_PORT, SPI_NSS_AFSOURCE, SPI_NSS_AFMODE);
    GPIO_PinAFConfig(SPI_MISO_PORT, SPI_MISO_AFSOURCE, SPI_MISO_AFMODE);
    GPIO_PinAFConfig(SPI_SCK_PORT, SPI_SCK_AFSOURCE, SPI_SCK_AFMODE);
    W25xx_CS_High();
//	W25xx_CS_Low();
    //spi2_cs  pa4
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = SPI_NSS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
    GPIO_Init(SPI_NSS_PORT, &GPIO_InitStructure);
    //spi2_sck  pa4
    GPIO_InitStructure.GPIO_Pin  = SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);
    //spi2_mosi  pa5
    GPIO_InitStructure.GPIO_Pin  = SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
    //spi2_miso  pa6
    GPIO_InitStructure.GPIO_Pin  = SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

}
////////////////////////////////////////////////////////////////////////////////
/// @brief  Modifiable parameter initialization SPI.
/// @note   None.
/// @param  SPIx:SPI1/SPI2.
/// @param  spi_baud_div:Specifies the Baud Rate prescaler value which will be.
/// @param  datawidth:data byte length.
/// @param  mode    :SPI mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
static void SPI2_Config(SPI_TypeDef* SPIx, u16 spi_baud_div)
{

     SPI_InitTypeDef SPI_InitStructure;
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = (SPI_BaudRatePrescaler_TypeDef)spi_baud_div;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
		
    SPI_Cmd(SPIx, ENABLE);
}
////////////////////////////////////////////////////////////////////////////////
/// @brief  Modifiable parameter initialization SPI.
/// @note   None.
/// @param  SPIx:SPI1/SPI2.
/// @param  spi_baud_div:Specifies the Baud Rate prescaler value which will be.
/// @param  datawidth:data byte length.
/// @param  mode    :SPI mode.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_1_32bit_Init(SPI_TypeDef* SPIx, u16 spi_baud_div)
{
    SPI2_GPIO_Config();
    SPI2_Config(SPIx, spi_baud_div);
}



u32 spi_read_write(u8* byte) 
{ 
     SPI_SendData(SPI, *byte);
	
     while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXAVL) == 0){;}
             
     return SPI_ReceiveData(SPI1); 
}


bool SPI1_write(u8* sendcallbuff, u8* data, u8 size_count)
{
	u8 i=0;
	data[0]=data[0]|0x80;
	W25xx_CS_Low();
	while(size_count>i)
	{
			*sendcallbuff=spi_read_write(data);
			sendcallbuff++;
			data++;
			i++;
	}
 	W25xx_CS_High();
	for(i=0;i<5;i++) 
  { } 
	return true;
}

bool SPI1_read(u8* rebuff_data, u8* data,u8 size_cout)
{
	int size_rev=0;
	int i=0;
	W25xx_CS_Low();	
	while(size_cout>size_rev)
	{
			*rebuff_data= spi_read_write(data);
			data++;
			rebuff_data++;
			size_rev++;
	}
	W25xx_CS_High();
	for(i=0;i<5;i++){;}
	return true;
}


void sendData(u8* rebuff_data,unsigned long address,long datagram)
{

	u8	cmd[5];
	u8  i=0;
	cmd[0]=address;
	cmd[1]=(uint8_t)(datagram >> 24);
	cmd[2]=(uint8_t)(datagram >> 16);
	cmd[3]=(uint8_t)(datagram >> 8);
	cmd[4]=(uint8_t)(datagram);
	W25xx_CS_Low();
	for(i=0;i<sizeof(cmd);i++){
		SPI_SendData(SPI,cmd[i]);
		 while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXAVL) == 0){;}
     rebuff_data[i]=SPI_ReceiveData(SPI1);
	}
	for(i=0;i<5;i++)
	{
		printf("%x ",rebuff_data[i]);
	}
	printf("\n");
	W25xx_CS_High();
	for(i=0;i<5;i++){;}
	
}
void tmc5160_init(u8* rebuff_data)
{
	sendData(rebuff_data,0xEC,0x000100C3); 				//PAGE46:CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadcycle)
//	sendData(rebuff_data,0xED,0x0100E313); 				//PAGE48:
//	sendData(rebuff_data,0x8A,0x00180710); 				//PAGE30:死区时间10：200nS，BBMCLKS：7，驱动电流19、18BIT：10，滤波时间21、20BIT：01：200nS
	//sendData(0x90,0x00061f0a);
	sendData(rebuff_data,0x90,0x00061406); 				//PAGE33:IHOLD_IRUN: IHOLD=06(默认10), IRUN=20 (31max.current), IHOLDDELAY=6  
	sendData(rebuff_data,0x91,0x0000000A);				//PAGE33:TPOWERDOWN=10:电机静止到电流减小之间的延时
	
	sendData(rebuff_data,0x80,0x00000004);				//PAGE27:EN_PWM_MODE=1，使能STEALTHCHOP
//	sendData(rebuff_data,0xF0,0x000C0000);				//PAGE43:PWMCONF--STEALTHCHOP:
	sendData(rebuff_data,0x93,0x000001F4);				//PAGE33:TPWM_THRS=500,对应切换速度35000=ca.30RPM
	
	sendData(rebuff_data,0xA4,10000);     				//PAGE35:A1=1000 第一阶段加速度
	sendData(rebuff_data,0xA5,500000);     				//PAGE35:V1=50000加速度阀值速度V1
	sendData(rebuff_data,0xA6,5000);     					//PAGE35:AMAX=500大于V1的加速度        
	sendData(rebuff_data,0xA7,20000000);     			//PAGE35:VMAX=200000       
	sendData(rebuff_data,0xA8,7000);							//PAGE35:DMAX=700大于V1的减速度 
	sendData(rebuff_data,0xAA,14000);     				//PAGE35:D1=1400小于V1的减速度 
	sendData(rebuff_data,0xAB,100);     					//PAGE35:VSTOP=10停止速度，接近于0		
	sendData(rebuff_data,0xA0,0); 								//PAGE35:RAMPMODE=0位置模式，使用所有A、V、D参数	

}




