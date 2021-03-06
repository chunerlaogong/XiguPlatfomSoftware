#ifndef _DRIVERS_SX1280_H_
#define _DRIVERS_SX1280_H_
#include <stm8l15x.h>
#include <math.h>
#include <string.h>
#include "Queue.h"
#include "BSP_UART.h"
#ifndef NULL
#define NULL 0
#endif


typedef enum {true = 0, false = 1} bool;

#define SX1280_CS_SET_HIGH(GPIOx, GPIO_Pin) GPIO_SetBits(GPIOx, GPIO_Pin)
#define SX1280_CS_SET_LOW( GPIOx,  GPIO_Pin) GPIO_ResetBits(GPIOx, GPIO_Pin)

#define SX1280_PRW_SET_HIGH( GPIOx,  GPIO_Pin)   GPIO_SetBits(GPIOx, GPIO_Pin)
#define SX1280_PRW_SET_LOW( GPIOx,  GPIO_Pin)   GPIO_ResetBits(GPIOx, GPIO_Pin)

#define SX1280_RESET_SET_LOW( GPIOx,  GPIO_Pin)   GPIO_ResetBits(GPIOx, GPIO_Pin)
#define SX1280_RESET_SET_HIGH( GPIOx,  GPIO_Pin)   GPIO_SetBits(GPIOx, GPIO_Pin)

#define SX1280_BUSY_SET_LOW(GPIOx,  GPIO_Pin);  GPIO_ResetBits(GPIOx, GPIO_Pin)
#define SX1280_READ_BUSY( GPIOx,  GPIO_Pin)   GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)

#define SX1280_IRQ_SET_LOW(GPIOx,  GPIO_Pin);  GPIO_ResetBits(GPIOx, GPIO_Pin)



/**********************
* brief Provides the frequency of the chip running on the radio and the frequency step
* remark These defines are used for computing the frequency divider to set the RF frequency
**********************/
#define XTAL_FREQ                   52000000
#define FREQ_STEP                   ((double)(XTAL_FREQ/pow(2.0,18.0)))


/**********************
* brief The addresses of the registers holding SyncWords values
* remark The addresses depends on the Packet Type in use, and not all
*  SyncWords are available for every Packet Type
**********************/
#define REG_LR_SYNCWORDBASEADDRESS1     0x09CE
#define REG_LR_SYNCWORDBASEADDRESS2     0x09D3
#define REG_LR_SYNCWORDBASEADDRESS3     0x09D8

/**********************
* brief The address of the register holding payload length
* remark Do NOT try to read it directly. Use GetRxBuffer( ) instead.
**********************/
#define REG_LR_PAYLOADLENGTH            0x901

/**********************
* brief The address of the register holding LORA packet parameters
**********************/
#define REG_LR_PACKETPARAMS             0x903





//定义读写器设备信息
typedef struct
{
    uint32_t     PCDevID;                         //后台PC机的设备ID
    uint32_t     SrcDevID;                        //读写器本机的设备ID
    uint8_t      ChannelNum;                      //射频通道总数量
} sDeviceCfg, *pDeviceCfg;

#define RFRxMaxLen        32                //数据长度
#define RFRxMaxCir        5               //环形缓存深度

//定义读写器收发数据缓存区
typedef struct
{
    uint8_t   RFRxLen;           //射频单次接收的数据长度，0-255
    uint8_t   RFDealCnt;         //处理深度计数
    uint8_t   RFRxCnt;           //接收深度计数
    uint8_t   RFRxBuffer[RFRxMaxCir][RFRxMaxLen]; //数据接收缓存
    uint8_t   RFTxBuffer[RFRxMaxLen];             //数据发送缓存
    uint8_t   RFRxResvFlag[RFRxMaxCir];           //当前数据正在被操作标志，防止读写同时进行导致数据变异
    int8_t    RFRssi[RFRxMaxCir];                 //RSSI值
    uint8_t   RFChan[RFRxMaxCir];                 //当前接收到数据的通道
    uint8_t   RFChan0Cnt;                        //射频通道0接收总计数
    uint8_t   RFChan1Cnt;                         //射频通道1接收总计数
} sRFCirBuff, *pRFCirBuff;


//定义读写器数据结构体
typedef struct
{
    sDeviceCfg    DeviceInfo;                  //设备属性信息
    sRFCirBuff    CirBuff;                     //数据收发属性信息
} sWRCfg, *pWRCfg;





/**********************
* brief Represents the possible packet type (i.e. modem) used
**********************/
typedef enum
{
    PACKET_TYPE_GFSK        = 0x00,
    PACKET_TYPE_LORA,
    PACKET_TYPE_RANGING,
    PACKET_TYPE_FLRC,
    PACKET_TYPE_BLE,
    PACKET_TYPE_NONE        = 0x0F,
} RadioPacketTypes_t;

/**********************
* brief Represents the preamble length values for GFSK and FLRC packet
*        types
**********************/
typedef enum
{
    PREAMBLE_LENGTH_04_BITS                 = 0x00,         //!< Preamble length: 04 bits
    PREAMBLE_LENGTH_08_BITS                 = 0x10,         //!< Preamble length: 08 bits
    PREAMBLE_LENGTH_12_BITS                 = 0x20,         //!< Preamble length: 12 bits
    PREAMBLE_LENGTH_16_BITS                 = 0x30,         //!< Preamble length: 16 bits
    PREAMBLE_LENGTH_20_BITS                 = 0x40,         //!< Preamble length: 20 bits
    PREAMBLE_LENGTH_24_BITS                 = 0x50,         //!< Preamble length: 24 bits
    PREAMBLE_LENGTH_28_BITS                 = 0x60,         //!< Preamble length: 28 bits
    PREAMBLE_LENGTH_32_BITS                 = 0x70,         //!< Preamble length: 32 bits
} RadioPreambleLengths_t;


/**********************
* brief The length of sync words for GFSK packet type
**********************/
typedef enum
{
    GFS_SYNCWORD_LENGTH_1_BYTE              = 0x00,         //!< Sync word length: 1 byte
    GFS_SYNCWORD_LENGTH_2_BYTE              = 0x02,         //!< Sync word length: 2 bytes
    GFS_SYNCWORD_LENGTH_3_BYTE              = 0x04,         //!< Sync word length: 3 bytes
    GFS_SYNCWORD_LENGTH_4_BYTE              = 0x06,         //!< Sync word length: 4 bytes
    GFS_SYNCWORD_LENGTH_5_BYTE              = 0x08,         //!< Sync word length: 5 bytes
} RadioSyncWordLengths_t;


/**********************
* \brief Represents the possible combinations of SyncWord correlators
*        activated for GFSK and FLRC packet types
**********************/
typedef enum
{
    RADIO_RX_MATCH_SYNCWORD_OFF             = 0x00,         //!< No correlator turned on, i.e. do not search for SyncWord
    RADIO_RX_MATCH_SYNCWORD_1               = 0x10,
    RADIO_RX_MATCH_SYNCWORD_2               = 0x20,
    RADIO_RX_MATCH_SYNCWORD_1_2             = 0x30,
    RADIO_RX_MATCH_SYNCWORD_3               = 0x40,
    RADIO_RX_MATCH_SYNCWORD_1_3             = 0x50,
    RADIO_RX_MATCH_SYNCWORD_2_3             = 0x60,
    RADIO_RX_MATCH_SYNCWORD_1_2_3           = 0x70,
} RadioSyncWordRxMatchs_t;





/**********************
* brief Represents the modulation index used in GFSK and BLE packet
*        types
**********************/
typedef enum
{
    GFS_BLE_MOD_IND_0_35       =  0,
    GFS_BLE_MOD_IND_0_50       =  1,
    GFS_BLE_MOD_IND_0_75       =  2,
    GFS_BLE_MOD_IND_1_00       =  3,
    GFS_BLE_MOD_IND_1_25       =  4,
    GFS_BLE_MOD_IND_1_50       =  5,
    GFS_BLE_MOD_IND_1_75       =  6,
    GFS_BLE_MOD_IND_2_00       =  7,
    GFS_BLE_MOD_IND_2_25       =  8,
    GFS_BLE_MOD_IND_2_50       =  9,
    GFS_BLE_MOD_IND_2_75       = 10,
    GFS_BLE_MOD_IND_3_00       = 11,
    GFS_BLE_MOD_IND_3_25       = 12,
    GFS_BLE_MOD_IND_3_50       = 13,
    GFS_BLE_MOD_IND_3_75       = 14,
    GFS_BLE_MOD_IND_4_00       = 15,
} RadioGfskBleModIndexes_t;

/**********************
* brief Represents the modulation shaping parameter for GFSK, FLRC and BLE
*        packet types
**********************/
typedef enum
{
    RADIO_MOD_SHAPING_BT_OFF        = 0x00,         //No filtering
    RADIO_MOD_SHAPING_BT_1_0        = 0x10,
    RADIO_MOD_SHAPING_BT_0_5        = 0x20,
} RadioModShapings_t;



/**********************
*  brief Radio packet length mode for GFSK and FLRC packet types
**********************/
typedef enum
{
    RADIO_PACKET_FIXED_LENGTH               = 0x00,         //!< The packet is known on both sides, no header included in the packet
    RADIO_PACKET_VARIABLE_LENGTH            = 0x20,         //!< The packet is on variable size, header included
} RadioPacketLengthModes_t;

/**********************
* brief Represents the CRC length for GFSK and FLRC packet types
* warning Not all configurations are available for both GFSK and FLRC
* packet type. Refer to the datasheet for possible configuration.
**********************/
typedef enum
{
    RADIO_CRC_OFF                           = 0x00,         //!< No CRC in use
    RADIO_CRC_1_BYTES                       = 0x10,
    RADIO_CRC_2_BYTES                       = 0x20,
    RADIO_CRC_3_BYTES                       = 0x30,
} RadioCrcTypes_t;

/**********************
* brief Radio whitening mode activated or deactivated for GFSK, FLRC and
* BLE packet types
**********************/
typedef enum
{
    RADIO_WHITENING_ON                      = 0x00,
    RADIO_WHITENING_OFF                     = 0x08,
} RadioWhiteningModes_t;



/**********************
* brief Represents the possible combinations of bitrate and bandwidth for
*        GFSK and BLE packet types
* The bitrate is expressed in Mb/s and the bandwidth in MHz
**********************/
typedef enum
{
    GFS_BLE_BR_2_000_BW_2_4     = 0x04,
    GFS_BLE_BR_1_600_BW_2_4     = 0x28,
    GFS_BLE_BR_1_000_BW_2_4     = 0x4C,
    GFS_BLE_BR_1_000_BW_1_2     = 0x45,
    GFS_BLE_BR_0_800_BW_2_4     = 0x70,
    GFS_BLE_BR_0_800_BW_1_2     = 0x69,
    GFS_BLE_BR_0_500_BW_1_2     = 0x8D,
    GFS_BLE_BR_0_500_BW_0_6     = 0x86,
    GFS_BLE_BR_0_400_BW_1_2     = 0xB1,
    GFS_BLE_BR_0_400_BW_0_6     = 0xAA,
    GFS_BLE_BR_0_250_BW_0_6     = 0xCE,
    GFS_BLE_BR_0_250_BW_0_3     = 0xC7,
    GFS_BLE_BR_0_125_BW_0_3     = 0xEF,     //Raw Bit Rate = 0.125Mb/s , band width = 0.3MHZ
} RadioGfskBleBitrates_t;


/**********************
* brief Declares the oscillator in use while in standby mode
* Using the STDBY_RC standby mode allow to reduce the energy consumption
* STDBY_XOSC should be used for time critical applications
**********************/
typedef enum
{
    STDBY_RC           = 0x00,
    STDBY_XOSC         = 0x01,
} RadioStandbyModes_t;

/**********************
* \brief Declares the power regulation used to power the device
* This command allows the user to specify if DC-DC or LDO is used for power regulation.
* Using only LDO implies that the Rx or Tx current is doubled
**********************/
typedef enum
{
    USE_LDO             = 0x00,           // Use LDO (default value)
    USE_DCDC            = 0x01,           // Use DCDC
} RadioRegulatorModes_t;

/**********************
* brief Represents the ramping time for power amplifier
**********************/
typedef enum
{
    RADIO_RAMP_02_US        = 0x00,
    RADIO_RAMP_04_US        = 0x20,
    RADIO_RAMP_06_US        = 0x40,
    RADIO_RAMP_08_US        = 0x60,
    RADIO_RAMP_10_US        = 0x80,
    RADIO_RAMP_12_US        = 0xA0,
    RADIO_RAMP_16_US        = 0xC0,
    RADIO_RAMP_20_US        = 0xE0,
} RadioRampTimes_t;


/**********************
* brief Represents the interruption masks available for the radio
* remark Note that not all these interruptions are available for all packet types
**********************/
typedef enum
{
    IRQ_RADIO_NONE                          = 0x0000,
    IRQ_TX_DONE                             = 0x0001,
    IRQ_RX_DONE                             = 0x0002,
    IRQ_SYNCWORD_VALID                      = 0x0004,
    IRQ_SYNCWORD_ERROR                      = 0x0008,
    IRQ_HEADER_VALID                        = 0x0010,
    IRQ_HEADER_ERROR                        = 0x0020,
    IRQ_CRC_ERROR                           = 0x0040,
    IRQ_RANGING_SLAVE_RESPONSE_DONE         = 0x0080,
    IRQ_RANGING_SLAVE_REQUEST_DISCARDED     = 0x0100,
    IRQ_RANGING_MASTER_RESULT_VALID         = 0x0200,
    IRQ_RANGING_MASTER_RESULT_TIMEOUT       = 0x0400,
    IRQ_RANGING_SLAVE_REQUEST_VALID         = 0x0800,
    IRQ_CAD_DONE                            = 0x1000,
    IRQ_CAD_ACTIVITY_DETECTED               = 0x2000,
    IRQ_RX_TX_TIMEOUT                       = 0x4000,
    IRQ_PREAMBLE_DETECTED                   = 0x8000,
    IRQ_RADIO_ALL                           = 0xFFFF,
} RadioIrqMasks_t;

/**********************
* brief Represents the tick size available for Rx/Tx timeout operations
**********************/
typedef enum
{
    RADIO_TICK_SIZE_0015_US                 = 0x00,
    RADIO_TICK_SIZE_0062_US                 = 0x01,
    RADIO_TICK_SIZE_1000_US                 = 0x02,
    RADIO_TICK_SIZE_4000_US                 = 0x03,
} RadioTickSizes_t;

/**********************
* brief Represents all possible opcode understood by the radio
**********************/
typedef enum RadioCommands_u
{
    RADIO_GET_STATUS                        = 0xC0,
    RADIO_WRITE_REGISTER                    = 0x18,
    RADIO_READ_REGISTER                     = 0x19,
    RADIO_WRITE_BUFFER                      = 0x1A,
    RADIO_READ_BUFFER                       = 0x1B,
    RADIO_SET_SLEEP                         = 0x84,
    RADIO_SET_STANDBY                       = 0x80,
    RADIO_SET_FS                            = 0xC1,
    RADIO_SET_TX                            = 0x83,
    RADIO_SET_RX                            = 0x82,
    RADIO_SET_RXDUTYCYCLE                   = 0x94,
    RADIO_SET_CAD                           = 0xC5,
    RADIO_SET_TXCONTINUOUSWAVE              = 0xD1,
    RADIO_SET_TXCONTINUOUSPREAMBLE          = 0xD2,
    RADIO_SET_PACKETTYPE                    = 0x8A,
    RADIO_GET_PACKETTYPE                    = 0x03,
    RADIO_SET_RFFREQUENCY                   = 0x86,
    RADIO_SET_TXPARAMS                      = 0x8E,
    RADIO_SET_CADPARAMS                     = 0x88,
    RADIO_SET_BUFFERBASEADDRESS             = 0x8F,
    RADIO_SET_MODULATIONPARAMS              = 0x8B,
    RADIO_SET_PACKETPARAMS                  = 0x8C,
    RADIO_GET_RXBUFFERSTATUS                = 0x17,
    RADIO_GET_PACKETSTATUS                  = 0x1D,
    RADIO_GET_RSSIINST                      = 0x1F,
    RADIO_SET_DIOIRQPARAMS                  = 0x8D,
    RADIO_GET_IRQSTATUS                     = 0x15,
    RADIO_CLR_IRQSTATUS                     = 0x97,
    RADIO_CALIBRATE                         = 0x89,
    RADIO_SET_REGULATORMODE                 = 0x96,
    RADIO_SET_SAVECONTEXT                   = 0xD5,
    RADIO_SET_AUTOTX                        = 0x98,
    RADIO_SET_AUTORX                        = 0x9E,
    RADIO_SET_LONGPREAMBLE                  = 0x9B,
    RADIO_SET_UARTSPEED                     = 0x9D,
    RADIO_SET_RANGING_ROLE                  = 0xA3,
} RadioCommands_t;

/**********************
* brief Represents the operating mode the radio is actually running
**********************/
typedef enum
{
    MODE_SLEEP    = 0x00,         // The radio is in sleep mode
    MODE_STDBY_RC,                // The radio is in standby mode with RC oscillator
    MODE_STDBY_XOSC,              // The radio is in standby mode with XOSC oscillator
    MODE_FS,                      // The radio is in frequency synthesis mode
    MODE_TX,                      // The radio is in transmit mode
    MODE_RX,                      // The radio is in receive mode
    MODE_CAD                      // The radio is in channel activity detection mode
} RadioOperatingModes_t;


/**********************
* brief The type describing the modulation parameters for every packet types
**********************/
typedef struct
{
    RadioPacketTypes_t
    PacketType;        //!< Packet to which the modulation parameters are referring to.
    struct
    {
        /**********************
        * brief Holds the GFSK modulation parameters
        * In GFSK modulation, the bit-rate and bandwidth are linked together. In this structure, its values are set using the same token.
        **********************/
        struct
        {
            RadioGfskBleBitrates_t
            BitrateBandwidth;  //!< The bandwidth and bit-rate values for BLE and GFSK modulations
            RadioGfskBleModIndexes_t
            ModulationIndex;   //!< The coding rate for BLE and GFSK modulations
            RadioModShapings_t
            ModulationShaping; //!< The modulation shaping for BLE and GFSK modulations
        } Gfsk;
    } Params;                                                  //!< Holds the modulation parameters structure
} ModulationParams_t;

/**********************
 * brief Represents an amount of time measurable by the radio clock
 * @code
 * Time = Step * NbSteps
 * Example:
 * Step = RADIO_TICK_SIZE_4000_US( 4 ms )
 * NbSteps = 1000
 * Time = 4e-3 * 1000 = 4 seconds
 * @endcode
**********************/
typedef struct TickTime_s
{
    RadioTickSizes_t
    Step;                                  //!< The step of ticktime
    /**********************
    * \brief The number of steps for ticktime
    * Special values are:
    *     - 0x0000 for single mode
    *     - 0xFFFF for continuous mode
    **********************/
    uint16_t NbSteps;
} TickTime_t;

typedef struct
{
    RadioPacketTypes_t               PacketType;         //包类型
    RadioGfskBleBitrates_t           BitrateBandwidth;   //数据带宽 1
    RadioGfskBleModIndexes_t         ModulationIndex;    //调制通道 1 
    RadioModShapings_t               ModulationShaping;  //调制修正
    RadioPreambleLengths_t           PreambleLength;     //帧头长度
    RadioSyncWordLengths_t           SyncWordLength;     //同步字长度
    RadioSyncWordRxMatchs_t          SyncWordMatch;      //同步匹配字
    RadioPacketLengthModes_t         HeaderType;         //头类型
    uint8_t                          PayloadLength;      //数据区长度
    RadioCrcTypes_t                  CrcLength;          //CRC长度
    RadioWhiteningModes_t            Whitening;          //Whitening状态

    RadioStandbyModes_t             StandByMode;           //唤醒功耗模式
    RadioRegulatorModes_t           USE_MODE;              //供电模式
    uint32_t                        RF_FREQUENCY;          //射频频率 1
    uint8_t                         TX_OUTPUT_POWER;       //发射功率   1
    RadioRampTimes_t                RADIO_RAMP_TIME;       //建立时间
    uint16_t                        irqMask;               //中断屏蔽寄存器
    uint16_t                        dio1Mask;              //IO1寄存器
    uint16_t                        dio2Mask;              //IO2寄存器
    uint16_t                        dio3Mask;              //IO3寄存器
    TickTime_t                      RXTimeOut;             //接收超时

} sSx1280RegCfg, *Sx1280RegCfg;


/**********************
* brief The type describing the packet parameters for every packet types
**********************/
typedef struct
{
    RadioPacketTypes_t
    PacketType;        //!< Packet to which the packet parameters are referring to.
    struct
    {
        /**********************
        * \brief Holds the GFSK packet parameters
        **********************/
        struct
        {
            RadioPreambleLengths_t
            PreambleLength;    //!< The preamble length for GFSK packet type
            RadioSyncWordLengths_t
            SyncWordLength;    //!< The synchronization word length for GFSK packet type
            RadioSyncWordRxMatchs_t
            SyncWordMatch;     //!< The synchronization correlator to use to check synchronization word
            RadioPacketLengthModes_t
            HeaderType;        //!< If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted
            uint8_t
            PayloadLength;     //!< Size of the payload in the GFSK packet
            RadioCrcTypes_t
            CrcLength;         //!< Size of the CRC block in the GFSK packet
            RadioWhiteningModes_t
            Whitening;         //!< Usage of whitening on payload and CRC blocks plus header block if header type is variable
        } Gfsk;
    } Params;                                                //!< Holds the packet parameters structure
} PacketParams_t;


/**********************
* brief Structure describing the radio status
**********************/
typedef union
{
    struct  //brief Structure of the radio status
    {
        uint8_t CpuBusy   : 1;  //Flag for CPU radio busy
        uint8_t DmaBusy   : 1;  //Flag for DMA busy
        uint8_t CmdStatus : 3;  //Command status
        uint8_t ChipMode  : 3;  //Chip mode
    } Fields;
    uint8_t Value;//brief Serialized radio status
} RadioStatus_t;

/**********************
* brief Represents the role of the radio during ranging operations
**********************/
typedef enum
{
    RADIO_RANGING_ROLE_SLAVE                = 0x00,
    RADIO_RANGING_ROLE_MASTER               = 0x01,
} RadioRangingRoles_t;


/**********************
* brief Represents the packet status for every packet type
**********************/
typedef struct
{
    RadioPacketTypes_t
    packetType;        //!< Packet to which the packet status are referring to.
    union
    {
        struct
        {
            int8_t RssiAvg;                                 //!< The averaged RSSI
            int8_t RssiSync;                                //!< The RSSI measured on last packet
            struct
            {
                bool SyncError : 1;                         //!< SyncWord error on last packet
                bool LengthError : 1;                       //!< Length error on last packet
                bool CrcError : 1;                          //!< CRC error on last packet
                bool AbortError : 1;                        //!< Abort error on last packet
                bool HeaderReceived : 1;                    //!< Header received on last packet
                bool PacketReceived : 1;                    //!< Packet received
                bool PacketControlerBusy : 1;               //!< Packet controller busy
            } ErrorStatus;                                  //!< The error status Byte
            struct
            {
bool RxNoAck :
                1;                           //!< No acknowledgment received for Rx with variable length packets
bool PacketSent :
                1;                        //!< Packet sent, only relevant in Tx mode
            } TxRxStatus;                                   //!< The Tx/Rx status Byte
uint8_t SyncAddrStatus :
            3;                     //!< The id of the correlator who found the packet
        } Gfsk;
        struct
        {
            int8_t RssiPkt;                                 //!< The RSSI of the last packet
            int8_t SnrPkt;                                  //!< The SNR of the last packet
            struct
            {
                bool SyncError : 1;                         //!< SyncWord error on last packet
                bool LengthError : 1;                       //!< Length error on last packet
                bool CrcError : 1;                          //!< CRC error on last packet
                bool AbortError : 1;                        //!< Abort error on last packet
                bool HeaderReceived : 1;                    //!< Header received on last packet
                bool PacketReceived : 1;                    //!< Packet received
                bool PacketControlerBusy : 1;               //!< Packet controller busy
            } ErrorStatus;                                  //!< The error status Byte
            struct
            {
bool RxNoAck :
                1;                           //!< No acknowledgment received for Rx with variable length packets
bool PacketSent :
                1;                        //!< Packet sent, only relevant in Tx mode
            } TxRxStatus;                                   //!< The Tx/Rx status Byte
uint8_t SyncAddrStatus :
            3;                     //!< The id of the correlator who found the packet
        } LoRa;
        struct
        {
            int8_t RssiAvg;                                 //!< The averaged RSSI
            int8_t RssiSync;                                //!< The RSSI of the last packet
            struct
            {
                bool SyncError : 1;                         //!< SyncWord error on last packet
                bool LengthError : 1;                       //!< Length error on last packet
                bool CrcError : 1;                          //!< CRC error on last packet
                bool AbortError : 1;                        //!< Abort error on last packet
                bool HeaderReceived : 1;                    //!< Header received on last packet
                bool PacketReceived : 1;                    //!< Packet received
                bool PacketControlerBusy : 1;               //!< Packet controller busy
            } ErrorStatus;                                  //!< The error status Byte
            struct
            {
                uint8_t RxPid : 2;                          //!< PID of the Rx
bool RxNoAck :
                1;                           //!< No acknowledgment received for Rx with variable length packets
                bool RxPidErr : 1;                          //!< Received PID error
bool PacketSent :
                1;                        //!< Packet sent, only relevant in Tx mode
            } TxRxStatus;                                   //!< The Tx/Rx status Byte
uint8_t SyncAddrStatus :
            3;                     //!< The id of the correlator who found the packet
        } Flrc;
        struct
        {
            int8_t RssiAvg;                                 //!< The averaged RSSI
            int8_t RssiSync;                                //!< The RSSI of the last packet
            struct
            {
                bool SyncError : 1;                         //!< SyncWord error on last packet
                bool LengthError : 1;                       //!< Length error on last packet
                bool CrcError : 1;                          //!< CRC error on last packet
                bool AbortError : 1;                        //!< Abort error on last packet
                bool HeaderReceived : 1;                    //!< Header received on last packet
                bool PacketReceived : 1;                    //!< Packet received
                bool PacketControlerBusy : 1;               //!< Packet controller busy
            } ErrorStatus;                                  //!< The error status Byte
            struct
            {
bool PacketSent :
                1;                        //!< Packet sent, only relevant in Tx mode
            } TxRxStatus;                                   //!< The Tx/Rx status Byte
uint8_t SyncAddrStatus :
            3;                     //!< The id of the correlator who found the packet
        } Ble;
    } Params;
} PacketStatus_t;





typedef struct
{
    GPIO_TypeDef *PRW_GPIO_BASE;         //PRWn电源管脚GPIO
    uint8_t  PRW_GPIO_Pin;               //PRWn电源管脚Pin
    GPIO_TypeDef *MCU_DRDY_GPIO_BASE;   //中断数据管脚GPIO
    uint8_t       MCU_DRDY_GPIO_Pin;    //中断数据管脚Pin
    uint8_t       MCU_DRDY_EXTI_Pin;    //外部中断，中断线
    uint8_t  MCU_DRDY_EXTI_BASE;            //外部中断，

    GPIO_TypeDef *SX1280_RESET_GPIO_BASE;       //SX1280 复位管脚GPIO
    uint8_t SX1280_RESET_EN_GPIO_Pin;            //SX1280 复位管脚Pin

    GPIO_TypeDef *SX1280_BUSY_GPIO_BASE;        //SX1280 忙管脚GPIO
    uint8_t SX1280_BUSY_EN_GPIO_Pin;            //SX1280 忙管脚Pin

    GPIO_TypeDef *SPI_EN_GPIO_BASE;     //SX1280 SPI EN管脚GPIO
    uint8_t SPI_EN_GPIO_Pin;            //SX1280 SPI EN管脚Pin

    GPIO_TypeDef *SPI_SCK_GPIO_BASE;     //SX1280 SPI SCK管脚GPIO
    uint8_t SPI_SCK_GPIO_Pin;            //SX1280 SPI SCK管脚Pin

    GPIO_TypeDef *SPI_MOSI_GPIO_BASE;     //SX1280 SPI MOSI管脚GPIO
    uint8_t SPI_MOSI_GPIO_Pin;            //SX1280 SPI MOSI管脚Pin

    GPIO_TypeDef *SPI_MISO_GPIO_BASE;     //SX1280 SPI MISO管脚GPIO
    uint8_t SPI_MISO_GPIO_Pin;            //SX1280 SPI MISO管脚Pin

	GPIO_TypeDef *RST_GPIO_BASE;         //SX1280 RST_GPIO管脚GPIO
    uint8_t RST_GPIO_Pin;                //SX1280 RST_GPIO管脚Pin

    SPI_TypeDef *SX1280_SPI_BASE;         //SX1280 SPI
    uint8_t SX1280_SPI_CLK_Peripheral; //SPI时钟

    uint8_t SPI_FirstBit;//SPI参数
    uint8_t SPI_BaudRatePrescaler;//SPI参数
    uint8_t SPI_Mode;//SPI参数
    uint8_t SPI_CPOL;//SPI参数
    uint8_t SPI_CPHA;//SPI参数
    uint8_t SPI_Data_Direction;//SPI参数
    uint8_t SPI_Slave_Management;//SPI参数
    uint8_t CRCPolynomial;//SPI参数

    uint8_t      *sx1280RxBuf;           //串口接收缓冲区
    uint16_t      SX1280_RX_BUF_SIZE;    //数据接收缓冲深度

	uint16_t  maxTryTimes;               //发送最大尝试次数
	uint8_t   txDoneFlag;                //发送完成标志

}*PDRIVERS_SX1280_CFG_TypeDef, SDRIVERS_SX1280_CFG_TypeDef;

void SX1280_Init();
void RF_RxProcess(pWRCfg pWRCfgInfo);

//SX1280 PWR引脚设为输出并拉高
#define SX1280_PWR_ON(SX1280_GPIO)\
                        SX1280_GPIO.PRW_GPIO_BASE->DDR |= SX1280_GPIO.PRW_GPIO_Pin; \
                        SX1280_GPIO.PRW_GPIO_BASE->CR1 |= SX1280_GPIO.PRW_GPIO_Pin; \
                        SX1280_GPIO.PRW_GPIO_BASE->ODR |= SX1280_GPIO.PRW_GPIO_Pin


#define DRIVERS_SX1280_RX_BUFF_SIZE 128

static uint8_t SX1280_RX_BUFF[DRIVERS_SX1280_RX_BUFF_SIZE] ;

#define UART_DEBUG 1
#define SX1280_TEST_MODE 0
#if SX1280_TEST_MODE || UART_DEBUG
static SBSP_UART_CFG_TypeDef SX1280_TEST_UART;					 //串口
#endif

typedef struct
{
    void (*SX1280_PowerOn)(void);    //SX1280深度睡眠
    void (*SX1280_PowerOff)(void);   //SX1280唤醒
    void (*SX1280_Init)(void);     //SX1280初始化
    void (*SX1280_SendBuff)(uint8_t * sendBuf, uint8_t bufSize);  //SX1280发送数据
    uint32_t (*SX1280_ReadBuff)(uint8_t *buffter,
                                uint32_t len);     //SX1280获取数据
    void (*SX1280_ParamModify)(uint8_t* buffer) ;    
    uint16_t (*SX1280_GetMaxTryTimes)(void);
	uint8_t (*SX1280_GetTxDoneFlag)(void);
	void   (*SX1280_SetTxDoneFlag)(uint8_t txDoneFlag);
	uint8_t   (*SX1280_TestMode)(void);    //SX1280测试模式
} SX1280_OPERATION_TypeDef;

uint32_t Drivers_SX1280ReadBuff(uint8_t *buffter, uint32_t len);
void Drivers_SX1280SendBuff(uint8_t * sendBuf, uint8_t bufSize);
void Drivers_SX1280Init();
void Drivers_SX1280PowerOn();
void Drives_SX1280PowerOff();
void Drivers_SX1280ParamModify(uint8_t *bufffer);
uint16_t Drivers_SX1280GetMaxTryTimes(void);
uint8_t Drivers_SX1280GetTxDoneFlag(void);
void Drivers_SX1280SetTxDoneFlag(uint8_t txDoneFlag);
uint8_t Drivers_SX1280TestMode(void);    //SX1280测试模式

//提供RM3100对外的接口
static const SX1280_OPERATION_TypeDef SX1280_Operation =
{
    .SX1280_PowerOn = Drivers_SX1280PowerOn,
    .SX1280_PowerOff = Drives_SX1280PowerOff,
    .SX1280_Init = Drivers_SX1280Init,
    .SX1280_SendBuff = Drivers_SX1280SendBuff,
    .SX1280_ReadBuff =  Drivers_SX1280ReadBuff,
    .SX1280_ParamModify = Drivers_SX1280ParamModify,
	.SX1280_GetMaxTryTimes = Drivers_SX1280GetMaxTryTimes,
	.SX1280_GetTxDoneFlag = Drivers_SX1280GetTxDoneFlag,
	.SX1280_SetTxDoneFlag = Drivers_SX1280SetTxDoneFlag,
	.SX1280_TestMode = Drivers_SX1280TestMode,
};

#endif
















