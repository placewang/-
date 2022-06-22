////////////////////////////////////////////////////////////////////////////////
/// @file     MM2400.C
/// @author   Z Yan
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE PROVIDES ALL THE MM2400 EXAMPLE.
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

#define _MM2400_C_

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

#include "mm2400.h"

void EXTI4_15_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line8)){
        EXTI_ClearITPendingBit(EXTI_Line8);
        mm2400_handler();
    }
}

u8 mm2400_init()
{
    HANDLE hEXTI = CreateFile(emIP_EXTI);
    if (hEXTI == NULL)      while(1);

    tAPP_EXTI_DCB dcb = {
        .mode   = emEXTI_IT,                    // emEXTI_IT, emEXTI_Event
        .edge   = emEDGE_Falling,               // emEDGE_Rising, emEXTI_Falling, emEDGE_RisingFalling
        .port   = emGPIOB,
        .hSub   = emFILE_EXTI8
    };

    if (!OpenFile(hEXTI, (void*)&dcb))      while(1);


    hSPI = CreateFile(emIP_SPI);
    if (hSPI == NULL)       while(1);


    // Step 3:  Assignment DCB structure    --------------->>>>>
    tAPP_SPI_DCB spidcb = {
// Base parameter
        .hSub               = MM2400_SPI,
        .type               = emTYPE_Polling,           // emTYPE_Polling,emTYPE_IT,emTYPE_DMA,
        .block              = emTYPE_Block,         // emTYPE_Blocking, emTYPE_NonBlocking

// Callback function
        .sync               = emTYPE_Sync,          // Sync, ASync
        .cbTx               = NULL,    //tx callback function access address
        .cbRx               = NULL,    //rx callback function access address

// operation mode
        .remapEn            = true,             // Disable : 0 ,Enable : 1
        .remapIdx           = 0,                    // u8 value

// SPI parameter
        .prescaler          = 64,
        .mode               = emSPI_MODE_0,
        .hardNss            = false,
        .firstLsb           = false,
        .master             = true
    };

    if (!OpenFile(hSPI, (void*)&spidcb))        while(1);

    mm2400_config();

    return mm2400_link_check();


}


void mm2400_config()
{
    //����mm2400�Ĵ���
    mm2400_CE_LOW();

    mm2400_writeReg(mm2400_WRITE_REG + SETUP_AW, ADR_WIDTH - 2);          //���õ�ַ����Ϊ TX_ADR_WIDTH

    mm2400_writeReg(mm2400_WRITE_REG + RF_CH, CHANAL);                    //����RFͨ��ΪCHANAL
    mm2400_writeReg(mm2400_WRITE_REG + RF_SETUP, 0x0f);                   //����TX�������,0db����,2Mbps,���������濪��

    mm2400_writeReg(mm2400_WRITE_REG + EN_AA, 0x01);                      //ʹ��ͨ��0���Զ�Ӧ��

    mm2400_writeReg(mm2400_WRITE_REG + EN_RXADDR, 0x01);                  //ʹ��ͨ��0�Ľ��յ�ַ

    //RXģʽ����
    mm2400_writeBuf(mm2400_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ

    mm2400_writeReg(mm2400_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);         //ѡ��ͨ��0����Ч���ݿ��

    mm2400_writeReg(FLUSH_RX, NOP);                                    //���RX FIFO�Ĵ���

    //TXģʽ����
    mm2400_writeBuf(mm2400_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); //дTX�ڵ��ַ

    mm2400_writeReg(mm2400_WRITE_REG + SETUP_RETR, 0x0F);                 //�����Զ��ط����ʱ��:250us + 86us;����Զ��ط�����:15��

    mm2400_writeReg(FLUSH_TX, NOP);                                    //���TX FIFO�Ĵ���

    mm2400_rx_mode();                                                  //Ĭ�Ͻ������ģʽ

    mm2400_CE_HIGH();

}

u8 mm2400_writeReg(u8 reg, u8 dat)
{
    return mm2400_write(SPI2, reg, &dat, 1);
}

u8 mm2400_readReg(u8 reg, u8 *pDat)
{
    return mm2400_read(SPI2, reg, pDat, 1);
}

u8 mm2400_writeBuf(u8 reg , u8 *pBuf, u16 len)
{
    return mm2400_write(SPI2, reg, pBuf, len);
}

u8 mm2400_readBuf(u8 reg, u8 *pBuf, u16 len)
{
    return mm2400_read(SPI2, reg, pBuf, len);
}


u8 mm2400_write(SPI_TypeDef* SPIx, u8 reg, u8 *pDat, u16 len)
{
    BSP_SPI_NSS_Configure(SPIx, 1, 0, ENABLE);

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

    BSP_SPI_NSS_Configure(SPIx, 1, 0, DISABLE);

    return status;
}

u8 mm2400_read(SPI_TypeDef* SPIx, u8 reg, u8* pDat, u16 len)
{
    BSP_SPI_NSS_Configure(SPIx, 1, 0, ENABLE);

    WRITE_REG(SPIx->TDR, reg);

    while(!SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL));
    u8 status = READ_REG(SPIx->RDR);

    while(len > 0) {
        WRITE_REG(SPIx->TDR, 0xFF);

        while(!SPI_GetFlagStatus(SPIx, SPI_FLAG_RXAVL));
        *pDat++ = (u8)READ_REG(SPIx->RDR);
        len --;
    }

    BSP_SPI_NSS_Configure(SPIx, 1, 0, DISABLE);
    return status;
}

void mm2400_rx_mode(void)
{
    mm2400_CE_LOW();

    mm2400_writeReg(mm2400_WRITE_REG + EN_AA, 0x01);          //ʹ��ͨ��0���Զ�Ӧ��

    mm2400_writeReg(mm2400_WRITE_REG + EN_RXADDR, 0x01);      //ʹ��ͨ��0�Ľ��յ�ַ

    mm2400_writeBuf(mm2400_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //дRX�ڵ��ַ


    mm2400_writeReg(mm2400_WRITE_REG + CONFIG, 0x0B | (IS_CRC16 << 2));       //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ

    /* ����жϱ�־*/
    mm2400_writeReg(mm2400_WRITE_REG + STATUS, 0xff);

    mm2400_writeReg(FLUSH_RX, NOP);                    //���RX FIFO�Ĵ���

    /*CE���ߣ��������ģʽ*/
    mm2400_CE_HIGH();

    mm2400_mode = RX_MODE;
}

void mm2400_tx_mode(void)
{
    volatile u32 i;

    mm2400_CE_LOW();

    mm2400_writeBuf(mm2400_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH); //дTX�ڵ��ַ

    mm2400_writeBuf(mm2400_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); //����RX�ڵ��ַ ,��ҪΪ��ʹ��ACK

    mm2400_writeReg(mm2400_WRITE_REG + CONFIG, 0x0A | (IS_CRC16 << 2)); //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�


    /*CE���ߣ����뷢��ģʽ*/
    mm2400_CE_HIGH();

    mm2400_mode = TX_MODE;

    i = 0x0fff;
    while(i--);         //CEҪ����һ��ʱ��Ž��뷢��ģʽ

}


u8 mm2400_link_check(void)
{
#define mm2400_CHECH_DATA  0xC2        //��ֵΪУ������ʱʹ�ã����޸�Ϊ����ֵ

    u8 reg;

    u8 txBuff[5] = {mm2400_CHECH_DATA, mm2400_CHECH_DATA, mm2400_CHECH_DATA, mm2400_CHECH_DATA, mm2400_CHECH_DATA};
    u8 rxBuff[5] = {0};
    u8 i;


    reg = mm2400_WRITE_REG + TX_ADDR;
    mm2400_write(SPI2, reg, txBuff, 5);//д��У������


    reg = TX_ADDR;
    mm2400_read(SPI2, reg, rxBuff, 5);//��ȡУ������



    /*�Ƚ�*/
    for(i = 0; i < 5; i++)
    {
        if(rxBuff[i] != mm2400_CHECH_DATA)
        {
            return 0 ;        //MCU��mm2400����������
        }
    }
    return 1 ;             //MCU��mm2400�ɹ�����
}



u16  mm2400_rx(u8 *rxbuf, u16 len)
{
    u32 tmplen = 0;
    u8 tmp;

    while( (mm2400_rx_flag != QUEUE_EMPTY) && (len != 0) )
    {
        if(len < DATA_PACKET)
        {
            memcpy(rxbuf, (u8 *)&(mm2400_ISR_RX_FIFO[mm2400_rx_front]), len);

            mm2400_CE_LOW();

            mm2400_rx_front++;

            if(mm2400_rx_front >= RX_FIFO_PACKET_NUM)
            {
                mm2400_rx_front = 0;
            }
            tmp =  mm2400_rx_rear;
            if(mm2400_rx_front == tmp)
            {
                mm2400_rx_flag = QUEUE_EMPTY;
            }
            mm2400_CE_HIGH();

            tmplen += len;
            return tmplen;
        }

        memcpy(rxbuf, (u8 *)&(mm2400_ISR_RX_FIFO[mm2400_rx_front]), DATA_PACKET);
        rxbuf   += DATA_PACKET;
        len     -= DATA_PACKET;
        tmplen  += DATA_PACKET;

        mm2400_CE_LOW();

        mm2400_rx_front++;

        if(mm2400_rx_front >= RX_FIFO_PACKET_NUM)
        {
            mm2400_rx_front = 0;
        }
        tmp  = mm2400_rx_rear;
        if(mm2400_rx_front == tmp)
        {
            mm2400_rx_flag = QUEUE_EMPTY;
        }
        else
        {
            mm2400_rx_flag = QUEUE_NORMAL;
        }
        mm2400_CE_HIGH();
    }

    return tmplen;
}










u8 mm2400_tx(u8 *txbuf, u8 len)
{
    mm2400_irq_tx_flag = 0;        //��λ��־λ

    if((txbuf == 0 ) || (len == 0))
    {
        return 0;
    }

    if(mm2400_irq_tx_addr == 0 )
    {
        //
        mm2400_irq_tx_pnum = (len - 1) / DATA_PACKET;        // �� 1 ��� �� ����Ŀ
        mm2400_irq_tx_addr = txbuf;

        if( mm2400_mode != TX_MODE)
        {
            mm2400_tx_mode();
        }

        //��Ҫ �ȷ���һ�����ݰ������ �жϷ���

        /*ceΪ�ͣ��������ģʽ1*/
        mm2400_CE_LOW();

        /*д���ݵ�TX BUF ��� 32���ֽ�*/
        mm2400_writeBuf(WR_TX_PLOAD, txbuf, DATA_PACKET);

        /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */
        mm2400_CE_HIGH();

        return 1;
    }
    else
    {
        return 0;
    }
}


mm2400_tx_state_e mm2400_tx_state ()
{
    /*
    if(mm2400_mode != TX_MODE)
    {
        return mm2400_NOT_TX;
    }
    */

    if((mm2400_irq_tx_addr == 0) && (mm2400_irq_tx_pnum == 0))
    {
        //�������
        if(mm2400_irq_tx_flag)
        {
            return mm2400_TX_ERROR;
        }
        else
        {
            return mm2400_TX_OK;
        }
    }
    else
    {
        return mm2400_TXING;
    }
}


void mm2400_handler(void)
{

    u8 state;
    u8 tmp;
    /*��ȡstatus�Ĵ�����ֵ  */
    mm2400_readReg(STATUS, &state);

    /* ����жϱ�־*/
    mm2400_writeReg(mm2400_WRITE_REG + STATUS, state);

    if(state & RX_DR) //���յ�����
    {
        mm2400_CE_LOW();

        if(mm2400_rx_flag != QUEUE_FULL)
        {
            //��û�������������
            //printf("+");
            mm2400_readBuf(RD_RX_PLOAD, (u8 *)&(mm2400_ISR_RX_FIFO[mm2400_rx_rear]), RX_PLOAD_WIDTH); //��ȡ����

            mm2400_rx_rear++;

            if(mm2400_rx_rear >= RX_FIFO_PACKET_NUM)
            {
                mm2400_rx_rear = 0;                            //��ͷ��ʼ
            }
            tmp = mm2400_rx_front;
            if(mm2400_rx_rear == tmp)                 //׷��ƨ���ˣ�����
            {
                mm2400_rx_flag = QUEUE_FULL;
            }
            else
            {
                mm2400_rx_flag = QUEUE_NORMAL;
            }
        }
        else
        {
            mm2400_writeReg(FLUSH_RX, NOP);                    //���RX FIFO�Ĵ���
        }
        mm2400_CE_HIGH();                                      //�������ģʽ
    }

    if(state & TX_DS) //����������
    {
        if(mm2400_irq_tx_pnum == 0)
        {
            mm2400_irq_tx_addr = 0;

            // ע��: mm2400_irq_tx_pnum == 0 ��ʾ ���� �Ѿ�ȫ�����͵�FIFO �� mm2400_irq_tx_addr == 0 ���� ȫ����������

            //������ɺ� Ĭ�� ���� ����ģʽ
            if( mm2400_mode != RX_MODE)
            {
                mm2400_rx_mode();
            }
        }
        else
        {
            if( mm2400_mode != TX_MODE)
            {
                mm2400_tx_mode();
            }

            //��û������ɣ��ͼ�������
            mm2400_irq_tx_addr += DATA_PACKET;    //ָ����һ����ַ
            mm2400_irq_tx_pnum --;                 //����Ŀ����

            /*ceΪ�ͣ��������ģʽ1*/
            mm2400_CE_LOW();

            /*д���ݵ�TX BUF ��� 32���ֽ�*/
            mm2400_writeBuf(WR_TX_PLOAD, (u8 *)mm2400_irq_tx_addr, DATA_PACKET);

            /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */
            mm2400_CE_HIGH();
        }
    }

    if(state & MAX_RT)      //���ͳ�ʱ
    {
        mm2400_irq_tx_flag = 1;                            //��Ƿ���ʧ��
        mm2400_writeReg(FLUSH_TX, NOP);                    //���TX FIFO�Ĵ���


        //�п����� �Է�Ҳ���� ����״̬

        //�������η���
        mm2400_irq_tx_addr = 0;
        mm2400_irq_tx_pnum = 0;

        mm2400_rx_mode();                                  //���� ����״̬


        //printf("\nMAX_RT");
    }

    if(state & TX_FULL) //TX FIFO ��
    {
        //printf("\nTX_FULL");

    }
}


void mm2400_ce_low()
{
    mm2400_writeReg(0x50, 0x53); //mm2400_activate
    mm2400_writeReg(MM2400_WRITE_REG | 0x16, 0x8C);
    mm2400_writeReg(0x50, 0x51); //mm2400_activate
}

void mm2400_power_down()
{
    mm2400_ce_low();
    mm2400_writeReg(MM2400_WRITE_REG | CONFIG, )
}
