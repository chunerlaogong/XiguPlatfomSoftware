
#include "Drivers_SX1280.h"
#include "Delay.h"
static SDRIVERS_SX1280_CFG_TypeDef G_SX1280_CFG;
static sSx1280RegCfg      G_tSx1280RegCfgInfo;
static PacketParams_t  packetParams;
/**********************
* brief Holds the internal operating mode of the radio
**********************/
RadioOperatingModes_t OperatingMode;
/**********************
* brief Stores the current packet type set in the radio
**********************/
RadioPacketTypes_t PacketType = PACKET_TYPE_GFSK;


/**********************
* brief Holds the polling state of the driver
**********************/
bool PollingMode;

uint8_t SYNC_WORD[5] = {0xDD, 0xA0, 0x96, 0x69, 0xDD};


uint8_t           G_BusyStatus;       //æµ״̬
uint16_t          G_SXDevVer;         //1280�豸�汾��Ϣ
RadioStatus_t   G_RFStatus;           //״̬
PacketStatus_t  G_PacketStatus;        //Packet״��



sWRCfg        G_WRCfgInfo ;         //��д������

static QUEUE8_t SX1280_RX_QUEUE = {0};


/*****************************************
*Name:Sx1280_PwrCfgInfo
*Description:SX1280��ز����趨
*Input:NULL
*Output:NULL
*Author:
*Date:
******************************************/
void Sx1280_PwrCfgInfo()
{
    G_tSx1280RegCfgInfo .PacketType = PACKET_TYPE_GFSK;
    G_tSx1280RegCfgInfo .BitrateBandwidth =
        GFS_BLE_BR_0_125_BW_0_3;   //GFSK�����ٶȺʹ�����0.125Mb/s, 0.3MHZ
    G_tSx1280RegCfgInfo .ModulationIndex = GFS_BLE_MOD_IND_1_00;       //�ŵ�
    G_tSx1280RegCfgInfo .ModulationShaping = RADIO_MOD_SHAPING_BT_1_0;
    G_tSx1280RegCfgInfo .PreambleLength = PREAMBLE_LENGTH_32_BITS;
    G_tSx1280RegCfgInfo .SyncWordLength = GFS_SYNCWORD_LENGTH_5_BYTE;
    G_tSx1280RegCfgInfo .SyncWordMatch = RADIO_RX_MATCH_SYNCWORD_1;
    G_tSx1280RegCfgInfo .HeaderType = RADIO_PACKET_VARIABLE_LENGTH;
    G_tSx1280RegCfgInfo .PayloadLength = 15;
    G_tSx1280RegCfgInfo .CrcLength = RADIO_CRC_2_BYTES;
    G_tSx1280RegCfgInfo .Whitening = RADIO_WHITENING_ON;

    G_tSx1280RegCfgInfo .USE_MODE = USE_DCDC;
    G_tSx1280RegCfgInfo .StandByMode = STDBY_RC;
    G_tSx1280RegCfgInfo .RF_FREQUENCY =  2460000000;     //��ƵƵ��
    G_tSx1280RegCfgInfo .TX_OUTPUT_POWER =
        31;           //���书��      0~31����-18dBm��13dBm
    G_tSx1280RegCfgInfo .RADIO_RAMP_TIME = RADIO_RAMP_02_US;
    G_tSx1280RegCfgInfo .irqMask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT;
    G_tSx1280RegCfgInfo .dio1Mask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT;
    G_tSx1280RegCfgInfo .dio2Mask = IRQ_RADIO_NONE;
    G_tSx1280RegCfgInfo .dio3Mask = IRQ_RADIO_NONE;
    G_tSx1280RegCfgInfo .RXTimeOut.Step = RADIO_TICK_SIZE_1000_US;
    G_tSx1280RegCfgInfo .RXTimeOut.NbSteps =
        0xFFFF;//0000ֻ����һ�Σ�0xFFFFΪ��������
    //��ʼ��G_SX1280_CFG
    G_SX1280_CFG.maxTryTimes = 500;   //RF��������Դ���
}


/*******************************************
*Name:SX1280SetInterruptMode
*Description:����Ϊ�ж�ģʽ
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetInterruptMode(void)
{
    PollingMode = false;
}

/*******************************************
*Name:SX1280HalReset
*Description:SX1280��λ��������������
*Input:��Ƶͨ����
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalReset(void)
{
    DelayMs(40);
    SX1280_RESET_SET_LOW(G_SX1280_CFG.SX1280_RESET_GPIO_BASE,
                         G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin);
    DelayMs(50);
    SX1280_RESET_SET_HIGH(G_SX1280_CFG.SX1280_RESET_GPIO_BASE,
                          G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin);
    DelayMs(40);
}

/*******************************************
*Name:SX1280HalInit
*Description:SX1280��ʼ��
*Input:�ж�ע�ᣬͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalInit(void)
{
    SX1280HalReset();               //��λ
    SX1280SetInterruptMode();           //���ù���ģʽ����ѯ�����ж�
}

/*******************************************
*Name:SX1280HalWaitOnBusy
*Description:�ȴ��豸״̬�ܽ�����
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalWaitOnBusy(void)
{
    uint32_t cnt = 500000;
    while((SX1280_READ_BUSY(G_SX1280_CFG.SX1280_BUSY_GPIO_BASE,
                            G_SX1280_CFG.SX1280_BUSY_EN_GPIO_Pin ) == 1) && (cnt > 0))
    {
        cnt--;
    };

    if(cnt != 0)
    {
        G_BusyStatus = true;
    }
    else
    {
        G_BusyStatus = false;
    }
}

/*******************************************
*Name:SX1280_WRITEBYTE
*Description: SX1280д����
*Input:���ݡ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
uint8_t SX1280_WRITEBYTE(uint8_t ucData)
{
    while(!(G_SX1280_CFG.SX1280_SPI_BASE->SR & SPI_SR_TXE));
    G_SX1280_CFG.SX1280_SPI_BASE->DR = ucData;
    while(!(G_SX1280_CFG.SX1280_SPI_BASE->SR & SPI_SR_RXNE));
    return G_SX1280_CFG.SX1280_SPI_BASE->DR;
}

/*******************************************
*Name:SX1280HalWriteCommand
*Description:SX1280д
*Input:�����֣���ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalWriteCommand(RadioCommands_t command, uint8_t *buffer,
                           uint16_t size)
{
    uint16_t i = 0;
    SX1280HalWaitOnBusy();
    SX1280_CS_SET_LOW(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280_WRITEBYTE((uint8_t)command);
    for(i = 0; i < size; i++)
    {
        SX1280_WRITEBYTE(buffer[i]);
    }
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    if(command != RADIO_SET_SLEEP)
    {
        SX1280HalWaitOnBusy();
    }
}

/*******************************************
*Name:SX1280SetRegulatorMode
*Description:SX1280У׼ģʽ����
*Input:У׼������ͨ����
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetRegulatorMode(RadioRegulatorModes_t mode)
{
    SX1280HalWriteCommand(RADIO_SET_REGULATORMODE, (uint8_t*)&mode, 1);
}


/*******************************************
*Name:SX1280HalReadRegisters
*Description:SX1280�Ĵ������
*Input:�Ĵ�����ַ�����ݵ�ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;
    SX1280HalWaitOnBusy();
    SX1280_CS_SET_LOW(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280_WRITEBYTE(RADIO_READ_REGISTER);
    SX1280_WRITEBYTE((address & 0xFF00) >> 8);
    SX1280_WRITEBYTE((address & 0x00FF));
    SX1280_WRITEBYTE(0);
    for(i = 0; i < size; i++)
    {
        buffer[i] = SX1280_WRITEBYTE(0);
    }
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280HalWaitOnBusy();
}
/*******************************************
*Name:SX1280HalReadRegister
*Description:SX1280�Ĵ�����
*Input:�Ĵ�����ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
uint8_t SX1280HalReadRegister(uint16_t address)
{
    uint8_t data = 0;
    SX1280HalReadRegisters(address, &data, 1);
    return data;
}


/*******************************************
*Name:SX1280GetFirmwareVersion
*Description:1280��ȡ�汾��
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
uint16_t SX1280GetFirmwareVersion(void)
{
    return(((SX1280HalReadRegister(0xA8)) << 8) | (SX1280HalReadRegister(0xA9)));
}

/*******************************************
*Name:SX1280SetStandby
*Description:1280��������ģʽ
*Input:����������ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetStandby(RadioStandbyModes_t standbyConfig)
{
    SX1280HalWriteCommand(RADIO_SET_STANDBY, (uint8_t*)&standbyConfig, 1);
    if(standbyConfig == STDBY_RC )
    {
        OperatingMode = MODE_STDBY_RC;   //��С��������������ʽ
    }
    else
    {
        OperatingMode = MODE_STDBY_XOSC; //����������ʽ
    }
}

/*******************************************
*Name:SX1280SetPacketType
*Description:1280���÷��Ͱ�����
*Input:�����ͣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetPacketType(RadioPacketTypes_t packetType)
{
    // Save packet type internally to avoid questioning the radio
    PacketType = packetType;
    SX1280HalWriteCommand(RADIO_SET_PACKETTYPE, (uint8_t*)&PacketType, 1);
}

/*******************************************
*Name:SX1280SetModulationParams
*Description:����1280���Ʋ���
*Input:���Ʋ�����ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetModulationParams(ModulationParams_t *modulationParams)
{
    uint8_t buf[3];
    //Check if required configuration corresponds to the stored packet type
    //If not, silently update radio packet type
    if(PacketType != modulationParams->PacketType)
    {
        SX1280SetPacketType(modulationParams->PacketType);
    }
    switch(modulationParams->PacketType)
    {
        case PACKET_TYPE_GFSK:
            buf[0] = modulationParams->Params.Gfsk.BitrateBandwidth;
            buf[1] = modulationParams->Params.Gfsk.ModulationIndex;
            buf[2] = modulationParams->Params.Gfsk.ModulationShaping;
            break;
        case PACKET_TYPE_NONE:
            buf[0] = NULL;
            buf[1] = NULL;
            buf[2] = NULL;
            break;
    }
    SX1280HalWriteCommand(RADIO_SET_MODULATIONPARAMS, buf, 3);
}

/*******************************************
*Name:SX1280SetPacketParams
*Description:����1280������
*Input:���ݰ�������ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetPacketParams(PacketParams_t *packetParams)
{
    uint8_t buf[7];
    //Check if required configuration corresponds to the stored packet type
    //If not, silently update radio packet type
    if(PacketType != packetParams->PacketType)
    {
        SX1280SetPacketType(packetParams->PacketType);
    }
    switch( packetParams->PacketType)
    {
        case PACKET_TYPE_GFSK:
            buf[0] = packetParams->Params.Gfsk.PreambleLength;
            buf[1] = packetParams->Params.Gfsk.SyncWordLength;
            buf[2] = packetParams->Params.Gfsk.SyncWordMatch;
            buf[3] = packetParams->Params.Gfsk.HeaderType;
            buf[4] = packetParams->Params.Gfsk.PayloadLength;
            buf[5] = packetParams->Params.Gfsk.CrcLength;
            buf[6] = packetParams->Params.Gfsk.Whitening;
            break;
        case PACKET_TYPE_NONE:
            buf[0] = NULL;
            buf[1] = NULL;
            buf[2] = NULL;
            buf[3] = NULL;
            buf[4] = NULL;
            buf[5] = NULL;
            buf[6] = NULL;
            break;
    }
    SX1280HalWriteCommand(RADIO_SET_PACKETPARAMS, buf, 7);
}

/*******************************************
*Name:SX1280SetRfFrequency
*Description:����1280��ƵƵ��
*Input:��ƵƵ�ʣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetRfFrequency(uint32_t frequency)
{
    uint8_t buf[3];
    uint32_t freq = 0;
    freq = (uint32_t)((double)frequency / (double)FREQ_STEP);
    buf[0] = (uint8_t)((freq >> 16) & 0xFF);
    buf[1] = (uint8_t)((freq >> 8) & 0xFF);
    buf[2] = (uint8_t)(freq & 0xFF);
    SX1280HalWriteCommand(RADIO_SET_RFFREQUENCY, buf, 3);
}

/*******************************************
*Name:SX1280SetBufferBaseAddresses
*Description:����1280���շ��ͻ����ַ
*Input:���ͻ���ַ�����ջ���ַ��ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetBufferBaseAddresses(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
    uint8_t buf[2];
    buf[0] = txBaseAddress;
    buf[1] = rxBaseAddress;
    SX1280HalWriteCommand(RADIO_SET_BUFFERBASEADDRESS, buf, 2);
}

/*******************************************
*Name:SX1280SetTxParams
*Description:����1280���Ͳ���
*Input:��Ƶ���ʣ���������ʱ�䣬ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetTxParams(uint8_t power, RadioRampTimes_t rampTime)
{
    uint8_t buf[2];
    //The power value to send on SPI/UART is in the range [0..31] and the
    //physical output power is in the range [-18..13]dBm
    buf[0] = power;
    buf[1] = (uint8_t)rampTime;
    SX1280HalWriteCommand(RADIO_SET_TXPARAMS, buf, 2);
}

/*******************************************
*Name:SX1280GetPacketType
*Description:��ȡ1280������������
*Input:ͨ��
*Output:������
*Author:
*Date:
********************************************/
RadioPacketTypes_t SX1280GetPacketType(void)
{
    //SX1280HalWriteCommand(RADIO_GET_PACKETTYPE,(uint8_t*)&PacketType,1);
    return PacketType;
}

/*******************************************
*Name:SX1280HalWriteRegisters
*Description:SX1280�Ĵ�����д
*Input:�Ĵ�����ַ�����ݵ�ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalWriteRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    uint16_t i = 0;
    SX1280HalWaitOnBusy();
    SX1280_CS_SET_LOW(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280_WRITEBYTE(RADIO_WRITE_REGISTER);
    SX1280_WRITEBYTE(((address & 0xFF00) >> 8));
    SX1280_WRITEBYTE((address & 0x00FF));
    for(i = 0; i < size; i++)
    {
        SX1280_WRITEBYTE(buffer[i]);
    }
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280HalWaitOnBusy();
}


/*******************************************
*Name:SX1280SetSyncWord
*Description:ͬ��������
*Input:ͬ��ͨ����ͬ���֣�ͨ����
*Output:״̬
*Author:
*Date:
********************************************/
uint8_t SX1280SetSyncWord(uint8_t syncWordIdx, uint8_t *syncWord)
{
    uint16_t addr;
    uint8_t syncwordSize = 0;
    switch( SX1280GetPacketType())
    {
        case PACKET_TYPE_GFSK:
            syncwordSize = 5;
            switch(syncWordIdx)
            {
                case 1:
                    addr = REG_LR_SYNCWORDBASEADDRESS1;
                    break;
                case 2:
                    addr = REG_LR_SYNCWORDBASEADDRESS2;
                    break;
                case 3:
                    addr = REG_LR_SYNCWORDBASEADDRESS3;
                    break;
                default:
                    return 1;
            }
            break;
        default:
            return 1;
    }
    SX1280HalWriteRegisters(addr, syncWord, syncwordSize);
    return 0;
}

/*******************************************
*Name:SX1280SetDioIrqParams
*Description:sx1280�жϲ�������
*Input:�ж����μĴ������жϲ���1���жϲ���2���жϲ���3��ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetDioIrqParams(uint16_t irqMask, uint16_t dio1Mask,
                           uint16_t dio2Mask, uint16_t dio3Mask)
{
    uint8_t buf[8];
    buf[0] = (uint8_t)((irqMask >> 8) & 0x00FF);
    buf[1] = (uint8_t)(irqMask & 0x00FF );
    buf[2] = (uint8_t)((dio1Mask >> 8) & 0x00FF);
    buf[3] = (uint8_t)(dio1Mask & 0x00FF);
    buf[4] = (uint8_t)((dio2Mask >> 8) & 0x00FF);
    buf[5] = (uint8_t)(dio2Mask & 0x00FF);
    buf[6] = (uint8_t)((dio3Mask >> 8) & 0x00FF);
    buf[7] = (uint8_t)(dio3Mask & 0x00FF);
    SX1280HalWriteCommand(RADIO_SET_DIOIRQPARAMS, buf, 8);
}

/*******************************************
*Name:SX1280HalWriteCommand
*Description:SX1280������д
*Input:�����֣���ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalReadCommand(RadioCommands_t command, uint8_t *buffer,
                          uint16_t size)
{
    uint16_t i = 0;
    SX1280HalWaitOnBusy();
    SX1280_CS_SET_LOW(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280_WRITEBYTE((uint8_t)command);
    SX1280_WRITEBYTE(0x00);
    for(i = 0; i < size; i++)
    {
        buffer[i] = SX1280_WRITEBYTE(0x00);
    }
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280HalWaitOnBusy();
}


/*******************************************
*Name:SX1280GetStatus
*Description:1280��ȡ״̬
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
RadioStatus_t SX1280GetStatus(void)
{
    uint8_t stat = 0;
    RadioStatus_t status;
    SX1280HalReadCommand(RADIO_GET_STATUS, (uint8_t*)&stat, 1);
    status.Value = stat;
    return status;
}

/*******************************************
*Name:SX1280GetPacketStatus
*Description:��ȡSX1280���ݰ�״̬
*Input:���ݰ�״̬������ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280GetPacketStatus(PacketStatus_t *pktStatus)
{
    uint8_t status[5];
    SX1280HalReadCommand(RADIO_GET_PACKETSTATUS, status, 5);
    pktStatus->packetType = SX1280GetPacketType();
    switch(pktStatus->packetType)
    {
        case PACKET_TYPE_GFSK:
            pktStatus->Params.Gfsk.RssiAvg = -status[0] / 2;
            pktStatus->Params.Gfsk.RssiSync = -status[1] / 2;
            pktStatus->Params.Gfsk.ErrorStatus.SyncError = ( status[2] >> 6 ) & 0x01;
            pktStatus->Params.Gfsk.ErrorStatus.LengthError = ( status[2] >> 5 ) & 0x01;
            pktStatus->Params.Gfsk.ErrorStatus.CrcError = ( status[2] >> 4 ) & 0x01;
            pktStatus->Params.Gfsk.ErrorStatus.AbortError = ( status[2] >> 3 ) & 0x01;
            pktStatus->Params.Gfsk.ErrorStatus.HeaderReceived = ( status[2] >> 2 ) & 0x01;
            pktStatus->Params.Gfsk.ErrorStatus.PacketReceived = ( status[2] >> 1 ) & 0x01;
            pktStatus->Params.Gfsk.ErrorStatus.PacketControlerBusy = status[2] & 0x01;
            pktStatus->Params.Gfsk.TxRxStatus.RxNoAck = ( status[3] >> 5 ) & 0x01;
            pktStatus->Params.Gfsk.TxRxStatus.PacketSent = status[3] & 0x01;
            pktStatus->Params.Gfsk.SyncAddrStatus = status[4] & 0x07;
            break;
        case PACKET_TYPE_NONE:
            //In that specific case, we set everything in the pktStatus to zeros
            //and reset the packet type accordingly
            memset(pktStatus, 0, sizeof( PacketStatus_t));
            pktStatus->packetType = PACKET_TYPE_NONE;
            break;
    }
}

/*******************************************
*Name:SX1280ClearIrqStatus
*Description:���IRQ�ж�״̬
*Input:ͨ����
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280ClearIrqStatus(uint16_t irq)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(((uint16_t)irq >> 8) & 0x00FF);
    buf[1] = (uint8_t)((uint16_t)irq & 0x00FF);
    SX1280HalWriteCommand(RADIO_CLR_IRQSTATUS, buf, 2);
}

/*******************************************
*Name:SX1280SetRangingRole
*Description:������ӻ�����
*Input:��ɫ��ͨ����
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetRangingRole(RadioRangingRoles_t role)
{
    uint8_t buf[1];
    buf[0] = role;
    SX1280HalWriteCommand(RADIO_SET_RANGING_ROLE, &buf[0], 1);
}


/*******************************************
*Name:SX1280SetRx
*Description:1280���ý���
*Input:��ʱ������ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetRx(TickTime_t timeout)
{
    uint8_t buf[3];
    buf[0] = timeout.Step;
    buf[1] = (uint8_t)((timeout.NbSteps >> 8) & 0x00FF);
    buf[2] = (uint8_t)(timeout.NbSteps & 0x00FF);
    SX1280ClearIrqStatus(IRQ_RADIO_ALL);
    //If the radio is doing ranging operations, then apply the specific calls
    //prior to SetRx
    if(SX1280GetPacketType() == PACKET_TYPE_RANGING)
    {
        SX1280SetRangingRole(RADIO_RANGING_ROLE_SLAVE);
    }
    SX1280HalWriteCommand(RADIO_SET_RX, buf, 3);
    OperatingMode = MODE_RX;
}

/*******************************************
*Name:SX1280HalWriteBuffer
*Description:SX1280д����
*Input:ƫ���������ݵ�ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalWriteBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    uint16_t i = 0;
    SX1280HalWaitOnBusy();
    SX1280_CS_SET_LOW(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280_WRITEBYTE(RADIO_WRITE_BUFFER);
    SX1280_WRITEBYTE(offset);
    for(i = 0; i < size; i++)
    {
        SX1280_WRITEBYTE(buffer[i]);
    }
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280HalWaitOnBusy();
}


/*******************************************
*Name:SX1280SetPayload
*Description:д�뷢������
*Input:����ָ�룬���ȣ�ͨ����
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetPayload(uint8_t *buffer, uint8_t size)
{
    SX1280HalWriteBuffer(0x00, buffer, size);
}

/*******************************************
*Name:SX1280SetTx
*Description:1280���÷���
*Input:��ʱ������ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SetTx(TickTime_t timeout)
{
    uint8_t  buf[3];
    buf[0] = timeout.Step;
    buf[1] = (uint8_t)((timeout.NbSteps >> 8) & 0x00FF);
    buf[2] = (uint8_t)(timeout.NbSteps & 0x00FF);
    SX1280ClearIrqStatus(IRQ_RADIO_ALL);
    // prior to SetTx
    if(SX1280GetPacketType() == PACKET_TYPE_RANGING)
    {
        SX1280SetRangingRole(RADIO_RANGING_ROLE_MASTER);
    }
    SX1280HalWriteCommand(RADIO_SET_TX, buf, 3);
    OperatingMode = MODE_TX;
}


/*******************************************
*Name:SX1280SendPayload
*Description:���ͻ������е�����
*Input:���ݣ���ʱʱ�䣬ͨ����
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280SendPayload(uint8_t * payload, uint8_t size, TickTime_t timeout)
{
    packetParams.Params.Gfsk.PayloadLength = size;
    SX1280SetPayload(payload, size);
    SX1280SetPacketParams(&packetParams);
    SX1280SetTx(timeout);
}

/*******************************************
*Name:SX1280GetRxBufferStatus
*Description:����1280���ջ���״̬
*Input:���ݳ��ȣ�������ʼָ���ַ��ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280GetRxBufferStatus(uint8_t *payloadLength,
                             uint8_t *rxStartBufferPointer)
{
    uint8_t status[2];
    SX1280HalReadCommand(RADIO_GET_RXBUFFERSTATUS, status, 2);
    //In case of LORA fixed header, the payloadLength is obtained by reading
    //the register REG_LR_PAYLOADLENGTH
    if((SX1280GetPacketType() == PACKET_TYPE_LORA) &&
       (SX1280HalReadRegister(REG_LR_PACKETPARAMS) >> 7 == 1))
    {
        *payloadLength = SX1280HalReadRegister(REG_LR_PAYLOADLENGTH);
    }
    else
    {
        *payloadLength = status[0];
    }
    *rxStartBufferPointer = status[1];
}

/*******************************************
*Name:SX1280HalReadBuffer
*Description:SX1280��ȡ����
*Input:ƫ���������ݵ�ַ�����ȣ�ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280HalReadBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    uint16_t i = 0;
    SX1280HalWaitOnBusy();
    SX1280_CS_SET_LOW(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280_WRITEBYTE(RADIO_READ_BUFFER);
    SX1280_WRITEBYTE(offset);
    SX1280_WRITEBYTE(0);
    for(i = 0; i < size; i++)
    {
        buffer[i] = SX1280_WRITEBYTE(0);
    }
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    SX1280HalWaitOnBusy();
}


/*******************************************
*Name:SX1280GetPayload
*Description:��ȡ���ݻ������е�����
*Input:����ָ�룬���ȣ�ͨ����
*Output:״̬
*Author:
*Date:
********************************************/
uint8_t SX1280GetPayload(uint8_t *buffer, uint8_t *size, uint8_t maxSize)
{
    uint8_t offset;
    SX1280GetRxBufferStatus(size, &offset);
    if(*size > maxSize)
    {
        return 1;
    }
    SX1280HalReadBuffer(offset, buffer, *size);
    return 0;
}


/*******************************************
*Name:Wait_TxComplete
*Description: �ȴ������귿
*Input:NULL
*Output:NULL
*Author:
*Date:
********************************************/
void Wait_TxComplete(uint16_t ucTimeOut)
{
    uint16_t ucI;
    for(ucI = 0; ucI < ucTimeOut; ucI++)
    {
        DelayMs(1);
        if(G_SX1280_CFG.txDoneFlag != 0)
        {
            //G_SX1280_CFG.txDoneFlag = 0;
            break;
        }
    }
}


/*******************************************
*Name:SX1280_INIT
*Description: SX1280��ʼ��
*Input:1280����,��Ƶģ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280_INIT()
{
    ModulationParams_t modulationParams;
    modulationParams.PacketType = G_tSx1280RegCfgInfo.PacketType;
    modulationParams.Params.Gfsk.BitrateBandwidth =
        G_tSx1280RegCfgInfo.BitrateBandwidth;
    modulationParams.Params.Gfsk.ModulationIndex =
        G_tSx1280RegCfgInfo.ModulationIndex;
    modulationParams.Params.Gfsk.ModulationShaping =
        G_tSx1280RegCfgInfo.ModulationShaping;

    packetParams.PacketType = G_tSx1280RegCfgInfo.PacketType;
    packetParams.Params.Gfsk.PreambleLength = G_tSx1280RegCfgInfo.PreambleLength;
    packetParams.Params.Gfsk.SyncWordLength = G_tSx1280RegCfgInfo.SyncWordLength;
    packetParams.Params.Gfsk.SyncWordMatch = G_tSx1280RegCfgInfo.SyncWordMatch;
    packetParams.Params.Gfsk.HeaderType    = G_tSx1280RegCfgInfo.HeaderType;
    packetParams.Params.Gfsk.PayloadLength = G_tSx1280RegCfgInfo.PayloadLength;
    packetParams.Params.Gfsk.CrcLength = G_tSx1280RegCfgInfo.CrcLength;
    packetParams.Params.Gfsk.Whitening = G_tSx1280RegCfgInfo.Whitening;




    SX1280HalInit();                                                              //���ûص�����
    SX1280SetRegulatorMode(
        G_tSx1280RegCfgInfo.USE_MODE);                                //���õ�ѹ������ģ��
    G_SXDevVer =
        SX1280GetFirmwareVersion();                                      //��ȡ�汾��Ϣ
    SX1280SetStandby(
        G_tSx1280RegCfgInfo.StandByMode);                                   //����ģʽ���漰�ٶȼ�����
    SX1280SetPacketType(
        modulationParams.PacketType);                             //���������Z
    SX1280SetModulationParams(
        &modulationParams);                                 //���Ʋ�������
    SX1280SetPacketParams(
        &packetParams);                                         //���Ͱ���������
    SX1280SetRfFrequency(
        G_tSx1280RegCfgInfo.RF_FREQUENCY);                              //��ƵƵ��
    SX1280SetBufferBaseAddresses(0x00,
                                 0x00);                                     //�շ�����ַ
    SX1280SetTxParams(G_tSx1280RegCfgInfo.TX_OUTPUT_POWER,
                      G_tSx1280RegCfgInfo.RADIO_RAMP_TIME); //���÷��ͲΔ�
    SX1280SetSyncWord(1,
                      SYNC_WORD);                                            //ͬ���ּ�ͬ��ͨ�����ã�ͨ��������Ϊ1-3��ͬ����ѡ�õ���Ƶ��
    SX1280SetDioIrqParams(G_tSx1280RegCfgInfo.irqMask, G_tSx1280RegCfgInfo.dio1Mask,
                          G_tSx1280RegCfgInfo.dio2Mask, G_tSx1280RegCfgInfo.dio3Mask); //�ж�����
    G_RFStatus =
        SX1280GetStatus();                                               //SX1280��ƵоƬ��״��
    SX1280GetPacketStatus(
        &G_PacketStatus);                                       //SX1280�����ݰ�״��
    SX1280SetRx(
        G_tSx1280RegCfgInfo.RXTimeOut);                                         //����1280Ϊ����״��
    if((G_RFStatus.Fields.ChipMode == 0x02) && (G_RFStatus.Value == 0x43))
    {
        // printf("SX1280��Ƶ����%d��ʼ��������\n",ucChan);
    }
    else
    {
        // printf("SX1280��Ƶ����%d��ʼ������\n",ucChan);
    }
}


/*******************************************
*Name:RF_RxProcess
*Description:��Ƶ���պ���
*Input:��д������
*Output:NULL
*Author:
*Date:
********************************************/
void RF_RxProcess(pWRCfg pWRCfgInfo)
{
    //��ǰ��������������Ȳ�һ�£���ʾ��δ����������
    if(pWRCfgInfo->CirBuff.RFDealCnt != pWRCfgInfo->CirBuff.RFRxCnt)
    {
        //��λ���������ݵı�����־���������ݱ��ж�ˢ��
        pWRCfgInfo->CirBuff.RFRxResvFlag[pWRCfgInfo->CirBuff.RFDealCnt] = 0x5A;
        //�ڴ˴��������ݵĴ�������
        pWRCfgInfo->CirBuff.RFRxBuffer[0];
        pWRCfgInfo->CirBuff.RFDealCnt++;
        //�������������������ۼ�
        if(pWRCfgInfo->CirBuff.RFDealCnt >= RFRxMaxCir)
        {
            pWRCfgInfo->CirBuff.RFDealCnt = 0;
        }
        pWRCfgInfo->CirBuff.RFRxResvFlag[pWRCfgInfo->CirBuff.RFDealCnt] =
            0;     //���㱣����־
    }
}


/*******************************************
*Name:SPI_INIT
*Description:SPI�ⲿ�ӿڳ�ʼ��
*Input:NULL
*Output:NULL
*Author:
*Date:
********************************************/
void SPI_INIT()
{
    CLK_PeripheralClockConfig(G_SX1280_CFG.SX1280_SPI_CLK_Peripheral, ENABLE);
    SPI_DeInit(G_SX1280_CFG.SX1280_SPI_BASE);
    /* SPI Configuration --------------------------------------------------------*/
    SPI_Init(G_SX1280_CFG.SX1280_SPI_BASE,  G_SX1280_CFG.SPI_FirstBit,
             G_SX1280_CFG.SPI_BaudRatePrescaler,
             G_SX1280_CFG.SPI_Mode, G_SX1280_CFG.SPI_CPOL,
             G_SX1280_CFG.SPI_CPHA, G_SX1280_CFG.SPI_Data_Direction,
             G_SX1280_CFG.SPI_Slave_Management, G_SX1280_CFG.CRCPolynomial);
    SPI_Cmd(G_SX1280_CFG.SX1280_SPI_BASE, ENABLE);
}

/*******************************************
*Name:SX1280JK_IRQ_ENBLE
*Description:SX1280�жϳ�ʼ��
*Input:��Ƶͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280JK_IRQ_ENBLE()
{
    //GPIOD4����Ϊ�����ж�ʹ�v
    GPIO_Init(G_SX1280_CFG.MCU_DRDY_GPIO_BASE, G_SX1280_CFG.MCU_DRDY_GPIO_Pin,
              GPIO_Mode_In_FL_IT);
    EXTI_ClearITPendingBit(G_SX1280_CFG.MCU_DRDY_EXTI_BASE);
    //�ж�4Ϊ�����ص͵�ƽ����
    EXTI_SetPinSensitivity(G_SX1280_CFG.MCU_DRDY_EXTI_Pin, EXTI_Trigger_Rising);
}


/*******************************************
*Name:SX1280JK_INIT
*Description: SX1280�ӿڳ�ʼ��
*Input:��Ƶģ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280JK_INIT()
{
    //PWR
    G_SX1280_CFG.PRW_GPIO_BASE->DDR |= G_SX1280_CFG.PRW_GPIO_Pin;
    G_SX1280_CFG.PRW_GPIO_BASE->CR1 |= G_SX1280_CFG.PRW_GPIO_Pin;

    //CS
    G_SX1280_CFG.SPI_EN_GPIO_BASE->DDR |= G_SX1280_CFG.SPI_EN_GPIO_Pin;
    G_SX1280_CFG.SPI_EN_GPIO_BASE->CR1 |= G_SX1280_CFG.SPI_EN_GPIO_Pin;
    //RST
    G_SX1280_CFG.SX1280_RESET_GPIO_BASE->DDR |=
        G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin;
    G_SX1280_CFG.SX1280_RESET_GPIO_BASE->CR1 |=
        G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin;
    //BUSY
    G_SX1280_CFG.SX1280_BUSY_GPIO_BASE->DDR &=  ~
            G_SX1280_CFG.SX1280_BUSY_EN_GPIO_Pin;
    G_SX1280_CFG.SX1280_BUSY_GPIO_BASE->CR1 &=  ~
            G_SX1280_CFG.SX1280_BUSY_EN_GPIO_Pin;
    //IRQ
    G_SX1280_CFG.MCU_DRDY_GPIO_BASE->DDR &=  ~G_SX1280_CFG.MCU_DRDY_GPIO_Pin;
    G_SX1280_CFG.MCU_DRDY_GPIO_BASE->CR1 &=  ~G_SX1280_CFG.MCU_DRDY_GPIO_Pin;

    //CS1����
    SX1280_CS_SET_HIGH(G_SX1280_CFG.SPI_EN_GPIO_BASE, G_SX1280_CFG.SPI_EN_GPIO_Pin);
    //PWR����
    SX1280_PRW_SET_HIGH(G_SX1280_CFG.PRW_GPIO_BASE, G_SX1280_CFG.PRW_GPIO_Pin);
    //RST����
    SX1280_RESET_SET_LOW(G_SX1280_CFG.SX1280_RESET_GPIO_BASE,
                         G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin);

    SPI_INIT();
}

/*******************************************
*Name:SX1280GetOpMode
*Description:1280��ȡ����ģʽ
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
RadioOperatingModes_t SX1280GetOpMode(void)
{
    return OperatingMode;
}

/*******************************************
*Name:SX1280GetIrqStatus
*Description:��ȡIRQ�ж�״̬
*Input:ͨ����
*Output:NULL
*Author:
*Date:
********************************************/
uint16_t SX1280GetIrqStatus()
{
    uint8_t irqStatus[2];
    SX1280HalReadCommand(RADIO_GET_IRQSTATUS, irqStatus, 2);
    return (irqStatus[0] << 8) | irqStatus[1];
}

/*******************************************
*Name:SX1280GetPacketStatus
*Description:��ȡSX1280��RSSIֵ
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
int8_t SX1280GetRssiInst(void)
{
    uint8_t raw = 0;
    SX1280HalReadCommand(RADIO_GET_RSSIINST, &raw, 1);
    return (int8_t)(-raw / 2);
}


/*******************************************
*Name:SX1280_TxDone
*Description: SX1280���ͽ��
*Input:NULL
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280_GetData(pWRCfg pWRCfgInfo)
{
    uint8_t buffer[255] = {0};
    uint8_t bufsize = 0;
    SX1280GetPayload(buffer, &bufsize, 255);
    QUEUE_PacketIn(&SX1280_RX_QUEUE, buffer, bufsize);

}


/*******************************************
*Name:SX1280_RxDone
*Description: SX1280������ɣ��ú������ж��е���
*Input:NULL
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280_RxDone(void)
{
    SX1280_GetData(&G_WRCfgInfo);
}


/*******************************************
*Name:SX1280_TxDone
*Description: SX1280���ͽ������ú������ж��е��m
*Input:NULL
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280_TxDone(void)
{
    G_SX1280_CFG.txDoneFlag = 0x5A;
}

/*******************************************
*Name:SX1280_TxTimeout
*Description: SX1280���ͳ�ʱ���ú������ж��е��m
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280_TxTimeout(void)
{
    //printf("SX1280��Ƶģ��:The Tx Of SX1280 Is TimeOut!\n");
}

/*******************************************
*Name:SX1280_RxTimeout
*Description: SX1280���ճ�ʱ���ú������ж��е���
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280_RxTimeout(void)
{
    //printf("SX1280��Ƶģ��:The Rx Of SX1280 Is TimeOut!\n");
}


/*******************************************
*Name:SX1280ProcessIrqs
*Description:1280�жϴ�������
*Input:ͨ��
*Output:NULL
*Author:
*Date:
********************************************/
void SX1280ProcessIrqs(void)
{
    uint16_t irqRegs = 0;
    RadioPacketTypes_t packetType = PACKET_TYPE_NONE;
    //�����˯��ģʽ
    if(SX1280GetOpMode() == MODE_SLEEP)
    {
        return;
    }
    //��ȡ�����͡��ж�״̬������ж�״̬
    packetType = SX1280GetPacketType();
    irqRegs = SX1280GetIrqStatus();
    SX1280ClearIrqStatus(IRQ_RADIO_ALL);
    switch(packetType)
    {
        case PACKET_TYPE_GFSK:
            switch(OperatingMode)
            {
                case MODE_RX:
                    if((irqRegs & IRQ_RX_DONE) == IRQ_RX_DONE)
                    {
                        SX1280_RxDone();
                    }
                    if((irqRegs & IRQ_RX_TX_TIMEOUT) == IRQ_RX_TX_TIMEOUT)
                    {
                        SX1280_RxTimeout();
                    }
                    break;
                case MODE_TX:
                    if((irqRegs & IRQ_TX_DONE) == IRQ_TX_DONE)
                    {
                        SX1280_TxDone();
                    }
                    if((irqRegs & IRQ_RX_TX_TIMEOUT) == IRQ_RX_TX_TIMEOUT)
                    {
                        SX1280_TxTimeout();
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            // Unexpected IRQ: silently returns
            break;
    }

}


/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
    SX1280ProcessIrqs();
    EXTI_ClearITPendingBit(EXTI_IT_Pin4);
}


/*******************************************************************************
* Function Name :Drives_SX1280DeepSleep()
* Description   :SX1280 ����
* Author        :
* Input         : SX1280_GPIO_TypeDef SX1280_GPIO
* Output        :
* Other         :
* Date          :
*******************************************************************************/
void Drives_SX1280DeepSleep(void)
{
    SX1280_PWR_OFF(G_SX1280_CFG);
    SX1280_RESET_OFF(G_SX1280_CFG);
    SX1280_SPI_EN_OFF(G_SX1280_CFG);
}

/*******************************************************************************
* Function Name :void Drivers_SX1280WakeUp()
* Description   :SX1280 ����
* Author        :
* Input         : SX1280_GPIO_TypeDef SX1280_GPIO
* Output        :
* Other         :
* Date          :
*******************************************************************************/
void Drivers_SX1280WakeUp(void)
{
    SX1280_PWR_ON(G_SX1280_CFG);
    SX1280_RESET_ON(G_SX1280_CFG);
    SX1280_SPI_EN_ON(G_SX1280_CFG);

}

/*******************************************************************************
* Function Name :void Drivers_SX1280Init(void)
* Description   :SX1280 ��ʼ��
* Author        :
* Input         :
* Output        :
* Other         :
* Date          :
*******************************************************************************/
void Drivers_SX1280Init(void)
{
    //��ʼ��SX1280;
    //SPI����
    G_SX1280_CFG.SX1280_SPI_BASE = SPI1;
    G_SX1280_CFG.SPI_FirstBit = SPI_FirstBit_MSB;
    G_SX1280_CFG.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    G_SX1280_CFG.SPI_Mode = SPI_Mode_Master;
    G_SX1280_CFG.SPI_CPOL = SPI_CPOL_High;
    G_SX1280_CFG.SPI_CPHA = SPI_CPHA_2Edge;
    G_SX1280_CFG.SPI_Data_Direction = SPI_Direction_2Lines_FullDuplex;
    G_SX1280_CFG.SPI_Slave_Management = SPI_NSS_Soft;
    G_SX1280_CFG.CRCPolynomial = (uint8_t)0x07;
    G_SX1280_CFG.SX1280_SPI_CLK_Peripheral = CLK_Peripheral_SPI1;
    //PWR
    G_SX1280_CFG.PRW_GPIO_BASE = GPIOE;
    G_SX1280_CFG.PRW_GPIO_Pin = GPIO_Pin_0;
    //SPI CS   -EN
    G_SX1280_CFG.SPI_EN_GPIO_BASE = GPIOB;
    G_SX1280_CFG.SPI_EN_GPIO_Pin = GPIO_Pin_3;
    //SX1280 RST
    G_SX1280_CFG.SX1280_RESET_GPIO_BASE = GPIOD;
    G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin = GPIO_Pin_7;
    //SX1280 BUSY
    G_SX1280_CFG.SX1280_BUSY_GPIO_BASE = GPIOD;
    G_SX1280_CFG.SX1280_BUSY_EN_GPIO_Pin = GPIO_Pin_5;
    //IRQ
    G_SX1280_CFG.MCU_DRDY_GPIO_BASE = GPIOD;
    G_SX1280_CFG.MCU_DRDY_GPIO_Pin = GPIO_Pin_4;
    G_SX1280_CFG.MCU_DRDY_EXTI_Pin = EXTI_Pin_4;
    G_SX1280_CFG.MCU_DRDY_EXTI_BASE = EXTI_IT_Pin4;

    //���ݽ��ջ���
    G_SX1280_CFG.sx1280RxBuf = SX1280_RX_BUFF;
    G_SX1280_CFG.SX1280_RX_BUF_SIZE = DRIVERS_SX1280_RX_BUFF_SIZE;
    disableInterrupts();                      //��ȫ���ж�
    Sx1280_PwrCfgInfo();                      //SX1280��ز����趨
    SX1280JK_INIT();                          //1280�ӿڳ�ʼ��
    SX1280JK_IRQ_ENBLE();                     //�ж�ʹ��
    SX1280_INIT();                            //SX1280��ʼ��
    enableInterrupts();                       //ȫ���ж�ʹ��
#if SX1280_TEST_MODE || UART_DEBUG   //���Ҫ����1280����,������Ҫ���Դ������ʼ������
    SX1280_TEST_UART.USARTNum = USART1;
    SX1280_TEST_UART.CLKPeripheralNum = CLK_Peripheral_USART1;
    SX1280_TEST_UART.UART_TX_GPIO_BASE = GPIOA;
    SX1280_TEST_UART.UART_TX_GPIO_Pin  = GPIO_Pin_2;
    SX1280_TEST_UART.UART_RX_GPIO_BASE = GPIOA;
    SX1280_TEST_UART.UART_RX_GPIO_Pin  = GPIO_Pin_3;
    SX1280_TEST_UART.baud = 115200;
    SX1280_TEST_UART.data = 8;
    SX1280_TEST_UART.stop = 1;
    SX1280_TEST_UART.parity = 0;
    BSP_UartOpen(&SX1280_TEST_UART);
#endif
    QUEUE_PacketCreate(&SX1280_RX_QUEUE, G_SX1280_CFG.sx1280RxBuf,
                       G_SX1280_CFG.SX1280_RX_BUF_SIZE);
}

/*******************************************************************************
* Function Name :void Drivers_SX1280SendBuff(uint8_t *buffter)
* Description   :SX1280 ��������
* Author        :
* Input         : uint8_t *buffter
* Output        :
* Other         :
* Date          :
*******************************************************************************/
void Drivers_SX1280SendBuff(uint8_t * sendBuf, uint8_t bufSize)
{
    //��Ƶ��������
    G_tSx1280RegCfgInfo .irqMask = IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT;
    G_tSx1280RegCfgInfo .dio1Mask = IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT;
    SX1280SetDioIrqParams(G_tSx1280RegCfgInfo .irqMask,
                          G_tSx1280RegCfgInfo .dio1Mask, G_tSx1280RegCfgInfo .dio2Mask,
                          G_tSx1280RegCfgInfo .dio3Mask);//�ж�����
    SX1280SendPayload(sendBuf, bufSize, (TickTime_t)
    {
        RADIO_TICK_SIZE_1000_US, 500
    });
    Wait_TxComplete(500);
    /*
    //���ݷ�����ɺ��Զ���Ϊ����ģʽ
    G_tSx1280RegCfgInfo .irqMask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT;
    G_tSx1280RegCfgInfo .dio1Mask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT;
    SX1280SetDioIrqParams(G_tSx1280RegCfgInfo .irqMask,
                          G_tSx1280RegCfgInfo .dio1Mask, G_tSx1280RegCfgInfo .dio2Mask,
                          G_tSx1280RegCfgInfo .dio3Mask);//�ж�����
    G_RFStatus = SX1280GetStatus();   //SX1280��ƵоƬ��״��
    SX1280GetPacketStatus(&G_PacketStatus);     //SX1280�����ݰ�״��
    SX1280SetRx(G_tSx1280RegCfgInfo .RXTimeOut);//
    */

}

/*******************************************************************************
* Function Name :void Drivers_SX1280ReadBuff(uint8_t *buffter, uint32_t len)
* Description   :SX1280 ��ȡ����
* Author        :
* Input         : uint8_t *buffter, uint32_t len
* Output        :
* Other         :
* Date          :
*******************************************************************************/
uint32_t Drivers_SX1280ReadBuff(uint8_t *buffter, uint32_t len)
{
    return QUEUE_PacketOut(&SX1280_RX_QUEUE, buffter, len);
}
/*******************************************************************************
* Function Name :void Drivers_SX1280ParamModify(uint8_t *bufffer)
* Description   :�޸�SX1280���� ���ݴ���/���书��/����ͨ��/��Ƶ����
* Author        :
* Input         : uint8_t *buffter
* Output        :
* Other         :
* Date          :
*******************************************************************************/

void Drivers_SX1280ParamModify(uint8_t *bufffer)
{

    uint8_t m_type_buf[3] = {0};
    uint8_t m_val_buf[5] = {0};
    static uint32_t m_type = 0;   //ѡ���޸ĵ�����
    static uint32_t m_val = 0;    //�޸ĵ�ֵ
    char *delim = ",";
    char *p;
    //sscanf(bufffer, "%d,%d", &m_type, &m_val);
    strcpy(m_type_buf, strtok(bufffer, delim));
    while((p = strtok(NULL, delim)))
        strcpy(m_val_buf, p);
    m_type = atoi(m_type, m_type_buf);
    m_val = atoi(m_val, m_val_buf);
    nop();
    switch(m_type)
    {
        case 0:   //�޸����ݴ���
            switch(m_val)
            {

                case 0:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_2_000_BW_2_4;
                    break;
                case 1:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_1_600_BW_2_4;
                    break;
                case 2:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_1_000_BW_2_4;
                    break;
                case 3:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_1_000_BW_1_2;
                    break;
                case 4:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_800_BW_2_4;
                    break;
                case 5:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_800_BW_1_2;
                    break;
                case 6:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_500_BW_1_2;
                    break;
                case 7:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_500_BW_0_6;
                    break;
                case 8:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_400_BW_1_2;
                    break;
                case 9:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_400_BW_0_6;
                    break;
                case 10:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_250_BW_0_6;
                    break;
                case 11:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_250_BW_0_3;
                    break;
                case 12:
                    G_tSx1280RegCfgInfo .BitrateBandwidth =
                        GFS_BLE_BR_0_125_BW_0_3;
                    break;
                default:
                    break;
            }
            break;
        case 1:   //�޸ĵ���ͨ��
            G_tSx1280RegCfgInfo .ModulationIndex = m_val;
            break;
        case 2:   //�޸ķ��书��
            G_tSx1280RegCfgInfo .TX_OUTPUT_POWER = m_val;
            break;
        case 3:   //�޸ķ���Ƶ��
            G_tSx1280RegCfgInfo .RF_FREQUENCY = m_val * 1000000;
            break;
        default:
            break;
    }
    SX1280_INIT();
}


/*******************************************************************************
* Function Name : void _RXIRQHandler(uint8_t COM)
* Description   :
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.23
*******************************************************************************/
void _RXIRQHandler()
{
    static uint8_t m_Buf[16] = {0};
    static int i = 0;
    volatile unsigned char clear;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t ch = USART1->DR;
        m_Buf[i++] = ch;

    }
    else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        clear = USART1->SR;
        clear = USART1->DR;
        Drivers_SX1280ParamModify(m_Buf);
        memset(m_Buf, 0, sizeof(m_Buf));
        i = 0;
    }
}

INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
    /* In order to detect unexpected events during development,
    it is recommended to set a breakpoint on the following instruction.
    */
    _RXIRQHandler();
}
void Drivers_SX1280PowerOn()
{
	Drivers_SX1280Init();
}
void Drives_SX1280PowerOff()
{
    G_SX1280_CFG.SPI_EN_GPIO_BASE->DDR |= G_SX1280_CFG.SPI_EN_GPIO_Pin;
    G_SX1280_CFG.SPI_EN_GPIO_BASE->ODR &= ~G_SX1280_CFG.SPI_EN_GPIO_Pin;
    G_SX1280_CFG.PRW_GPIO_BASE->DDR |= G_SX1280_CFG.PRW_GPIO_Pin;
    G_SX1280_CFG.PRW_GPIO_BASE->ODR &= ~G_SX1280_CFG.PRW_GPIO_Pin;
    G_SX1280_CFG.SX1280_RESET_GPIO_BASE->DDR |=
        G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin;
    G_SX1280_CFG.SX1280_RESET_GPIO_BASE->ODR &=
        ~G_SX1280_CFG.SX1280_RESET_EN_GPIO_Pin;
    G_SX1280_CFG.SX1280_BUSY_GPIO_BASE->DDR |= G_SX1280_CFG.SX1280_BUSY_EN_GPIO_Pin;
    G_SX1280_CFG.SX1280_BUSY_GPIO_BASE->ODR &=
        ~G_SX1280_CFG.SX1280_BUSY_EN_GPIO_Pin;
    G_SX1280_CFG.MCU_DRDY_GPIO_BASE->DDR |= G_SX1280_CFG.MCU_DRDY_GPIO_Pin;
    G_SX1280_CFG.MCU_DRDY_GPIO_BASE->ODR &= ~G_SX1280_CFG.MCU_DRDY_GPIO_Pin;
}

uint16_t Drivers_SX1280GetMaxTryTimes(void)
{
    return G_SX1280_CFG.maxTryTimes;      //��������Դ���
}
uint8_t Drivers_SX1280GetTxDoneFlag(void)
{
    return G_SX1280_CFG.txDoneFlag;                //������ɱ�־
}
void Drivers_SX1280SetTxDoneFlag(uint8_t txDoneFlag)
{
    G_SX1280_CFG.txDoneFlag = txDoneFlag;
}


















