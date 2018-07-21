#include "Drivers_RF6936.h"
#include "BSP_SPI.h"
#include "Delay.h"
#include "Drivers_Led.h"

/************************��������************************/
const unsigned char Data_PNCODE_table[4][16] =
{
    {
        0x83, 0xF7, 0xA8, 0x2D, 0x7A, 0x44, 0x64, 0xD3,
        0x3F, 0x2C, 0x4E, 0xAA, 0x71, 0x48, 0x7A, 0xC9,
    },

    {
        0x17, 0xFF, 0x9E, 0x21, 0x36, 0x90, 0xC7, 0x82,
        0xA6, 0x46, 0xB5, 0x9A, 0x3A, 0x30, 0xB6, 0xAD,
    },

    {
        0xBC, 0x5D, 0x9A, 0x5B, 0xEE, 0x7F, 0x42, 0xEB,
        0x24, 0xF5, 0xDD, 0xF8, 0x7A, 0x77, 0x74, 0xE7,
    },

    {
        0x3D, 0x70, 0x7C, 0x94, 0xDC, 0x84, 0xAD, 0x95,
        0x1E, 0x6A, 0xF0, 0x37, 0x52, 0x7B, 0x11, 0xD4,
    },

};

const unsigned char  SOP_PNCODE_table[12][8] =
{
    {0xB7, 0x8F, 0x2B, 0x01, 0xEF, 0x76, 0x0D, 0x01,},     // PN Code 0
    {0x82, 0xC7, 0x90, 0x36, 0x21, 0x9e, 0xFF, 0x17,},     // PN Code 1 default
    {0xE3, 0x5C, 0xFA, 0x02, 0x97, 0x93, 0xF9, 0x02,},     // PN Code 2
    {0xCB, 0x52, 0x77, 0x03, 0x1A, 0xBF, 0x74, 0x03,},     // PN Code 3
    {0xBF, 0x6B, 0x90, 0x06, 0xF3, 0x8E, 0x8D, 0x06,},     // PN Code 4
    {0x4F, 0x17, 0xEC, 0x09, 0xAD, 0xA4, 0xEB, 0x09,},     // PN Code 5
    {0xAD, 0x39, 0xA2, 0x0F, 0x9B, 0xC5, 0xA1, 0x0F,},     // PN Code 6
    {0x3A, 0x6F, 0x89, 0x1C, 0xDF, 0xD4, 0x88, 0x1C,},     // PN Code 7
    {0xD2, 0x8F, 0xB1, 0x2A, 0xEF, 0x64, 0xB0, 0x2A,},     // PN Code 8
    {0x66, 0xCD, 0x7C, 0x50, 0xDD, 0x26, 0x7C, 0x50,},     // PN Code 9
    {0xE2, 0xF8, 0xCC, 0x91, 0x3C, 0x37, 0xCC, 0x91,},     // PN Code 10
    {0xB2, 0x3D, 0x0D, 0xA5, 0xDF, 0x83, 0x0C, 0xA5,},     // PN Code 11
};
const unsigned char PREAM_pn_code_table[12][3] =
{
    {0x02, 0xF7, 0x02},
    {0x02, 0x7A, 0x02},
    {0x02, 0xD3, 0x3F},
    {0x02, 0x4E, 0xAA},
    {0x02, 0x48, 0x7A},
    {0x02, 0x17, 0xFF},
    {0x02, 0x21, 0x36},
    {0x02, 0xC7, 0x82},
    {0x02, 0x46, 0xB5},
    {0x02, 0x3A, 0x30},
    {0x02, 0xAD, 0xBC},
    {0x02, 0x9A, 0x5B},

};
/*****************************�����������***************************/

/*******************************************************************************
* Function Name : void Drivers_Rf6936RegWrite(uint8_t pAddr, uint8_t mData)
* Description   : Rf6936ģ��Ĵ���д��
* Author        : YJD
* Input         : pAddr:д��Ĵ�����ַ          mData: д��Ĵ�������
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/

void Drivers_Rf6936RegWrite(uint8_t pAddr, uint8_t mData)
{
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, pAddr, mData);
}

/*******************************************************************************
* Function Name : void Drivers_Rf6936RegWrite(uint8_t pAddr, uint8_t mData)
* Description   : ��Rf6936ģ��Ĵ���
* Author        : YJD
* Input         : pAddr:���Ĵ�����ַ
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/

uint8_t Drivers_Rf6936RegRead(uint8_t pAddr)
{
    return  BSP_SpiReadByte(FR6936_SPI_NUMBER, pAddr);

}

/*******************************************************************************
* Function Name : void Drivers_Rf6936Wake(void)
* Description   : Rf6936ģ�黽��
* Author        : YJD
* Input         :
* Output        :
* Other         :
* Date          : 2018.05.11
*******************************************************************************/
void Drivers_Rf6936Wake(void)
{
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, MODE_OVERRIDE_ADR, 0x38);
}
void Drivers_Rf6936RxStart(void)          //��δִ�н��ն�������������
{
    PA_EN_SET;
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, XACT_CFG_ADR,
                     FRC_END_STATE | END_STATE_IDLE | ACK_TO_4X);
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, RX_CTRL_ADR, RX_GO);
    DelayUs(2);    //��ʱ2uS
}

void Drivers_Rf6936RxDIdle(void)         //��δִ�н��ն�������������
{
    PA_EN_SET;
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, XACT_CFG_ADR,
                     FRC_END_STATE | END_STATE_IDLE | ACK_TO_4X);
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, RX_CTRL_ADR, RX_GO);
    DelayUs(2);//��ʱ2uS

}
void Drivers_Rf6936RadioSetSopPnCode(uint8_t remotePnAddr[])
{
    uint8_t i;
    for(i = 0; i < 8; i++ )
    {
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, SOP_CODE_ADR, remotePnAddr[i]);
    }
}

void Drivers_Rf6936RadioSetChannel(uint8_t mChannel)
{
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, CHANNEL_ADR, mChannel );
}

void Drivers_Rf6936RadioDataPnCode(uint8_t remoteDataPnAddr[])
{
    uint8_t i;

    for( i = 0; i < 16; i++ )
    {
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, DATA_CODE_ADR, remoteDataPnAddr[i]);
    }
}

void Drivers_Rf6936RadioSetPreamPnCode(uint8_t remotePreamAddr[],
                                       uint8_t mRemoval)
{
    uint8_t i;
    for( i = 0; i < 3; i++ )
    {
        if(mRemoval != 0)
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, PREAMBLE_ADR,  remotePreamAddr[i] );
        else
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, PREAMBLE_ADR, 0);
    }
}

void Drivers_Rf6936RadioSetMfgChannel(uint32_t mChannel)
{
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, CHANNEL_ADR,
                     (mChannel + LS_CHANNEL_OFFSET));
}//mChannel+2

void Drivers_Rf6936SetPowerAndBitrate(uint8_t mRf0BaundRate,
                                      uint8_t remotePower)
{
    nop();
    nop();
    switch(((mRf0BaundRate & 0xf0) >> 4))
    {
        case 0x00:
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CFG_ADR, DATMODE_RAW | remotePower);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, FRAMING_CFG_ADR, 0x0);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OFFSET_LSB_ADR, 0x55 );
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OFFSET_MSB_ADR, 0x05 );

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, CRC_SEED_MSB_ADR, 0x1a);   //CRC����
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, CRC_SEED_LSB_ADR, 0xbb);
            return;
        case 0x01:

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CFG_ADR,
                             (DATCODE_LEN_32 | DATMODE_8DR | remotePower));
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, DATA32_THOLD_ADR, 0x05);
            break;
        case 0x02:

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CFG_ADR,
                             (DATCODE_LEN_64 | DATMODE_8DR | remotePower));
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, DATA64_THOLD_ADR, 0x0e);
            break;
        case 0x03:

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CFG_ADR,
                             DATCODE_LEN_32 | DATMODE_DDR | remotePower);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, DATA32_THOLD_ADR, 0x05);
            break;
        case 0x04:

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CFG_ADR,
                             DATCODE_LEN_64 | DATMODE_DDR | remotePower);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, DATA64_THOLD_ADR, 0x0e);
            break;
        case 0x05:

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CFG_ADR,
                             (DATCODE_LEN_64 | DATMODE_SDR | remotePower));
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, DATA64_THOLD_ADR, 0x0e);
            break;
        default:
            break;
    }

}

void Drivers_Rf6936CheckCryStalStableBack(void)
{
    uint8_t i;
    for(i = 0; i < 1000; i++)
    {
        if((Drivers_Rf6936RegRead( TX_IRQ_STATUS_ADR ) & 0x80) != 0)
        {
            nop();
            nop();
            i = 1000;
        }
    }
    nop();
    nop();
}

void Drivers_Rf6936CheckCryStalStable(void)
{
    uint8_t i;
    for(i = 0; i < 6000; i++)
    {
        if((Drivers_Rf6936RegRead(TX_IRQ_STATUS_ADR ) & 0x80) != 0)
        {
            nop();
            nop();
            i = 6000;
        }
    }
    nop();
    nop();
}

void Drivers_Rf6936RemotePowerTest(void)           //���书�ʲ���
{

    BSP_SpiWriteByte(FR6936_SPI_NUMBER, PREAMBLE_ADR, 0x01);
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, PREAMBLE_ADR, 0x00);
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, PREAMBLE_ADR, 0x00);
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OVERRIDE_ADR, FRC_PREAMBLE );
    BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CTRL_ADR, TX_GO );

}
void Drivers_Rf6936RadioIniMap(PcontrolerConfig pControlerConfig)
{

    uint8_t temp;
    for(temp = 0; temp < 8; temp++)
        pControlerConfig->TagNode.remot_PN_adress[temp] =
            SOP_PNCODE_table[(pControlerConfig->TagNode.remot_PN_code % 12)][temp];

    for(temp = 0; temp < 16; temp++)
        pControlerConfig->TagNode.remot_DATA_pn_addr[temp] =
            Data_PNCODE_table[(pControlerConfig->TagNode.remot_PN_code % 4)][temp];

    for(temp = 0; temp < 3; temp++)
        pControlerConfig->TagNode.remot_pream_addr[temp] =
            PREAM_pn_code_table[pControlerConfig->TagNode.remot_pream_code][temp] ;
}

void Drivers_Rf6936SelectChannel(PcontrolerConfig pControlerConfig)
{

    switch(pControlerConfig->TagNode.chanel_temp)
    {
        case 0x01:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_1channel);//����1ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 1;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 1;
            break;
        case 0x02:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_2channel);//����2ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 2;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 2;
            break;
        case 0x03:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_3channel);//����1ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 3;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 3;
            break;
        case 0x04:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_4channel);//����2ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 4;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 4;
            break;
        case 0x05:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_5channel);//����1ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 5;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 5;
            break;
        case 0x06:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_6channel);//����2ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 6;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 6;
            break;
        case 0x07:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_7channel);//����2ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode + 7;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode + 7;
            break;
        default:
            Drivers_Rf6936RadioSetMfgChannel(
                pControlerConfig->TagNode.remot_0channel);//����0ͨ��
            pControlerConfig->TagNode.remot_PN_code =
                pControlerConfig->TagNode.remot_PN_Constcode;
            pControlerConfig->TagNode.remot_pream_code =
                pControlerConfig->TagNode.remot_pream_Constcode;
            break;
    }
    Drivers_Rf6936RadioIniMap(pControlerConfig);
    Drivers_Rf6936RadioDataPnCode(
        pControlerConfig->TagNode.remot_DATA_pn_addr);             //��������pn��
    Drivers_Rf6936RadioSetSopPnCode(
        pControlerConfig->TagNode.remot_PN_adress);              //����sop pn��
    Drivers_Rf6936RadioSetPreamPnCode(pControlerConfig->TagNode.remot_pream_addr,
                                      1);        //����ǰ����
    DelayUs(10);   //2uS*

}

uint8_t Drivers_Rf6936Init(PcontrolerConfig pControlerConfig, uint8_t mRemoval)
{
    uint8_t i;
    DISABLE_RF_IRQ_INT();                                           //#define DISABLE_RF_IRQ_INT() PD_CR2_C24 = 0
    switch(pControlerConfig->TagNode.RFiniFlag)
    {
        case 0x01:
            Drivers_Rf6936Wake();
            Drivers_Rf6936SelectChannel(pControlerConfig);
            break;
        default:
            SETB_RSET_SET;
            DelayUs(10);
            SETB_RSET_CLR;
            DelayUs(20);
            DelayUs(50);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, MODE_OVERRIDE_ADR,
                             RF_RST );                          //device rst
            DelayUs(100);
            Drivers_Rf6936SelectChannel(pControlerConfig);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, XACT_CFG_ADR,
                             FRC_END_STATE | END_STATE_IDLE | ACK_TO_4X);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, PWR_CTRL_ADR, 0x80);
            Drivers_Rf6936SetPowerAndBitrate(pControlerConfig->TagNode.RF0bard,
                                             pControlerConfig->TagNode.remot_power);
            switch(mRemoval)
            {
                case 0x00:
                    pControlerConfig->TagNode.FRAMe_Cfg_ADR = SOP_EN | LEN_EN | 0x04;
                    BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OVERRIDE_ADR, 0x00);
                    BSP_SpiWriteByte(FR6936_SPI_NUMBER, RX_OVERRIDE_ADR, 0x08);

                    break;
                case 0x01:
                    pControlerConfig->TagNode.FRAMe_Cfg_ADR  = SOP_EN | SOP_LEN | LEN_EN | 0x0e;
                    BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OVERRIDE_ADR, 0);
                    BSP_SpiWriteByte(FR6936_SPI_NUMBER, RX_OVERRIDE_ADR, 0x08);
                    break;
                default:
                    break;
            }
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, RX_CFG_ADR,
                             (pControlerConfig->TagNode.reci_sensity | HILO_EN | RXOW_EN) );
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, FRAMING_CFG_ADR,
                             pControlerConfig->TagNode.FRAMe_Cfg_ADR);
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OFFSET_LSB_ADR, 0x55 );
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_OFFSET_MSB_ADR, 0x05 );

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, CRC_SEED_MSB_ADR,
                             0x1a);               //CRC����
            BSP_SpiWriteByte(FR6936_SPI_NUMBER, CRC_SEED_LSB_ADR, 0xbb);
            break;
            //RFCloseFlag=1;            //��rf�رձ�־
    }

    return 1;
}


uint8_t Drivers_Rf6936CheckRssiValue(Pcontroler_Symple pSymple, uint32_t value)
{
    if(value > (pSymple->Config.TagPara.Rssi_bottom + 3))
        return 0;
    else
        return 1;
}

uint8_t Drivers_Rf6936CollisoHneadDeal(Pcontroler_Symple pSymple)
{
    uint8_t mPointer = 0;
    uint8_t mTemp[10] = {0};
    uint8_t i = 0;
    uint32_t mCount = 0;
    uint32_t mTotal = 50;

    for(mCount = 0; mCount < 300; mCount++)
    {
        nop();
        mTemp[mPointer++] = Drivers_Rf6936RegRead(RSSI_ADR) & 0x1f;
        mPointer = mPointer % 6;
        if(mCount > 12)
        {
            for(i = 0; i < 6; i++ )
            {
                mTotal += mTemp[i];
            }
            mTotal = mTotal /
                     6;                               //���ɼ������ź�ǿ�����ȡƽ��
            mCount = 300;

            if(Drivers_Rf6936CheckRssiValue(mTotal) == 1)
                return 1;
            if(mCount > 150)
            {
                return 0;
            }
        }
        DelayUs(10);

    }
    return 0;
}


uint8_t BSP_Rf6936SendPackage(uint8_t retryTimes, uint8_t sendLen,
                              uint8_t packageType, uint8_t sendNodeCount, uint8_t sendPackCount)
{
    uint8_t mStatus = 0;
    uint8_t sendCount = 0;
    uint8_t *mTemp;
    uint8_t txCount = 0;
    uint32_t ryCycle = 0;
    uint32_t i = 0;
    do
    {
        if(packageType != 2)
        {
            if(Drivers_Rf6936CollisoHneadDeal() == 0)
            {
                nop();
                if (sendNodeCount < (sendPackCount + 3))
                {
                    PA_EN_CLR;                            //�رշ��͹��ŵ�Դ�����ڽ��͹���
                    return 0;
                }
            }
        }
        Drivers_FlashLeds(1, pSymple->Config.TagNode.LEDFlag);
        mTemp = (unsigned char *)&pSymple->txReci[0];
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, XACT_CFG_ADR,
                         FRC_END_STATE | END_STATE_IDLE | ACK_TO_4X);  //�������ģʽ
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_LENGTH_ADR,
                         sendLen);                                //д�뷢�����ݳ���
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CTRL_ADR,
                         TX_CLR );                                  //������ͻ�����

        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_BUFFER_ADR,
                         mTemp[0]);                               //д�����ݣ���1�ֽ�
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_BUFFER_ADR,
                         mTemp[1]);                               //д�����ݣ���2�ֽ�
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_BUFFER_ADR,
                         mTemp[2]);                               //д�����ݣ���3�ֽ�
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_BUFFER_ADR,
                         mTemp[3]);                               //д�����ݣ���4�ֽ�
        BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_CTRL_ADR, TX_GO);//��������
        //������͹������������������Ϊ�պ󳬹�ʱ��δ�������ݣ��ᵼ�´�����
        //Ϊ��ֹ����������д�벿�����ݵ�������������������
        //���͹�����ѭ����鷢�ͻ������Ƿ�Ϊ��������������������ݣ�
        //������ʱ�ȴ�����ȡΪδ��,Ȼ����������
        //��ʱʱ��ϵ���ɲ����ʾ���
        for(sendCount = 4; sendCount < sendLen; sendCount++)
        {

            BSP_SpiWriteByte(FR6936_SPI_NUMBER, TX_BUFFER_ADR, mTemp[sendCount]);//д������
            for(i = 0; i < pSymple->Config.TagPara.tempComp; i++)
            {
                mStatus = Drivers_Rf6936RegRead(TX_IRQ_STATUS_ADR);
                if((mStatus & 0x20) != 0)
                {
                    i = pSymple->Config.TagPara.tempComp + 1;
                }
            }

        }
        nop();
        nop();
        if(packageType == 0x02)
        {
            pSymple->Config.TagPara.tempOver = 1200;
        }
        for(ryCycle = 0; ryCycle < pSymple->Config.TagPara.tempOver; ryCycle++)
        {
            mStatus = Drivers_Rf6936RegRead(TX_IRQ_STATUS_ADR);
            nop();
            nop();
            if((mStatus & TXC_IRQ) != 0)       //�жϴ�������ж�״̬
            {
                nop();
                PA_EN_CLR;                     //�رշ��͹��ŵ�Դ�����ڽ��͹���
                LED1_CLR;
                switch(packageType)
                {
                    case 0x00:
                        pSymple->Config.TagPara.CollisonInterval = 0;
                        pSymple->Config.TagNode.RFReciTDelay = 6;
                        return 1;
                    case 0x01:
                        return  mStatus;
                    case 0x02:
                        pSymple->Config.TagPara.CollisonInterval = 0;
                        pSymple->Config.TagNode.RFReciTDelay = 0;
                        nop();
                        nop();
                        return 1;
                    default:
                        return 0;
                }
            }
            if((mStatus & 0x05) != 0)       //�������buffer��������ʹ������˳�ѭ��
            {
                ryCycle = pSymple->Config.TagPara.tempOver;

            }
            DelayUs(4);//2uS*2
        }
        nop();
        nop();
        txCount++  ;
    }
    while( txCount < retryTimes );
    nop();
    nop();
    PA_EN_CLR;//�رշ��͹��ŵ�Դ�����ڽ��͹���
    LED1_CLR;
    return 0;
}

uint8_t Drivers_Rf6936GetChannelTimer(Pcontroler_Symple pSymple)
{
    //ȡ��ϵͳ����ʱ��
    switch(((pSymple->Config.TagNode.RF0bard & 0x0f0) >> 4))
    {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x04:
            pSymple->Config.TagPara.WaitRspTimerConst = 60;
            pSymple->Config.TagPara.tempComp = 240;
            pSymple->Config.TagPara.GetAckDls = 320;
            pSymple->Config.TagPara.tempOver = 1200;
            break;
        case 0x03:
            pSymple->Config.TagPara.WaitRspTimerConst = 30;
            pSymple->Config.TagPara.tempComp = 160;
            pSymple->Config.TagPara.GetAckDls = 160;
            pSymple->Config.TagPara.tempOver = 600;
            break;
        case 0x05:
            pSymple->Config.TagPara.WaitRspTimerConst = 120;
            pSymple->Config.TagPara.tempComp = 400;
            pSymple->Config.TagPara.GetAckDls = 640;
            pSymple->Config.TagPara.tempOver = 3200;
            break;
        default:
            return 0;

    }
    return 1;
}
void Drivers_6936UniversalTest()
{

    LED1_CLR;
    LED2_CLR;
    PcontrolerConfig pControlerConfig = (PcontrolerConfig)pSymple;
    if(Drivers_Rf6936GetChannelTimer() != TRUE)
        return;
    pControlerConfig->TagNode.RFiniFlag = 0;
    pControlerConfig->TagNode.chanel_temp = 1;
    pControlerConfig->TagNode.remot_power =
        0;         //���ͷ��͹��ʣ����Ʒ�Χ//��ʱ���ģ����幦����Ҫʵ�ʲ���
    pControlerConfig->TagNode.remot_1channel =
        96;     //�ı�F1ͨ��ͨѶƵ��ΪĬ��Ƶ��
    pControlerConfig->TagNode.reci_sensity =
        0x20;     //���ͽ��������ȣ����Ʒ�Χ
    pControlerConfig->TagNode.UserCode = 1;            //�ͻ���
    pSymple->Config.TagNode.Tag_id_nub[0] = 0;         //��ǩ��ַ
    pSymple->Config.TagNode.Tag_id_nub[1] = 0;
    pSymple->Config.TagNode.Tag_id_nub[2] = 0;



}




