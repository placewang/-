////////////////////////////////////////////////////////////////////////////////
/// @file     NRF24L01.H
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE NRF24L01
///           EXAMPLES.
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
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

#ifndef __NRF24L01_H
#define __NRF24L01_H



#include "hal_spi.h"


//������Ӳ������
#define NRF_SPI         emFILE_SPI2


//�������û����õ�ѡ��

#define DATA_PACKET             32      //һ�δ�������֧�ֵ��ֽ�����1~32��
#define RX_FIFO_PACKET_NUM      80      //���� FIFO �� �� ��Ŀ ( �ܿռ� ����Ҫ���� һ��ͼ��Ĵ�С������ û�������� )
#define ADR_WIDTH               5       //�����ַ���ȣ�3~5��
#define IS_CRC16                1       //1��ʾʹ�� CRC16��0��ʾ ʹ��CRC8 (0~1)

// �û����� ���ͺ� ���յ�ַ��Ƶ��



#define CHANAL          40                              //Ƶ��ѡ��



//NRF24L01+״̬
typedef enum
{
    NOT_INIT = 0,
    TX_MODE,
    RX_MODE,
} nrf_mode_e;

typedef enum
{
    QUEUE_EMPTY = 0,        //���п�ģʽ��ֻ�������
    QUEUE_NORMAL,           //����ģʽ��������������У������в��ղ���
    QUEUE_FULL,             //������ģʽ������������ӣ�����������
} nrf_rx_queueflag_e; //�жϽ���ʱ������״̬���λ

typedef enum
{
    NRF_TXING,              //������
    NRF_TX_ERROR,           //���ʹ���
    NRF_TX_OK,              //�������
} nrf_tx_state_e;


//gpio����CE��IRQ
#define NRF_CE_HIGH()       GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define NRF_CE_LOW()        GPIO_ResetBits(GPIOB, GPIO_Pin_9)

// �ڲ����ò���
#define TX_ADR_WIDTH    ADR_WIDTH                       //�����ַ���
#define TX_PLOAD_WIDTH  DATA_PACKET                     //��������ͨ����Ч���ݿ��0~32Byte

#define RX_ADR_WIDTH    ADR_WIDTH                       //���յ�ַ���
#define RX_PLOAD_WIDTH  DATA_PACKET                     //��������ͨ����Ч���ݿ��0~32Byte

/******************************** NRF24L01+ �Ĵ������� �궨��***************************************/

// SPI(nRF24L01) commands , NRF��SPI����궨�壬���NRF����ʹ���ĵ�
#define NRF_READ_REG    0x00    // Define read command to register
#define NRF_WRITE_REG   0x20    // Define write command to register
#define RD_RX_PLOAD     0x61    // Define RX payload register address
#define WR_TX_PLOAD     0xA0    // Define TX payload register address
#define FLUSH_TX        0xE1    // Define flush TX register command
#define FLUSH_RX        0xE2    // Define flush RX register command
#define REUSE_TX_PL     0xE3    // Define reuse TX payload register command
#define NOP             0xFF    // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses) ��NRF24L01 ��ؼĴ�����ַ�ĺ궨��
#define CONFIG      0x00        // 'Config' register address
#define EN_AA       0x01        // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02        // 'Enabled RX addresses' register address
#define SETUP_AW    0x03        // 'Setup address width' register address
#define SETUP_RETR  0x04        // 'Setup Auto. Retrans' register address
#define RF_CH       0x05        // 'RF channel' register address
#define RF_SETUP    0x06        // 'RF setup' register address
#define STATUS      0x07        // 'Status' register address
#define OBSERVE_TX  0x08        // 'Observe TX' register address
#define CD          0x09        // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A        // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B        // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C        // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D        // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E        // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F        // 'RX address pipe5' register address
#define TX_ADDR     0x10        // 'TX address' register address
#define RX_PW_P0    0x11        // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12        // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13        // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14        // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15        // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16        // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17        // 'FIFO Status Register' register address


//������Ҫ��״̬���
#define TX_FULL     0x01        //TX FIFO �Ĵ�������־�� 1 Ϊ ����0Ϊ ����
#define MAX_RT      0x10        //�ﵽ����ط������жϱ�־λ
#define TX_DS       0x20        //��������жϱ�־λ
#define RX_DR       0x40        //���յ������жϱ�־λ


#ifdef _NRF24L01_C_
#define GLOBAL
u8 TX_ADDRESS[5] = {0xFF, 0xFF, 0x00, 0xFF, 0xFF};   // ����һ����̬���͵�ַ
u8 RX_ADDRESS[5] = {0xFF, 0xFF, 0x00, 0xFF, 0xFF};

//u8 TX_ADDRESS[5] = {0x34, 0x43, 0x10, 0x10, 0x01};   // ����һ����̬���͵�ַ
//u8 RX_ADDRESS[5] = {0x34, 0x43, 0x10, 0x10, 0x01};
#else
#define GLOBAL extern

#endif

GLOBAL HANDLE hSPI;

GLOBAL u8 nrf_mode;
GLOBAL volatile u8  nrf_rx_front, nrf_rx_rear;              //����FIFO��ָ��
GLOBAL volatile u8 nrf_rx_flag;

GLOBAL u8 NRF_ISR_RX_FIFO[RX_FIFO_PACKET_NUM][DATA_PACKET];         //�жϽ��յ�FIFO

GLOBAL volatile u8 *nrf_irq_tx_addr;
GLOBAL volatile u32 nrf_irq_tx_pnum;                    //pnum = (len+MAX_ONCE_TX_NUM -1)  / MAX_ONCE_TX_NUM

GLOBAL u8 nrf_irq_tx_flag;                     //0 ��ʾ�ɹ� ��1 ��ʾ ����ʧ��



#undef GLOBAL

extern u8 nrf_init();
extern void nrf_config();
extern u8 nrf_writeReg(u8 reg, u8 dat);
extern u8 nrf_readReg(u8 reg, u8 *pDat);
extern u8 nrf_readBuf(u8 reg, u8 *pBuf, u16 len);
extern u8 nrf_writeBuf(u8 reg , u8 *pBuf, u16 len);
extern u8 nrf_write(SPI_TypeDef* SPIx, u8 reg, u8 *pDat, u16 len);
extern u8 nrf_read(SPI_TypeDef* SPIx, u8 reg, u8* pDat, u16 len);
extern void nrf_rx_mode(void);
extern void nrf_tx_mode(void);
extern u16  nrf_rx(u8 *rxbuf, u16 len);
extern u8 nrf_tx(u8 *txbuf, u8 len);
extern nrf_tx_state_e nrf_tx_state();
extern u8 nrf_link_check(void);
extern void nrf_handler(void);






#endif      //__NRF24L01_H
