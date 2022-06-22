////////////////////////////////////////////////////////////////////////////////
/// @file     NRF24L01.C
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE NRF24L01 EXAMPLE.
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

#define _NRF24L01_C_

#include <string.h>
#include "types.h"
#include "system_mm32.h"

#include "hal_exti.h"
#include "drv.h"
#include "hal_spi.h"
#include "hal_rcc.h"
#include "hal_gpio.h"

#include "bsp.h"
#include "bsp_spi.h"

#include "nrf24l01.h"

void EXTI9_5_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line8)){
        EXTI_ClearITPendingBit(EXTI_Line8);
        nrf_handler();
    }
}

u8 nrf_init()
{
    GPIOB_ClockEnable();
    GPIO_Mode_OUT_PP_Init(GPIOB, GPIO_Pin_9);
    NRF_CE_LOW();

    HANDLE hEXTI = CreateFile(emIP_EXTI);
    if (hEXTI == NULL)      while(1);

    tAPP_EXTI_DCB dcb = {
//      .sync   = emTYPE_Sync,                  // emTYPE_ASync
        .mode   = emEXTI_IT,                    // emEXTI_IT, emEXTI_Event
        .edge   = emEDGE_Falling,               // emEDGE_Rising, emEXTI_Falling, emEDGE_RisingFalling
//      .cb     = NULL,
        .port   = emGPIOB,
        .hSub   = emFILE_EXTI8
    };

    if (!OpenFile(hEXTI, (void*)&dcb))      while(1);


    hSPI = CreateFile(emIP_SPI);
    if (hSPI == NULL)       while(1);


    // Step 3:  Assignment DCB structure    --------------->>>>>
    tAPP_SPI_DCB spidcb = {
// Base parameter
        .hSub               = NRF_SPI,
        .type               = emTYPE_Polling,           // emTYPE_Polling,emTYPE_IT,emTYPE_DMA,
        .block              = emTYPE_Block,         // emTYPE_Blocking, emTYPE_NonBlocking

// Callback function
        .sync               = emTYPE_Sync,          // Sync, ASync
        .cbTx               = NULL,    //tx callback function access address
        .cbRx               = NULL,    //rx callback function access address

// operation mode
        .remapEn            = false,                // Disable : 0 ,Enable : 1
        .remapIdx           = 0,                    // u8 value

// SPI parameter
        .prescaler          = 64,
        .mode               = emSPI_MODE_0,
        .hardNss            = false,
        .firstLsb           = false,
        .master             = true
    };

    if (!OpenFile(hSPI, (void*)&spidcb))        while(1);

    nrf_config();

    return nrf_link_check();


}


void nrf_config()
{
    //����NRF�Ĵ���
    NRF_CE_LOW();

    nrf_writeReg(NRF_WRITE_REG + SETUP_AW, ADR_WIDTH - 2);          //���õ�ַ����Ϊ TX_ADR_WIDTH

    nrf_writeReg(NRF_WRITE_REG + RF_CH, CHANAL);                    //����RFͨ��ΪCHANAL
    nrf_writeReg(NRF_WRITE_REG + RF_SETUP, 0x0f);                   //����TX�������,0db����,2Mbps,���������濪��

    nrf_writeReg(NRF_WRITE_REG + EN_AA, 0x01);                      //ʹ��ͨ��0���Զ�Ӧ��

    nrf_writeReg(NRF_WRITE_REG + EN_RXADDR, 0x01);                  //ʹ��ͨ��0�Ľ��յ�ַ

    //RXģʽ����
    nrf_writeBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ

    nrf_writeReg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);         //ѡ��ͨ��0����Ч���ݿ��

    nrf_writeReg(FLUSH_RX, NOP);                                    //���RX FIFO�Ĵ���

    //TXģʽ����
    nrf_writeBuf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); //дTX�ڵ��ַ

    nrf_writeReg(NRF_WRITE_REG + SETUP_RETR, 0x0F);                 //�����Զ��ط����ʱ��:250us + 86us;����Զ��ط�����:15��

    nrf_writeReg(FLUSH_TX, NOP);                                    //���TX FIFO�Ĵ���

    nrf_rx_mode();                                                  //Ĭ�Ͻ������ģʽ

    NRF_CE_HIGH();

}

u8 nrf_writeReg(u8 reg, u8 dat)
{
    return nrf_write(SPI2, reg, &dat, 1);
}

u8 nrf_readReg(u8 reg, u8 *pDat)
{
    return nrf_read(SPI2, reg, pDat, 1);
}

u8 nrf_writeBuf(u8 reg , u8 *pBuf, u16 len)
{
    return nrf_write(SPI2, reg, pBuf, len);
}

u8 nrf_readBuf(u8 reg, u8 *pBuf, u16 len)
{
    return nrf_read(SPI2, reg, pBuf, len);
}


u8 nrf_write(SPI_TypeDef* SPIx, u8 reg, u8 *pDat, u16 len)
{
    BSP_SPI_NSS_Configure(SPIx, 0, 0, ENABLE);

    WRITE_REG(SPIx->TDR, reg);

    while(!SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL));
    u8 status = READ_REG(SPIx->RDR);

    while(len > 0) {
        if (SPI_GetITStatus(SPIx, SPI_IT_TX)){
            SPI_ClearITPendingBit(SPIx, SPI_IT_TX);
            WRITE_REG(SPIx->TDR, *pDat++);
            len --;
        }
    }
    while(SPI_GetFlagStatus(SPIx, SPI_FLAG_TXEPT) == 0) {
    }


    while(SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL)){
        READ_REG(SPIx->RDR);
    }

    BSP_SPI_NSS_Configure(SPIx, 0, 0, DISABLE);

    return status;
}

u8 nrf_read(SPI_TypeDef* SPIx, u8 reg, u8* pDat, u16 len)
{
    BSP_SPI_NSS_Configure(SPIx, 0, 0, ENABLE);

    WRITE_REG(SPIx->TDR, reg);

    while(!SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL));
    u8 status = READ_REG(SPIx->RDR);

    while(len > 0) {
        WRITE_REG(SPIx->TDR, 0xFF);

        while(!SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL));
        *pDat++ = (u8)READ_REG(SPIx->RDR);
        len --;
    }

    BSP_SPI_NSS_Configure(SPIx, 0, 0, DISABLE);
    return status;
}

void nrf_rx_mode(void)
{
    NRF_CE_LOW();

    nrf_writeReg(NRF_WRITE_REG + EN_AA, 0x01);          //ʹ��ͨ��0���Զ�Ӧ��

    nrf_writeReg(NRF_WRITE_REG + EN_RXADDR, 0x01);      //ʹ��ͨ��0�Ľ��յ�ַ

    nrf_writeBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ


    nrf_writeReg(NRF_WRITE_REG + CONFIG, 0x0B | (IS_CRC16 << 2));       //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ

    /* ����жϱ�־*/
    nrf_writeReg(NRF_WRITE_REG + STATUS, 0xff);

    nrf_writeReg(FLUSH_RX, NOP);                    //���RX FIFO�Ĵ���

    /*CE���ߣ��������ģʽ*/
    NRF_CE_HIGH();

    nrf_mode = RX_MODE;
}

void nrf_tx_mode(void)
{
    volatile u32 i;

    NRF_CE_LOW();

    nrf_writeBuf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); //дTX�ڵ��ַ

    nrf_writeBuf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //����RX�ڵ��ַ ,��ҪΪ��ʹ��ACK

    nrf_writeReg(NRF_WRITE_REG + CONFIG, 0x0A | (IS_CRC16 << 2)); //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�


    /*CE���ߣ����뷢��ģʽ*/
    NRF_CE_HIGH();

    nrf_mode = TX_MODE;

    i = 0x0fff;
    while(i--);         //CEҪ����һ��ʱ��Ž��뷢��ģʽ

}


u8 nrf_link_check(void)
{
#define NRF_CHECH_DATA  0xC2        //��ֵΪУ������ʱʹ�ã����޸�Ϊ����ֵ

    u8 reg;

    u8 txBuff[5] = {NRF_CHECH_DATA, NRF_CHECH_DATA, NRF_CHECH_DATA, NRF_CHECH_DATA, NRF_CHECH_DATA};
    u8 rxBuff[5] = {0};
    u8 i;


    reg = NRF_WRITE_REG + TX_ADDR;
    nrf_write(SPI2, reg, txBuff, 5);//д��У������


    reg = TX_ADDR;
    nrf_read(SPI2, reg, rxBuff, 5);//��ȡУ������



    /*�Ƚ�*/
    for(i = 0; i < 5; i++)
    {
        if(rxBuff[i] != NRF_CHECH_DATA)
        {
            return 0 ;        //MCU��NRF����������
        }
    }
    return 1 ;             //MCU��NRF�ɹ�����
}



u16  nrf_rx(u8 *rxbuf, u16 len)
{
    u32 tmplen = 0;
    u8 tmp;

    while( (nrf_rx_flag != QUEUE_EMPTY) && (len != 0) )
    {
        if(len < DATA_PACKET)
        {
            memcpy(rxbuf, (u8 *)&(NRF_ISR_RX_FIFO[nrf_rx_front]), len);

            NRF_CE_LOW();

            nrf_rx_front++;

            if(nrf_rx_front >= RX_FIFO_PACKET_NUM)
            {
                nrf_rx_front = 0;
            }
            tmp =  nrf_rx_rear;
            if(nrf_rx_front == tmp)
            {
                nrf_rx_flag = QUEUE_EMPTY;
            }
            NRF_CE_HIGH();

            tmplen += len;
            return tmplen;
        }

        memcpy(rxbuf, (u8 *)&(NRF_ISR_RX_FIFO[nrf_rx_front]), DATA_PACKET);
        rxbuf   += DATA_PACKET;
        len     -= DATA_PACKET;
        tmplen  += DATA_PACKET;

        NRF_CE_LOW();

        nrf_rx_front++;

        if(nrf_rx_front >= RX_FIFO_PACKET_NUM)
        {
            nrf_rx_front = 0;
        }
        tmp  = nrf_rx_rear;
        if(nrf_rx_front == tmp)
        {
            nrf_rx_flag = QUEUE_EMPTY;
        }
        else
        {
            nrf_rx_flag = QUEUE_NORMAL;
        }
        NRF_CE_HIGH();
    }

    return tmplen;
}










u8 nrf_tx(u8 *txbuf, u8 len)
{
    nrf_irq_tx_flag = 0;        //��λ��־λ

    if((txbuf == 0 ) || (len == 0))
    {
        return 0;
    }

    if(nrf_irq_tx_addr == 0 )
    {
        //
        nrf_irq_tx_pnum = (len - 1) / DATA_PACKET;        // �� 1 ��� �� ����Ŀ
        nrf_irq_tx_addr = txbuf;

        if( nrf_mode != TX_MODE)
        {
            nrf_tx_mode();
        }

        //��Ҫ �ȷ���һ�����ݰ������ �жϷ���

        /*ceΪ�ͣ��������ģʽ1*/
        NRF_CE_LOW();

        /*д���ݵ�TX BUF ��� 32���ֽ�*/
        nrf_writeBuf(WR_TX_PLOAD, txbuf, DATA_PACKET);

        /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */
        NRF_CE_HIGH();

        return 1;
    }
    else
    {
        return 0;
    }
}


nrf_tx_state_e nrf_tx_state ()
{
    /*
    if(nrf_mode != TX_MODE)
    {
        return NRF_NOT_TX;
    }
    */

    if((nrf_irq_tx_addr == 0) && (nrf_irq_tx_pnum == 0))
    {
        //�������
        if(nrf_irq_tx_flag)
        {
            return NRF_TX_ERROR;
        }
        else
        {
            return NRF_TX_OK;
        }
    }
    else
    {
        return NRF_TXING;
    }
}


void nrf_handler(void)
{

    u8 state;
    u8 tmp;
    /*��ȡstatus�Ĵ�����ֵ  */
    nrf_readReg(STATUS, &state);

    /* ����жϱ�־*/
    nrf_writeReg(NRF_WRITE_REG + STATUS, state);

    if(state & RX_DR) //���յ�����
    {
        NRF_CE_LOW();

        if(nrf_rx_flag != QUEUE_FULL)
        {
            //��û�������������
            //printf("+");
            nrf_readBuf(RD_RX_PLOAD, (u8 *)&(NRF_ISR_RX_FIFO[nrf_rx_rear]), RX_PLOAD_WIDTH); //��ȡ����

            nrf_rx_rear++;

            if(nrf_rx_rear >= RX_FIFO_PACKET_NUM)
            {
                nrf_rx_rear = 0;                            //��ͷ��ʼ
            }
            tmp = nrf_rx_front;
            if(nrf_rx_rear == tmp)                 //׷��ƨ���ˣ�����
            {
                nrf_rx_flag = QUEUE_FULL;
            }
            else
            {
                nrf_rx_flag = QUEUE_NORMAL;
            }
        }
        else
        {
            nrf_writeReg(FLUSH_RX, NOP);                    //���RX FIFO�Ĵ���
        }
        NRF_CE_HIGH();                                      //�������ģʽ
    }

    if(state & TX_DS) //����������
    {
        if(nrf_irq_tx_pnum == 0)
        {
            nrf_irq_tx_addr = 0;

            // ע��: nrf_irq_tx_pnum == 0 ��ʾ ���� �Ѿ�ȫ�����͵�FIFO �� nrf_irq_tx_addr == 0 ���� ȫ����������

            //������ɺ� Ĭ�� ���� ����ģʽ
            if( nrf_mode != RX_MODE)
            {
                nrf_rx_mode();
            }
        }
        else
        {
            if( nrf_mode != TX_MODE)
            {
                nrf_tx_mode();
            }

            //��û������ɣ��ͼ�������
            nrf_irq_tx_addr += DATA_PACKET;    //ָ����һ����ַ
            nrf_irq_tx_pnum --;                 //����Ŀ����

            /*ceΪ�ͣ��������ģʽ1*/
            NRF_CE_LOW();

            /*д���ݵ�TX BUF ��� 32���ֽ�*/
            nrf_writeBuf(WR_TX_PLOAD, (u8 *)nrf_irq_tx_addr, DATA_PACKET);

            /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */
            NRF_CE_HIGH();
        }
    }

    if(state & MAX_RT)      //���ͳ�ʱ
    {
        nrf_irq_tx_flag = 1;                            //��Ƿ���ʧ��
        nrf_writeReg(FLUSH_TX, NOP);                    //���TX FIFO�Ĵ���


        //�п����� �Է�Ҳ���� ����״̬

        //�������η���
        nrf_irq_tx_addr = 0;
        nrf_irq_tx_pnum = 0;

        nrf_rx_mode();                                  //���� ����״̬


        //printf("\nMAX_RT");
    }

    if(state & TX_FULL) //TX FIFO ��
    {
        //printf("\nTX_FULL");

    }
}
