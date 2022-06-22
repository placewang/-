/* This is MM32SPIN06x spi coder file.*/

#include <stdio.h>


//#include "platform_config.h"
#include "hal_conf.h"

#define SPI  SPI1

#define SPI_MOSI_BUSCLK                  RCC_AHBPeriph_GPIOA
#define SPI_MOSI_PIN                     GPIO_Pin_7
#define SPI_MOSI_PORT                    GPIOA
#define SPI_MOSI_AFSOURCE                GPIO_PinSource7
#define SPI_MOSI_AFMODE                  GPIO_AF_0

#define SPI_NSS_BUSCLK                  RCC_AHBPeriph_GPIOA
#define SPI_NSS_PIN                     GPIO_Pin_4
#define SPI_NSS_PORT                    GPIOA
#define SPI_NSS_AFSOURCE                GPIO_PinSource4
#define SPI_NSS_AFMODE                  GPIO_AF_0

#define SPI_MISO_BUSCLK                  RCC_AHBPeriph_GPIOA
#define SPI_MISO_PIN                     GPIO_Pin_6
#define SPI_MISO_PORT                    GPIOA
#define SPI_MISO_AFSOURCE                GPIO_PinSource6
#define SPI_MISO_AFMODE                  GPIO_AF_0

#define SPI_SCK_BUSCLK                  RCC_AHBPeriph_GPIOA
#define SPI_SCK_PIN                     GPIO_Pin_5
#define SPI_SCK_PORT                    GPIOA
#define SPI_SCK_AFSOURCE                GPIO_PinSource5
#define SPI_SCK_AFMODE                  GPIO_AF_0






//#define SPI1_CS_ENABLE(x)	((x!=0) ? (GPIO_SetBits(GPIOA,GPIO_Pin_4)):(GPIO_ResetBits(GPIOA,GPIO_Pin_4)))
//#define SPI1_SCLK(x)			((x!=0) ? (GPIO_SetBits(GPIOA,GPIO_Pin_5)):(GPIO_ResetBits(GPIOA,GPIO_Pin_5)))
//#define SPI1_SCLK_N(x)			((x==0) ? (GPIO_SetBits(GPIOA,GPIO_Pin_5)):(GPIO_ResetBits(GPIOA,GPIO_Pin_5)))

//#define SPI1_READ_DATA()  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
//#define SPI1_WRITE_DATA(x)((x!=0) ? (GPIO_SetBits(GPIOA,GPIO_Pin_7)):(GPIO_ResetBits(GPIOA,GPIO_Pin_7)))

//#define SPI2_CS_ENABLE(x)	((x!=0) ? (GPIO_SetBits(GPIOB,GPIO_Pin_12)):(GPIO_ResetBits(GPIOB,GPIO_Pin_12)))
//#define SPI2_SCLK(x)			((x!=0) ? (GPIO_SetBits(GPIOB,GPIO_Pin_13)):(GPIO_ResetBits(GPIOB,GPIO_Pin_13)))
//#define SPI2_READ_DATA()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
//#define SPI2_WRITE_DATA(x)  ((x!=0) ? (GPIO_SetBits(GPIOB,GPIO_Pin_15)):(GPIO_ResetBits(GPIOB,GPIO_Pin_15)))


volatile unsigned char SSI_IDX2LOG[4] = {0,1,2,3};
volatile unsigned char SSI_IDX2CS[4] = {0,1,3,2};

#define SPI_DATA_LENGTH_16	
#define SPI_ISR



void wait_ms(int);
void SPI1_CS_High()
{
    //Spi cs release
    SPI_CSInternalSelected(SPI, DISABLE);
}

void SPI1_CS_Low()
{
    //Spi cs Enable
    SPI_CSInternalSelected(SPI, ENABLE);
}
static void SPI1_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_PinAFConfig(SPI_MOSI_PORT, SPI_MOSI_AFSOURCE, SPI_MOSI_AFMODE);
    GPIO_PinAFConfig(SPI_NSS_PORT, SPI_NSS_AFSOURCE, SPI_NSS_AFMODE);
    GPIO_PinAFConfig(SPI_MISO_PORT, SPI_MISO_AFSOURCE, SPI_MISO_AFMODE);
    GPIO_PinAFConfig(SPI_SCK_PORT, SPI_SCK_AFSOURCE, SPI_SCK_AFMODE);
    SPI1_CS_High();
    //spi2_cs  pb12
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = SPI_NSS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_NSS_PORT, &GPIO_InitStructure);
    //spi2_sck  pb13
    GPIO_InitStructure.GPIO_Pin  = SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);
    //spi2_mosi  pb15
    GPIO_InitStructure.GPIO_Pin  = SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
    //spi2_miso  pb14
    GPIO_InitStructure.GPIO_Pin  = SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

}

static void SPI1_Config(SPI_TypeDef* SPIx, u16 spi_baud_div)
{

    SPI_InitTypeDef SPI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_DataWidth = SPI_DataWidth_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = (SPI_BaudRatePrescaler_TypeDef)spi_baud_div;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Rx);
    SPI_BiDirectionalLineConfig(SPIx, SPI_Direction_Tx);
    SPI_Cmd(SPIx, ENABLE);
}


void SPI_css_out(unsigned char spino,unsigned char isenable)
{
	if(isenable)
	{
		SPI1_CS_Low();
		return;
	}	

	SPI1_CS_High();

}

void SPI_init()
{

    SPI1_GPIO_Config();
    SPI1_Config(SPI, SPI_BaudRatePrescaler_256);

}

uint32_t spi_read_write(uint8_t byte) 
{ 
    SPI_SendData(SPI1, byte); 
      
      while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXAVL) == 0); 
             
     return SPI_ReceiveData(SPI1); 
}


//int SPI_Transmit_Data(unsigned char spi_no,unsigned char *data_send,unsigned int timeout)
//{
//	SPI_TypeDef* SPIx ;	
//	unsigned char istimeout=0;
//	unsigned int timeoutdata = timeout;
//	unsigned int firstdata;

//	if(!data_send) return -1;
//	
//	//firstdata = Time_get_first_time();
//	
//	
//	SPIx = SPI;

//	do
//	{
//		if(SPI_GetFlagStatus(SPIx,SPI_FLAG_TXEPT)!=RESET)
//		{
//			SPI_SendData(SPIx,*data_send);
//			istimeout =0;
//			break;
//		}
//		istimeout++;
//	}
//	while(istimeout<timeoutdata);

//	if(istimeout) return 1;

//	do
//	{			
//		if(SPI_GetFlagStatus(SPIx,SPI_FLAG_RXAVL)!=RESET)
//		{
//			*data_send = SPI_ReceiveData(SPIx);
//			istimeout =0;
//			break;
//		}		
//		istimeout ++;//= Time_check_timeout(&firstdata, &timeoutdata);
//	}
//	while(istimeout<timeoutdata);

//	if(istimeout) return 2;

//	return 0;
//	
//}


//int SPI_Read_Data_5Bytes(unsigned char whichspi,unsigned char regaddr,unsigned char *Getdata)
//{
//	int i,ret;
//	int ret_ret=0;
//	unsigned char datasend[5];
//	SPI_TypeDef* SPIx ;
//	if(!Getdata) return -1;
//	if(whichspi>1) return -2;

//	SPI_css_out(whichspi,1);

//	datasend[0]=regaddr & 0x7F;  /*确保最高位为0*/
//	datasend[1]=0;
//	datasend[2]=0;
//	datasend[3]=0;
//	datasend[4]=0;
//	ret =0;
//	for(i=0;i<5;i++)/*发送5个字节*/
//	{
//		ret = SPI_Transmit_Data(whichspi,&datasend[i],1000);
//		if(ret)/*error*/
//		{
//			break;
//		}
//	}
//	if(ret)
//	{
//		ret_ret = 	ret+100;
//		goto RET_error_return;
//	}	 

//	/*这个关断再开启过程必须要。TMC5160的特性，5个字节连续读写*/
//	SPI_css_out(whichspi,0);
//	delayus();
//	SPI_css_out(whichspi,1);

//	datasend[0]=0x7F;/*确保不是写数据*/
//	
//	for(i=0;i<5;i++)/*读取5个字节*/
//	{
//		ret = SPI_Transmit_Data(whichspi,&datasend[i],1000);
//		if(ret)/*error*/
//		{
//			break;
//		}
//	}
//	if(ret) 
//	{
//		ret_ret= ret+200;
//		goto RET_error_return;
//	}

//	memcpy(Getdata,datasend,5);
//	ret_ret = 0;

//RET_error_return:

//	SPI_css_out(whichspi,0);
//	return ret_ret;
//	
//	
//	
//}




//int SPI_Write_Data_5Bytes(unsigned char whichspi,unsigned char regaddr,unsigned char *Getdata)
//{
//	int i,ret;
//	int ret_ret;
//	unsigned char datasend[5];
//	SPI_TypeDef* SPIx ;
//	if(!Getdata) return -1;
//	if(whichspi>1) return -2;

//	SPI_css_out(whichspi,1);

//	datasend[0]=regaddr | 0x80;  /*确保最高位为1*/
//	memcpy(&datasend[1],Getdata,4);
//	ret =0;
//	for(i=0;i<5;i++)/*发送5个字节*/
//	{
//		ret = SPI_Transmit_Data(whichspi,&datasend[i],1000);
//		if(ret)/*error*/
//		{
//			break;
//		}
//	}
//	if(ret) 
//	{
//		ret_ret= ret+100;
//	}	
//	else
//		ret_ret = 0;
//	memcpy(Getdata,datasend,1);
//	
//	SPI_css_out(whichspi,0);
//	return ret_ret;
//	
//	
//}



//void ssidelay15ns(volatile unsigned int delay)
//{
//	while(--delay){ ; }
//}






//unsigned int Read_CPLD_vis()
//{
//	unsigned int w_add_data=0;
//	
//	return w_add_data;
//}




