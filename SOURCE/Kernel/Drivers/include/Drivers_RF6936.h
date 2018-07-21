#ifndef _DRIVERS_FR6936_H_
#define _DRIVERS_FR6936_H_

#include "BSP_SPI.h"
#include "stm8l15x.h"
#include "Drivers_Common.h"

/*****************************宏定义**************************/
#define FR6936_SPI_NUMBER SPI1


/******************Rf6936 factory define start*****************/


/*-------------------------------------------------------------
*
* Outline:
*      Register Function Detail
*      File Function Detail
*--------------------------------------------------------------
*      Register Function Detail
*--------------------------------------------------------------
* -------------------------------------------------------------
* Channel register
*--------------------------------------------------------------
* all - R/W
*6:0  This field selects the channel. 0x00 sets 2400MHz 0x62 sets 2498Mhz.
*     Values above 0x62 are not valid.
*     I.e range in decimal 0-98 default to channel 72
* -------------------------------------------------------------
*/
#define CHANNEL_ADR                                       0x00
#define CHANNEL_RST                                       0x48
#define CHANNEL_MSK                                       0x7F

#define CHANNEL_MAX                                       0x62
#define CHANNEL_MIN                                       0x00
#define CHANNEL_2P498_GHZ                                 0x62
#define CHANNEL_2P4_GHZ                                   0x00

/*-------------------------------------------------------------
* -------------------------------------------------------------
* TX Length register
* -------------------------------------------------------------
* all - R/W
* 7:0  This register sets the length of the packet to be transmitted. A length
*      of zero is valid, and will transmit a packet with SOP, length and CRC
*      fields (if enabled), but no data field. Packet lengths of more than 16
*      bytes will require that some data bytes be written after transmission
*      of the packet has begun because of the size of the TX data buffer.
* -------------------------------------------------------------
*/
#define TX_LENGTH_ADR                                     0x01
#define TX_LENGTH_RST                                     0x00
#define TX_LENGTH_MSK                                     0xFF

/*-------------------------------------------------------------
* -------------------------------------------------------------
* TX Control register
* -------------------------------------------------------------
* all - R/W
* 7    TX_GO - Transmit Enable. Setting this bit triggers the transmission of
*      a packet.  Writing a 0 to this flag has no effect. This bit is cleared
*      automatically at the end of packet transmission.
*
*      The TX buffer may be loaded [before or] after setting this bit. If data
*      is loaded after setting this bit, the length of time available to load
*      the buffer depends on the starting state (sleep, idle or synth), the
*      length of the SOP code and the packet data rate. For example, if
*      starting from idle mode on a fast channel in 8DR mode with 32 chip SOP
*      codes the time available is 100us (synth start) + 64us (SOP length) +
*      32us (length byte) = 196us. If there are no bytes in the TX buffer at
*      the end of transmission of the length field, an underflow IRQ will
*      occur.
* 6    TX_CLR - Clear TX Buffer. Writing a 1 to this register clears the TX
*      buffer and associated error flags.  Writing a 0 to this bit has no
*      effect. The previous packet may be re-transmitted by setting TX_GO and
*      not setting this bit.  When not re-transmitting data currently in the TX
*      buffer, TX_GO may be set at the same time or any time after TX_CLR is
*      set.
*
*      If the TX buffer is to be loaded after the TX_GO bit has been set, then
*      TX_CLR should be set when TX_GO is set.
*      If the TX buffer is to be loaded before the TX_GO bit has been set,
*      then TX_CLR must be set before loading the data.
*
*      Note: TX_GO does not clear under-run, only TX_CLR does.
*
* For the following bits see TX_IRQ register for IRQ function description.
* 5    TXB15_IRQ_EN    - Buffer Not Full Interrupt Enable.
* 4    TXB8_IRQ_EN     - Buffer Half Empty Interrupt Enable.
* 3    TXB0_IRQ_EN     - Buffer Empty Interrupt Enable.
* 2    TXBERR_IRQ_EN   - TX Buffer Under-run/overflow Interrupt Enable.
* 1    TXC_IRQ_EN      - Transmission Complete Interrupt Enable.
* 0    TXE_IRQ_EN      - Transmit Error Interrupt Enable.
* ---------------------------------------------------------------------------
*/
#define TX_CTRL_ADR                                       0x02
#define TX_CTRL_RST                                       0x03

/* See TX_IRQ for remaining bit position definitions
* TX_CTRL bit masks
*/
#define TX_GO                                             0x80
#define TX_CLR                                            0x40
/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* TX Configuration register
* ------------------------------------------------------------------------------
* all - R/W
* 5    Data Code Length - This bit selects between 32 and 64 chip PN codes for
*      the data portion of the packet. This bit is ignored when the data mode
*      is set to GFSK. 1 = 64 chip codes. 0 = 32 chip codes.
*      This field also applies to RX packets when SOP_EN=0 or FRC_RXDR=1.
* 4:3  Data Mode - This field sets the data transmission mode.
*      00 = 1Mbps GFSK.  01 =  8DR Mode. 10 = DDR Mode. 11 = SDR Mode.
*      This field also applies to RX packets when SOP_EN=0 or FRC_RXDR=1.
* 2:0  PA Setting. This field sets the transmit signal strength.
*      0 = -30dBm, 1 = -25dBm, 2 = -20dBm, 3 = -15dBm,
*      4 = -10dBm, 5 = -5dBm,  6 = 0dBm,   7 = +4dBm.
*      Default Value 5 = -5dBm
* -----------------------------------------------------------------------------
*/
#define TX_CFG_ADR                                        0x03
#define TX_CFG_RST                                        0x05

/* separate bit field masks */
#define TX_DATCODE_LEN_MSK                                0x20
#define TX_DATMODE_MSK                                    0x18
#define PA_VAL_MSK                                        0x07

/* DATMODE values */
#define _1MBPS                                            0x00
#define _8DR                                              0x01
#define xxxDDR                                            0x02
#define SDR                                               0x03
/* DATACODE_LEN values */
#define DCLEN_32                                          0x00
#define DCLEN_64                                          0x01

/* DATCODE_LEN register masks */
#define DATCODE_LEN_64                                    0x20
#define DATCODE_LEN_32                                    0x00
/* DATMODE register masks */
#define DATMODE_RAW                                       0x00
#define DATMODE_8DR                                       0x08
#define DATMODE_DDR                                       0x10
#define DATMODE_SDR                                       0x18
/* PA_SET register masks */
#define PA_N30_DBM                                        0x00
#define PA_N25_DBM                                        0x01
#define PA_N20_DBM                                        0x02
#define PA_N15_DBM                                        0x03
#define PA_N10_DBM                                        0x04
#define PA_N5_DBM                                         0x05
#define PA_0_DBM                                          0x06
#define PA_4_DBM                                          0x07

/* For HDL use only!
 *combined {DATCODE_LEN & DATMODE} sofdatatype values
 */
#define SDTYPE_ACK                                        0x04
#define SDTYPE_RAW                                        0x00
#define SDTYPE_64_8DR                                     0x05
#define SDTYPE_32_8DR                                     0x01
#define SDTYPE_64_DDR                                     0x06
#define SDTYPE_32_DDR                                     0x02
#define SDTYPE_64_SDR                                     0x07
#define SDTYPE_32_SDR                                     0x03

/*-------------------------------------------------------------------------

* -------------------------------------------------------------------------
* TX IRQ Status register
* -------------------------------------------------------------------------
* The state of all IRQ Status bits is valid regardless of whether or not the
* IRQ is enabled. The IRQ output of the device is in its active state whenever
* one or more bits in this register is set and the corresponding IRQ enable
* bit is also set.
*
* all - R
* 7    Oscillator Stable IRQ Status. This bit is set when the internal crystal
*      oscillator is stable. This bit is set whenever the oscillator is stable.
* 6    Low Voltage Interrupt Status. This bit is set when the voltage on Vadcref
*      is below the LVI threshold.  It is cleared whenever the Vadcref voltage is
*      above the LVI threshold.
* 5    Buffer Not Full Interrupt Status. This bit is set whenever there are 15
*      or fewer bytes remaining in the transmit buffer, and cleared otherwise.
* TODO: SYO wordsmith TXB8 - also change names from "full", empty, etc.
* 4    Buffer Half Empty Interrupt Status. This bit is set whenever there are 8
*      or fewer bytes remaining in the transmit buffer, and cleared otherwise.
* 3    Buffer Empty Interrupt Status. This bit is set at any time that the
*      transmit buffer is empty, and cleared whenever there are any bytes
*      present in the buffer.
* 2    TX Buffer Error Interrupt Status. This IRQ is triggered in one of two
*      ways.  1. When the number of bytes remaining to be transmitted is
*      greater than 0 and the transmit buffer is empty when the TX state
*      machine tries to fetch the next byte for transmission.  2. When the
*      TX buffer is full and there is an attempt to write data to the TX
*      buffer.  Setting TX_CLR bits clears this flag.
* 1    Transmission Complete Interrupt Status. This IRQ is triggered when
*      transmission is complete.  If transaction mode is enabled, this
*      interrupt is triggered only when an ACK is correctly received. If
*      transaction mode is not enabled then this interrupt is triggered
*      immediately after transmission of the last bit of the CRC. This bit is
*      cleared when this register is read.
* 0    Error Interrupt Status. This IRQ is triggered when there is an error in
*      transmission. This interrupt is only applicable to Transaction Mode. It
*      is triggered whenever no valid ACK packet is received within the ACK
*      timeout period. Reading this register clears this bit.
*
* ----------------------------------------------------------------------------
*/
#define TX_IRQ_STATUS_ADR                                 0x04
/* There is no default value for this register.
*
* TX_IRQ bit masks*/
#define XS_IRQ                                            0x80
#define LV_IRQ                                            0x40
#define TXB15_IRQ                                         0x20
#define TXB8_IRQ                                          0x10
#define TXB0_IRQ                                          0x08
#define TXBERR_IRQ                                        0x04
#define TXC_IRQ                                           0x02
#define TXE_IRQ                                           0x01

/* See PWR_CTRL register for LV_IRQ_EN
* See XTAL_CTRL register for XS_IRQ_EN

*------------------------------------------------------------------------------
* -----------------------------------------------------------------------------
* RX Control register
* -----------------------------------------------------------------------------
* all - R/W
* 7    RX_GO - Receive Enable. Setting causes the device to transition to
*      Receive mode.  If necessary, the crystal oscillator and synthesizer will
*      start automatically after this bit is set, and before the device enters
*      receive mode.  Clearing this bit has no effect.  Setting this bit when
*      the device is already in receive mode will cause the receive state
*      machine to be reset; all pending RX IRQs will be cleared and the RX
*      buffer reset.
*
* 6    SOFDET_IRQ_EN   - Start of Packet Detect Interrupt Enable.
* 5    RXB16_IRQ_EN    - Buffer Full Interrupt Enable.
* 4    RXB8_IRQ_EN     - Buffer Half Empty Interrupt Enable.
* 3    RXB1_IRQ_EN     - Buffer Not Empty Interrupt Enable.
* 2    RXBERR_IRQ_EN   - Buffer Error Interrupt Enable.
* 1    RXC_IRQ_EN      - Packet Reception Complete Interrupt Enable.
* 0    RXE_IRQ_EN      - Receive Error Interrupt Enable.
* -----------------------------------------------------------------------------
*/
#define RX_CTRL_ADR                                       0x05
#define RX_CTRL_RST                                       0x07

/* See RX_IRQ register for bit positions definitions also used for this register

* RX_CTRL bit masks*/
#define RX_GO                                             0x80

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* RX Configuration register
* ------------------------------------------------------------------------------
* all - R/W
* 7    AGEN - Automatic Gain Control Enable. When this bit is set, automatic
*      gain control is enabled, and the LNA is controlled by the AGC circuit.
*      When this bit is cleared the LNA is controlled manually using the LNA
*      bit.
* 6    LNA - LNA Manual Control. When AGC EN is cleared, this bit controls the
*      receiver LNA; when AGC EN is set, this bit has no effect. Setting this
*      bit enables the LNA; clearing this bit disables the LNA.
* 5    ATT - Receive Attenuator Enable. Setting this bit enables the receiver
*      attenuator. The Receiver Attenuator may be used to de-sensitize the
*      receiver so that only very strong signals may be received.
* 4    HILO - When HILO_EN is set, this bit is used to select whether the device
*      will use the high frequency for the channel selected, or the low
*      frequency.  1 = hi; 0= lo.
*      When HILO_EN is not enabled this also controls the highlow bit to the
*      receiver and should be left at the default value of 1 for high side RX
*      injection.
* 3    HILO_EN - HiLo Mode Enable. When this bit is set, the HiLo bit in the
*      Channel register determines whether the device receives data transmitted
*      1MHz above the RX Synthesizer frequency or 1MHz below the RX Synthesizer
*      frequency. Use of this mode allows for very fast turn-around, because
*      the same synthesizer frequency may be used for both transmit and
*      receive, thus eliminating the synthesizer re-settling period between
*      transmit and receive. Note that when this bit is set, and the HiLo bit
*      is cleared, received data bits are automatically inverted to compensate
*      for the inversion of data received on the "image" frequency.
* 1    RXOW_EN - Overwrite Enable. When this bit is set, if an SOP is detected
*      while there RX buffer is not empty, then the existing contents of RX
*      buffer are lost, and the new packet is loaded into the RX buffer.  When
*      this bit is set, the RXOW_IRQ is enabled.
*      If this bit is cleared, then the RX buffer may not be over-written by a
*      new packet, and whenever the RX buffer is not empty SOP conditions are
*      ignored, and it is not possible to receive data until the previously
*      received packet has been completely read from the RX buffer.
* 0    VLD_EN - Valid Flag Enable. When this bit is set, the receive buffer can
*      store only 8 bytes of data.  The other half of the buffer is used to
*      store valid flags.  See RX_BUFFER for more detail.
* -----------------------------------------------------------------------------
*/
#define RX_CFG_ADR                                        0x06
#define RX_CFG_RST                                        0x92

#define AUTO_AGC_EN                                       0x80
#define LNA_EN                                            0x40
#define ATT_EN                                            0x20
#define HI                                                0x10
#define LO                                                0x00
#define HILO_EN                                           0x08      //When FAST TURN EN is set,uses the high frequency for the channel 
//selected,实际为FAST_TURN_EN位
#define RXOW_EN                                           0x02
#define VLD_EN                                            0x01      //需要兼容老设备时才使能
#define FAST_TURN_EN                                      (0x08)

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* RX IRQ register
* ------------------------------------------------------------------------------
* The state of all IRQ Status bits is valid regardless of whether or not
* the IRQ is enabled. The IRQ output of the device is in its active state
* whenever one or more bits in this register is set and the corresponding
* IRQ enable bit is also set (see also TX IRQ register).
*
* all - R (except RXOW_IRQ which must be written to clear)
* 7    RXOW_IRQ - Receive Overwrite Interrupt Status. This IRQ is triggered
*      when the receive buffer is over-written by a packet being received
*      before the previous packet has been read from the buffer.  This bit is
*      cleared only by writing to this register (the value does not matter).
*      This condition is only possible when the Overwrite Enable bit in the
*      Transaction Configuration register is set. This bit must be cleared by
*      firmware before the new packet may be read from the RX packet buffer.
* 6    SOFDET_IRQ - Start of Packet Detect Interrupt Status. This bit is set
*      when a SOF condition (pair of SOPs) is detected. This bit is cleared
*      when this register is read.
* 5    RXB16_IRQ - Receive Buffer Full Interrupt Status. This bit is set
*      whenever the receive buffer is full, and cleared otherwise.
* TODO: SYO  wordsmith RXB8 for use in conjunction with RXB16 and RXB1 etc.
* 4    RXB8_IRQ - Receive Buffer Half Full Interrupt Status. This bit is set
*      whenever there are 8 or more bytes remaining in the receive buffer, and
*      cleared when there are 7 or fewer bytes remaining.
* 3    RXB1_IRQ - Receive Buffer Not Empty Interrupt Status. This bit is set
*      at any time that there are 1 or more bytes in the receive buffer, and
*      cleared only when the buffer is empty.
* 2    RXBERR_IRQ - RX Buffer Error Interrupt Status. This IRQ is triggered in
*      one of two ways.  1. When the RX buffer is empty and there is an
*      attempt to read data.  2. When the RX buffer is full and more data is
*      recieved.  This flag is cleared when a RX_GO is set and an SOP pair is
*      received.
* 1    RXC_IRQ - Packet Receive Complete Interrupt Status. This IRQ is
*      triggered when a packet has been received. If Transaction Mode is
*      enabled, then this bit is not sent until after transmission of the ACK
*      or there was a recieve error and no ACK is to be sent.
*      If Transaction Mode is not enabled then this bit is set as soon as a
*      packet is received.  This bit is cleared when this register is read.
* 0    RXE_IRQ - Receive Error Interrupt Status. This IRQ is triggered when
*      there is an error in reception. It is triggered whenever a packet is
*      received with a bad CRC, an unexpected EOP is detected, a packet type
*      (data or ACK) mismatch, or a packet is dropped because the RX buffer is
*      still not empty when the next packet starts.  The exact cause of the
*      error may be determined by reading the RX Status Register. This bit is
*      cleared when this register is read.
* ----------------------------------------------------------------------------
*/
#define RX_IRQ_STATUS_ADR                                        0x07
/* There is no default value for this register.
*
* RX_IRQ bit masks*/
#define RXOW_IRQ                                                 0x80
#define SOFDET_IRQ                                               0x40
#define RXB16_IRQ                                                0x20
#define RXB8_IRQ                                                 0x10
#define RXB1_IRQ                                                 0x08
#define RXC_IRQ                                                  0x02
#define RXE_IRQ                                                  0x01

/*------------------------------------------------------------------------------
* ------------------------------------------------------------------------------
* RX Status register
* ------------------------------------------------------------------------------
* The bits in this register are only valid after RXC_IRQ or RXE_IRQ is set and
* remain valid until the next SOFDET_IRQ.
*
* all - R
* 7    RX_ACK - RX Packet Type.  This bit is set when the received packet is an
*      ACK packet, and cleared when the Received packet is a standard packet.
* 6    PKTERR - RX Packet Type Error.  This bit is set when the packet
*      type received is what not was expected and cleared when the packet type
*      received was as expected.  For example, if a data packet is expected
*      and an ACK is received, this bit will be set.
* 5    EOPERR - Unexpected EOP Error.  This bit is set when an EOP is detected
*      before the expected data length and CRC fields have been received.
*      This bit is cleared when SOP pattern for the next packet has been
*      received.  This includes the case where there are invalid bits detected
*      in the length field and the length field is forced to 0.
* 4    CRC0 - Zero-seed CRC. This bit is set whenever the CRC of the last
*      correctly received packet has a zero seed.
* 3    BAD_CRC - Bad CRC. This bit is set when the CRC of the last received
*      packet is incorrect.  This bit may also indicate that there was a
*      demodulation error on the length byte and the packet was dropped when
*      CRC is disabled.
* 2    RX_DATCODE_LEN - Receive Code Length. This bit indicates the PN code
*      length used in the last correctly received packet.
*      1 = 64 chip PN code, 0 = 32-chip PN code.
* 1:0  RX_DATMODE - Receive Data Mode. This bit indicates the data mode of the
*      last correctly received packet.
*      00 = 1Mbps GFSK.  01 =  8DR Mode. 10 = DDR Mode. 11 = SDR Mode.
*
* --------------------------------------------------------------------------------
*/
#define RX_STATUS_ADR                                     0x08
/* There is no default value for this register.
* single flag bits & multi-bit-field masks */
#define RX_ACK                                            0x80
#define RX_PKTERR                                         0x40
#define RX_EOPERR                                         0x20
#define RX_CRC0                                           0x10
#define RX_BAD_CRC                                        0x08
#define RX_DATCODE_LEN                                    0x04
#define RX_DATMODE_MSK                                    0x03

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* RX Count register
* ------------------------------------------------------------------------------
* all - R
* 7:0  This register contains the total number of payload bytes received
*      during reception of the current packet. After packet reception is
*      complete, this register will match the RX Length Register value unless
*      there was a packet error. This register is reset to 0x00 when RX_LENGTH
*      is set.
* -----------------------------------------------------------------------------
*/
#define RX_COUNT_ADR                                      0x09
#define RX_COUNT_RST                                      0x00
#define RX_COUNT_MSK                                      0xFF

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* RX Length Field register
* ------------------------------------------------------------------------------
* all - R
* 7:0  This register contains the length field which is updated with
*      the reception of a new length field (shortly after SOFDET_IRQ).  If
*      there is an error in the received length field, 0x00 is loaded instead
*      and an error is flagged.  See RX_IRQ(RXE_IRQ) and RX_STATUS(EOP_ERR).
* -----------------------------------------------------------------------------
*/
#define RX_LENGTH_ADR                                     0x0A
#define RX_LENGTH_RST                                     0x00
#define RX_LENGTH_MSK                                     0xFF

/*-----------------------------------------------------------------------------

* -----------------------------------------------------------------------------
* Power Control register
* -----------------------------------------------------------------------------
* The Power Management Unit (PMU) provides a boost converter to generate
* voltages required by some device circuits when the main supply voltage (Vadcref)
* falls below the minimum voltage required by these blocks. The PMU can also
* provide power to external devices. In order to provide maximum flexibility,
* the PMU output voltage is programmable, so that device requiring voltages
* higher than the LS2 minimum can be powered from the PMU.
*
* all - R/W
* 7    PMU_EN - PMU Enable (!pmuoff). Setting this bit enables the PMU. When
*      the PMU
*      is disabled, or if the PMU is enabled and the Vadcref voltage is above the
*      value set in Bits 1:0 of this register, the Vout pin is internally
*      connected to the Vadcref pin. If the PMU is enabled and the Vadcref voltage
*      is below the value set by PMU_OUTV, then the PMU will boost the Vout
*      pin to a voltage not less than the value set by PMU_OUTV.
*      Clearing PMU_SEN causes this bit to have no effect.
*  TODO BR doc says: When pmuoff is asserted, this bit disables the PMU and
*      isolates VBAT from VREG
* 6    LV_IRQ_EN - Low Voltage Interrupt Enable. Setting this bit enables the
*      Low Voltage Interrupt. When this interrupt is enabled, if the Vadcref
*      voltage falls below the value set by LVITH of this register, then a low
*      voltage interrupt will be generated. Unless bit 5 of this register is
*      also set, the LVI IRQ will be automatically disabled when the device is
*      in Sleep Mode.
* 5    PMU_SEN - PMU Enabled in Sleep Mode. If this bit is set, the PMU will
*      continue to operate normally when the device is in Sleep Mode. If this
*      bit is not set, then the PMU is disabled when the device is in Sleep
*      Mode. In this case, if Vadcref is below the PMU_OUTV voltage and PMU_EN is
*      set when the device enters Sleep Mode the Vout voltage is fall to the
*      Vadcref voltage as the Vout capacitors discharge.
*      Verification notes: pmuoff = PMU_SEN ? !PMU_EN : vdigoff;
* 4    PFET_OFF - When set disables the PFET that connects VBAT to VREG
*      verification notes: directly drive pfetoff analog signal.
*
*      ECO EDIT - the next two fields are redefined as inverse of previous
*      settings to match the ECO edit being done prior to tapeout
* 3:2  LV_IRQ_TH - Low Voltage Interrupt Threshold. This field sets the voltage
*      on Vadcref at which the LVI is triggered.
*       11 = 1.8V;
*   10 = 2.0V;
*   01 = 2.2V;
*   00 = PMU_OUTV; = default reset value
* 1:0  PMU_OUTV - PMU Output Voltage (vregsel). This field sets the minimum
*      output
*      voltage of the PMU.
*   11 = 2.4V;
*   10 = 2.5V;
*   01 = 2.6V;
*   00 = 2.7V; = default reset value
*  When the PMU is active, the voltage output by the PMU on Vadcref will
*  never be less this voltage provided that the total load on the Vout pin
*  is less than the specified maximum value, and the voltage in Vadcref is
*  greater than the specified minimum value.
* -----------------------------------------------------------------------------
*/
#define PWR_CTRL_ADR                                      0x0B
#define PWR_CTRL_RST                                      0xA0

/* single flag bits & multi-bit-field masks */
#define PMU_EN                                            0x80
#define LV_IRQ_EN                                         0x40
#define PMU_SEN                                           0x20
#define PFET_OFF                                          0x10
#define LV_IRQ_TH_MSK                                     0x0C
#define PMU_OUTV_MSK                                      0x03

/* LV_IRQ_TH values */
#define LV_IRQ_TH_1P8_V                                   0x00
#define LV_IRQ_TH_2P0_V                                   0x01
#define LV_IRQ_TH_2P2_V                                   0x02
#define LV_IRQ_TH_2P4_V                                   0x03
/* PMU_OUTV values */
#define PMU_OUTV_2P4                                      0x00
#define PMU_OUTV_2P5                                      0x01
#define PMU_OUTV_2P6                                      0x02
#define PMU_OUTV_2P7                                      0x03

/*--------------------------------------------------------------------------

* --------------------------------------------------------------------------
* Crystal Control register
* --------------------------------------------------------------------------
* all - R/W (unless otherwise noted)
* 7:6  XOUT_FNC - XOUT Pin Function. This field selects between the different
*      functions of the XOUT pin.
*      00 = Clock frequency set by XOUT FREQ;
*      01 = Active low PA Control;
*      10 = Radio data serial bitstream.
*        If this option is selected and SPI is configured for 3-wire mode then
*        the MISO pin will output a the serial clock associated with this data
*        stream;
*        If the PACTL pin is asserted, then the stream will be TX data otherwise
*        it will is RX data.
*      11 = GPIO
*        To disable this output, set to GPIO mode, and set the GPIO state in
*        the IO Configuration register.
* 5    XS_IRQ_EN - Crystal Stable Interrupt Enable. This bit enables the
*      Crystal Stable interrupt. When enabled, this interrupt generates an IRQ
*      when the crystal has stabilized after the device has woken from sleep
*      mode.
* 2:0  XOUT_FREQ - Xout Frequency. This field sets the frequency output on the
*      XOUT pin when XOUTFN is set to 00.
*      0 = 12MHz; 1 = 6MHz, 2 = 3MHz, 3 = 1.5MHz, 4 = 0.75MHz;
*      other values are not defined
*
* ----------------------------------------------------------------------------
*/
#define XTAL_CTRL_ADR                                     0x0C
#define XTAL_CTRL_RST                                     0x04

/* single flag bits & multi-bit-field masks */
#define XOUT_FNC_MSK                                      0xC0
#define XS_IRQ_EN                                         0x20
#define XOUT_FREQ_MSK                                     0x07

/* XOUT_FNC values */
#define XOUT_FNC_XOUT_FREQ                                0x00
#define XOUT_FNC_PA_N                                     0x40
#define XOUT_FNC_RAD_STREAM                               0x80
#define XOUT_FNC_GPIO                                     0xC0

/* XOUT_FREQ values */
#define XOUT_FREQ_12MHZ                                   0x00
#define XOUT_FREQ_6MHZ                                    0x01
#define XOUT_FREQ_3MHZ                                    0x02
#define XOUT_FREQ_1P5MHZ                                  0x03
#define XOUT_FREQ_P75MHZ                                  0x04
/* This is used for characterization/test */
#define XOUT_12MHZ_CTS                                    0x05

/*------------------------------------------------------------------------

* ------------------------------------------------------------------------
* I/O Configuration register
* ------------------------------------------------------------------------
* When using a GPIO pin as an input, the output mode should be set to open
* drain, and a "1" written to the corresponding output register bit.
*
* all - R/W
* 7    IRQ_OD - IRQ Pin Drive Strength. Setting this bit configures the IRQ pin
*      as an Open Drain Output. Clearing this bit configures the IRQ pin as a
*      Standard CMOS Output, with the Output "1" drive voltage being equal to
*      the Vio pin voltage.
* 6    IRQ_POL - IRQ Polarity. Setting this bit configures the IRQ signal
*      polarity to be active high. Clearing this bit configures the IRQ signal
*      polarity to be active low.
* 5    MISO_OD - MISO Pin Drive Strength. Setting this bit configures the MISO
*      pin as an Open Drain Output. Clearing this bit configures the miso pin
*      as a Standard CMOS Output, with the Output "1" drive voltage being
*      equal to the Vio pin voltage.  This setting this bit affects both MISO
*      MOSI when SPI_3_PIN is also set.
* 4    XOUT_OD - XOUT Pin Drive Strength. Setting this bit configures the XOUT
*      pin as an Open Drain Output. Clearing this bit configures the XOUT pin
*      as a Standard CMOS Output, with the Output "1" drive voltage being equal
*      to the Vio pin voltage.
* 3    PACTL_OD - PACTL Pin Drive Strength. Setting this bit configures the
*      PACTL pin as an Open Drain Output. Clearing this bit configures the
*      PACTL pin as a Standard CMOS Output, with the Output "1" drive voltage
*      being equal to the Vio pin voltage.
* 2    PACTL_GPIO - PACTL Mode.  When this bit is set the PACTL pins is a GPIO.
* 1    SPI_3_PIN - SPI Mode. When this bit is cleared, the SPI interface acts
*      as a standard 4-wire SPI Slave interface. When this bit is set, the SPI
*      interface operates in "3-Wire Mode" combining MISO and MOSI on the same
*      pin, and the MISO pin is available as a GPIO pin.  MISO is also turned
*      on as an output.
* 0    IRQ_GPIO - IRQ Mode. When this bit is cleared, the IRQ pin is asserted
*      when an IRQ is active; the polarity of this IRQ signal is configurable
*      in IRQ POL. When this bit is set, the IRQ pin is available for use as a
*      GPIO pin, and the IRQ function is multiplexed onto the MOSI pin. In the
*      IRQ signal state is presented on the MOSI pin whenever the SPI /SS
*      signal is inactive (high).
*
* ----------------------------------------------------------------------------
*/
#define IO_CFG_ADR                                        0x0D
#define IO_CFG_RST                                        0x00
#define IO_CFG_MSK                                        0xFF

/* single flag bits & multi-bit-field masks */
#define IRQ_OD                                            0x80
#define IRQ_POL                                           0x40
#define MISO_OD                                           0x20
#define XOUT_OD                                           0x10
#define PACTL_OD                                          0x08
#define PACTL_GPIO                                        0x04
#define SPI_3_PIN                                         0x02
#define IRQ_GPIO                                          0x01

/*-------------------------------------------------------------------------

* -------------------------------------------------------------------------
* GPIO Control register
* -------------------------------------------------------------------------
* When using a GPIO pin as an input, the output mode should be set to open
* drain, and a "1" written to the corresponding output register bit.
*
* all - R/W (unless otherwise noted)
* 7    XOUT_OP - XOUT Output. When the Xout pin is configured to be a GPIO,
*      the state of this bit sets the output state of the Xout pin.
* 6    MISO_OP - MISO Output. When the MISO pin is configured to be a GPIO,
*      the state of this bit sets the output state of the MISO pin.
* 5    PACTL_OP - PACTL Output. When the PACTL pin is configured to be a GPIO,
*      the state of this bit sets the output state of the PACTL pin.
* 4    IRQ_OP - IRQ Output. When the IRQ pin is configured to be a GPIO,
*      the state of this bit sets the output state of the IRQ pin.
* 3(R) XOUT_IP - XOUT Input. When the Xout pin is configured to be a GPIO,
*      the state of this bit reflects the voltage on the Xout pin.
* 2(R) MISO_IP - MISO Input. When the MISO pin is configured to be a GPIO,
*      the state of this bit reflects the voltage on the MISO pin.
* 1(R) PACTL_IP - PACTL Input. When the PACTL pin is configured to be a GPIO,
*      the state of this bit reflects the voltage on the PACTL pin.
* 0(R) IRQ_IP - IRQ Input. When the IRQ pin is configured to be a GPIO,
*      the state of this bit reflects the voltage on the IRQ pin.
*
* --------------------------------------------------------------------------
*/
#define GPIO_CTRL_ADR                                     0x0E
#define GPIO_CTRL_RST                                     0x00
/* The bottom four *_IP bits have no default register value. */
#define GPIO_CTRL_MSK                                     0xF0

/* single flag bits & multi-bit-field masks */
#define XOUT_OP                                           0x80
#define MISO_OP                                           0x40
#define PACTL_OP                                          0x20
#define IRQ_OP                                            0x10
#define XOUT_IP                                           0x08
#define MISO_IP                                           0x04
#define PATCL_IP                                          0x02
#define IRQ_IP                                            0x01

/*-------------------------------------------------------------------------

* -------------------------------------------------------------------------
* Transaction Configuration register
* -------------------------------------------------------------------------
* all - R/W
* 7    ACK_EN - Acknowledge Enable. When this bit is set, and ACK packet is
*      automatically transmitted whenever a valid packet is received; in this
*      case the device is considered to be in "Transaction Mode". After
*      transmission of the ACK packet, the device the automatically transitions
*      to the END STATE. When this bit is cleared, the device transitions
*      directly to the END STATE immediately after the end of packet
*      transmission.
* 5    FRC_END_STATE - Force a transition to the state set in END_STATE.
*      This bit is automatically cleared upon completion.
* 4:2  END_STATE - Transaction End State. This field defines the mode to which
*      the device transitions after receiving or transmitting a packet.
*      000 = Sleep Mode;
*      001 = Idle Mode;
*      010 = Synth Mode (TX);
*      011 = Synth Mode (RX);
*      100 = RX Mode.
*      In normal use, this field will typically be set to 000 or 011 when the
*      device is transmitting packets, and 100 when the device is receiving
*      packets.
*      Sleep modes verification notes:
*          Sets stby=1; waits 25us then sets xtalen=0,vdigoff=0,bgen=<spi write>
*      Non-sleep mode verification notes:
*          Sets stby=0, xtalen=1,vdigoff=0,bgen=1
*      Verification notes: FRC_AWAKE overrides controlling stby, xtalen, vdigoff, bgen
* 1:0  ACK_TO - ACK Timeout. When this bit is set, and the device is configured
*      for Transaction Mode, this field sets the timeout period after
*      transmission of a packet during which an ACK must be correctly received
*      in order to prevent a TX Error condition from being detected. This
*      timeout period is expressed in terms of a number of SOP PN Code Lengths;
*      if SOP LEN is set, then the timeout period is this value multiplied by
*      64 and if SOP LEN is cleared then the timeout is this value multiplied
*      by 32.
*      00 = 4x; 01 = 8x, 10 = 12x; 11 = 15x the SOP code length.
*
* -------------------------------------------------------------------------------
*/
#define XACT_CFG_ADR                                      0x0F
#define XACT_CFG_RST                                      0x80

/* single flag bits & multi-bit-field masks */
#define ACK_EN                                            0x80
#define FRC_END_STATE                                     0x20
#define END_STATE_MSK                                     0x1C
#define ACK_TO_MSK                                        0x03

/* END_STATE field values */
#define END_STATE_SLEEP                                   0x00
#define END_STATE_IDLE                                    0x04
#define END_STATE_TXSYNTH                                 0x08
#define END_STATE_RXSYNTH                                 0x0C
#define END_STATE_RX                                      0x10
/* ACK_TO field values */
#define ACK_TO_4X                                         0x00
#define ACK_TO_8X                                         0x01
#define ACK_TO_12X                                        0x02
#define ACK_TO_15X                                        0x03

/*-----------------------------------------------------------------------------

* -----------------------------------------------------------------------------
* Framing Configuration register
* -----------------------------------------------------------------------------
* all - R/W
* 7    SOP_EN - Framing Enable. When this bit is set, each transmitted packet
*      begins with a SOP field, and only packets beginning with a valid SOP
*      field will be received. If this bit is cleared, no SOP field will be
*      generated when a packet is transmitted, and packet reception will begin
*      whenever 2 successive correlations against the Data PN code are
*      detected.
*      The 1Mbps, 250kbps and 125kbps data rates require that this bit is set.
* 6    SOP_LEN - SOP PN Code Length. When this bit is set the SOP PN Code
*      length is 64 Chips. When this bit is cleared the SOP PN Code length is
*      32 Chips.  Be sure to change SOP_THRESH as appropriate when changing this.
* 5    LEN_EN - Packet Length Enable. When this bit is set the 8-bit value
*      contained in the TX Packet Length Register is transmitted immediately
*      after the SOP field. In Receive Mode, the 8 bits immediately following
*      the SOP are interpreted as the length of the packet. When this bit is
*      cleared no Packet Length field is transmitted This only affects modes
*      other than 8DR, for 8DR a length field is assumed on transmission and
*      reception.
*      When used in 1Mbps mode with CRC and SOP also disabled, this allows the
*      transmition of arbitrary data patterns.
* 4:0  SOP_THRESH - SOP Correlator Threshold. This is the RX data correlator
*      threshold used when attempting to detect an SOP symbol. The threshold
*      applies independently to each of the 2 PN Codes that comprise the SOP
*      symbol. When SOP LEN is set, all 5 bits of this field are used. When
*      SOP LEN is cleared, the most significant bit is disregarded.
*
* ----------------------------------------------------------------------------
*/
#define FRAMING_CFG_ADR                                   0x10
#define FRAMING_CFG_RST                                   0xA5

/* single flag bits & multi-bit-field masks */
#define SOP_EN                                            0x80
#define SOP_LEN                                           0x40
#define LEN_EN                                            0x20
#define SOP_THRESH_MSK                                    0x1F
#define SOP_TH_SOP32                                      0x04
#define SOP_TH_SOP64                                      0x0e

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Data Threshold 32 register
* ------------------------------------------------------------------------------
* all - R/W
* 3:0  TH32 - 32 Chip Data PN Code Correlator Threshold. This register sets
*      the correlator threshold used in DSSS modes when the Data PN code
*      length is set to 32.
* -----------------------------------------------------------------------------
*/
#define DATA32_THOLD_ADR                                  0x11
#define DAT32_THRESH_RST                                  0x04
#define DAT32_THRESH_MSK                                  0x0F

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Data Threshold 64 register
* ------------------------------------------------------------------------------
* all - R/W
* 4:0  TH64 - 64 Chip Data PN Code Correlator Threshold. This register sets
*      the correlator threshold used in DSSS modes when the Data PN code
*      length is set to 64.
* -----------------------------------------------------------------------------
*/
#define DATA64_THOLD_ADR                                  0x12
#define DAT64_THRESH_RST                                  0x0A
#define DAT64_THRESH_MSK                                  0x1F
#define DATA_THOLD_DATA32   (0x05)
#define DATA_THOLD_DATA64   (0x0e)
/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* RSSI register
* ------------------------------------------------------------------------------
* A Received Signal Strength Indicator reading is taken automatically when an
* SOP symbol is detected. In addition, a RSSI reading is taken whenever the
* RSSI register is read. The contents of this register are not valid after the
* device is configured for RX mode until either an SOP is detected, or the
* register is read.
* If it is desired to measure the background RF signal strength on a channel
* before a packet has been received then the MCU should do a "dummy" RSSI read
* of this register, the result of which should be discarded. This "dummy" read
* will cause an RSSI measurement to be taken, and therefore subsequent readings
* of the register will yield valid data.
*
* all - R
* 7    SOP_RSSI - SOP RSSI Reading. When set, this bit indicates that the
*      reading in the RSSI field was taken when a SOP was detected. When
*      cleared, this bit indicates that the reading stored in the RSSI field
*      was triggered by a previous SPI read of this register.
* 5    LNA_STATE - LNA State. This bit indicates the LNA state when the RSSI
*      reading was taken. When set, this bit indicates that the LNA was
*      enabled; if clear this bit indicates that the LNA was disabled when the
*      RSSI reading was taken.
* 4:0  RSSI_LVL - RSSI Reading. This field indicates the strength of the RF
*      signal being received at the time that the reading was taken. A larger
*      value indicates a stronger signal. The signal strength measured is for
*      the RF signal on the configured channel, and is measured after the LNA.
* TODO: SYO - is there a constraint for back-to-back reads
*
* In 7-bit mode - see the RSSI Control Register - NOT FOR DATASHEET
* 7    The agc override indication
* 6:0  7 bit ADC read data.
*
* ----------------------------------------------------------------------------
*/
#define RSSI_ADR                                          0x13
#define RSSI_RST                                          0x20

/* single flag bits & multi - bit - field masks */
#define SOP_RSSI                                          0x80
#define LNA_STATE                                         0x20
#define RSSI_LVL_MSK                                      0x1F

/*-----------------------------------------------------------------------------

* -----------------------------------------------------------------------------
* EOP Control register
* -----------------------------------------------------------------------------
* If the LEN EN bit is set, then the contents of this register have no effect.
* If the LEN EN bit is cleared, then this register is used to configure how an
* EOP (end of packet) condition is detected.
*
* all - R/W
* 7    H_EN - EOP Hint Enable. When set, this bit will cause an EOP to be
*      detected if no correlations have been detected for the configured number
*      of symbol periods AND the last 2 received bytes match the calculated
*      CRC for all previously received bytes.
* 6:4  HINT_EOP - EOP Hint Symbol Count. The minimum number of symbols of
*      consecutive non-correlations at which the last 2 bytes are checked
*      against the calculated CRC to detect an EOP condition.
*      This value is defined in symbols, it will default to the number of
*      symbols that constitute a byte. So for example a length of 7 symbols,
*      when a DDR reception, CRC hint termination would occur at 4 invalid
*      symbols, and would not affect the CRC calculations.
* 3:0  EOP - EOP Symbol Count. An EOP condition is deemed to exist whenever
*      this number of consecutive non-correlations is detected.
*
* ---------------------------------------------------------------------------
*/
#define EOP_CTRL_ADR                                      0x14
#define EOP_CTRL_RST                                      0xA4

/*single flag bits & multi-bit-field masks */
#define HINT_EN                                           0x80
#define HINT_EOP_MSK                                      0x70
#define EOP_MSK                                           0x0F

/*---------------------------------------------------------------------------

* ---------------------------------------------------------------------------
* CRC Seed registers
* ---------------------------------------------------------------------------
* all - R/W
* ---------------------------------------------------------------------------
*/
#define CRC_SEED_LSB_ADR                                  0x15
#define CRC_SEED_MSB_ADR                                  0x16
#define CRC_SEED_LSB_RST                                  0x00
#define CRC_SEED_MSB_RST                                  0x00

/*CRC related values */
/* USB CRC-16 */
#define CRC_POLY_MSB                                      0x80
#define CRC_POLY_LSB                                      0x05
#define CRC_RESI_MSB                                      0x80
#define CRC_RESI_LSB                                      0x0D

/*----------------------------------------------------------------------------

* ----------------------------------------------------------------------------
* TX CRC Calculated registers
* ----------------------------------------------------------------------------
* all - R
*      TX_CRC becomes valid at the end of transmition.
*      This only applies to data packets.
* ----------------------------------------------------------------------------
*/
#define TX_CRC_LSB_ADR                                    0x17
#define TX_CRC_MSB_ADR                                    0x18

/*------------------------------------------------------------------------------

* -----------------------------------------------------------------------------
* RX CRC Field registers
* -----------------------------------------------------------------------------
* all - R
*      RX_CRC becomes valid at the completion of a reception.
*      This applies to data and ACK packets.
* -----------------------------------------------------------------------------
*/
#define RX_CRC_LSB_ADR                                    0x19
#define RX_CRC_MSB_ADR                                    0x1A
#define RX_CRC_LSB_RST                                    0xFF
#define RX_CRC_MSB_RST                                    0xFF

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Synth Offset registers
* ------------------------------------------------------------------------------
* all - R/W
*  TODO: SYO verify 12 bit length of number  - that's what is implemented now
* MSB -----
* 3:0  STRIM_MSB - The most significant 4 bits of the synthesizer trim value
* LSB -----
* 7:0  STRIM_LSB - The least significant 8 bits of the synthesizer trim value.
*      This is a 12-bit 2's complement number which may be used to trim the
*      transmit frequency of the device by up to TBD Hz. A positive value
*      increases the transmit frequency, and a negative value reduces the
*      transmit frequency.  A value of +1 increases the transmit frequency by
*      TBD ppm; a value of -1 decreases the transmit frequency by TBD ppm. A
*      value of TBD increases the transmit frequency by 1MHz; a value of TBD
*      decreases the transmit frequency by 1MHz. The maximum trim value is
*      TDB.
*
* Synthesizer offset has no effect on receive frequency.
* Note that Synthesizer offset has no effect on receive frequency.
* It is recommended that this register only be written to when the device is in
* sleep or idle modes.

* -----------------------------------------------------------------------------
*/
#define TX_OFFSET_LSB_ADR                              0x1B
#define TX_OFFSET_MSB_ADR                              0x1C
#define TX_OFFSET_LSB_RST                              0x00
#define TX_OFFSET_MSB_RST                              0x00

/* single flag bits & multi-bit-field masks */
#define STRIM_MSB_MSK                                     0x0F
#define STRIM_LSB_MSK                                     0xFF

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Mode Override register
* ------------------------------------------------------------------------------
* all - W
* 7    DIS_AUTO_SEN - Disable auto Synthesizer setup when
*      TX_CTRL_ADR(TX_GO) or TX_CTRL_ADR(RX_GO) are set.  You must
*      manually setup Synthesizer for TX using FRC_SEN and SEN_TX or RX using
*      FRC_SEN and !SEN_TX before initiating an RX or TX.
* 6    SEN_TXRXB - Synthesize for TX mode when set otherwise synthesize for RX.
*      Setting this bit causes the device to synthesize to the TX channel
*      frequency when FRC_SEN is set otherwise the RX channel frequency is used.
* 5    FRC_SEN - Manually Initiate Synthesizer.
*      Clearing this bit has no effect.  For this bit to operate correctly,
*      the crystal oscillator must previously have been started using
*      END_STATE bits for something besides END_STATE_SLEEP and then
*      FRC_END_STATE.
* 4:3  FRC_AWAKE - Setting this field controls the oscilator enable, pmu, and
*      standby.
*      3 - FRC_AWAKE awake
*      2 - don't use - invalid
*      1 - FRC_AWAKE_OFF_1 - step 1
*      0 - FRC_AWAKE_OFF_2 - step 2
*      0 - default state - allow oscillator to be controlled by END_STATE.
*      Setting FRC_AWAKE forces the oscilator to remain enabled regardless of
*      END_STATE settings.
*      In order to allow the oscillator to be controlled again you must set
*      this field to FRC_AWAKE_OFF_1 followed by setting this field to
*      FRC_AWAKE_OFF_2 atleast 25us later.
*      Verification notes: bit-4=1 forces stby=0; bit-3=1 forces xtalen=1,vdigoff=0,bgen=1
*      This has priority over FRC_END_STATE
* TODO: SYO
* 1    FRC_PA - Force Power Amplifier Enable. Setting this bit turns on the
*      internal RF power amplifier.
* 0    RST - Reset. Setting this bit causes the device to undergo a hardware
*      Reset. Clearing this bit has no effect
*
* --------------------------------------------------------------------------------------
*/
#define MODE_OVERRIDE_ADR                                 0x1D
#define MODE_OVERRIDE_RST                                 0x00

#define FRC_AWAKE                                         0x03
#define FRC_AWAKE_OFF_1                                   0x01
/* must wait 25 us from setting FRC_AWAKE_OFF_1 before setting FRC_AWAKE_OFF_2 */
#define FRC_AWAKE_OFF_2                                   0x00

/* single flag bits & multi-bit-field masks */
#define DIS_AUTO_SEN                                      0x80
#define SEN_TXRXB                                         0x40
#define FRC_SEN                                           0x20
#define FRC_AWAKE_MSK                                     0x18
#define MODE_OVRD_FRC_AWAKE                               0x18
#define MODE_OVRD_FRC_AWAKE_OFF_1                         0x08
#define MODE_OVRD_FRC_AWAKE_OFF_2                         0x00
#define RF_RST                                            0x01
#define FRC_PA                                            0x02

/*----------------------------------------------------------------------------

* ----------------------------------------------------------------------------
* RX Override register
* ----------------------------------------------------------------------------
* all - R/W
* TODO: SYO rename to SYNTH_TX_EQUALS_RX
* 7    ACK_RX - Enable ACK RX. When this bit is set, the device uses the TX
*      synth frequency rather than the RX synth frequency for the given channel
*      when [automatically] entering RX mode.
* 6    EXTEND_RX_TX - Extend RX-to-TX delay.
*  TODO:SYO I added the word automatically above to indicate that this does
*  not have to to with SEN_TXRXB or FRC_SEN.  i.e. it is only used when
*  DIS_AUTO_SEN=0.  Is this correct?  Should the bit be called something else?
* 5    MAN_RXACK - Expected Packet Type. When this bit is set, the device is
*      configured to receive an ACK packet at the data rate defined in the TX
*      configuration register. TX_CTRL_ADR(RX_GO) initiates the RX packet.
* 4    FRC_RXDR - Manual RX Data rate. When this bit is set, the Receiver will
*      ignore the data rate encoded in the SOP symbol, and will receive data
*      at the data rate defined in the TX configuration register.
* 3    DIS_CRC0 - Reject Zero-seed CRC. Setting this bit causes the receiver
*      to reject packets with a zero-seed, and accept only packets with a CRC
*      that matches the seed in the CRC seed registers.
* 2    DIS_RXCRC - Disable CRC. The RX CRC checker is disabled.
*      CRC field is not stripped off.
* 1    ACE - Accept Bad CRC. Setting this bit causes the receiver to accept
*      packets with a CRC that does not match the seed in the CRC seed
*      registers.  An ACK is to be sent regardless of the condition of recieved
*      CRC.
* RXINV is a duplicate of HILO_CONTROL(HILO_RX_INVERT)
*
* -----------------------------------------------------------------------------
*/
#define RX_OVERRIDE_ADR                                   0x1E
#define RX_OVERRIDE_RST                                   0x00

/* single flag bits & multi-bit-field masks */
#define ACK_RX                                            0x80
#define EXTEND_RX_TX                                      0x40
#define MAN_RXACK                                         0x20
#define FRC_RXDR                                          0x10
#define DIS_CRC0                                          0x08
#define DIS_RXCRC                                         0x04
#define ACE                                               0x02

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* TX Override register
* ------------------------------------------------------------------------------
* all - R/W
* TODO:NEW bits
* TODO: SYO rename to SYNTH_RX_EQUALS_TX
* 7    ACK_TX - Enable ACK TX. When this bit is set, the device uses the RX
*      synth frequency rather than the TX synth frequency for the given
*      channel when [automatically] entering TX mode.
* TODO:SYO I added the word automatically above to indicate that this does
* not have to to with SEN_TXRXB or FRC_SEN.  i.e. it is only used when
* DIS_AUTO_SEN=0.  Is this correct?
* 6    FRC_PREAMBLE - When this bit is set, the TX Modem to continuously sends
*      the preamble pattern instead of data.  TX_GO must be set to activate.
* 5    DIS_TX_RETRANS - Do not clear read pointer on TX buffer when
*      TX_CTRL_ADR(TX_GO) is set. - NOT FOR DATASHEET
* 4    MAN_TXACK - Transmit Packet Type. When this bit is set, the device is
*      configured to send an ACK packet when TX_CTRL_ADR(TX_GO) is set.
* 3    OVRRD_ACK - Use TX configuration register to determine the data rate
*      and the CRC-seed value.
* TODO:old bits
* 2    DIS_TXCRC - Disable CRC. No CRC will be transmitted at the end of any
*      data packet.
* TODO: SYO - implement
* 1    CO - Force Carrier Only. Setting this bit disables modulation of the
*      transmitted signal.
* 0    TXINV - TX Data Invert. When this bit is set, the TX bitstream is
*      inverted.
* -------------------------------------------------------------------------
*/
#define TX_OVERRIDE_ADR                                   0x1F
#define TX_OVERRIDE_RST                                   0x00

/* single flag bits & multi-bit-field masks */
#define ACK_TX_SEN                                        0x80
#define FRC_PREAMBLE                                      0x40
#define DIS_TX_RETRANS                                    0x20
#define MAN_TXACK                                         0x10
#define OVRRD_ACK                                         0x08
#define DIS_TXCRC                                         0x04
#define CO                                                0x02
#define TXINV                                             0x01


/*------------------------------------------------------------------------------
*      File Function Detail
*------------------------------------------------------------------------------

* -----------------------------------------------------------------------------
* TX Buffer - 16 bytes
* -----------------------------------------------------------------------------
* The TX Buffer is a FIFO. Writing to this file adds a byte to the buffer
* being sent. Writing more bytes to the file than the packet length in the TX
* Packet Length Register will have no effect, and these bytes will be lost
* after successful packet transmission. It is NOT possible to load two 8 byte
* packets into this register, and then transmit them sequentially by enabling
* the TX_GO bit twice; this would have the effect of sending the first 8 bytes
* twice.
* all - W
* ------------------------------------------------------------------------------
*/
#define TX_BUFFER_ADR                                         0x20

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* RX Buffer - 16 bytes
* ------------------------------------------------------------------------------
* The RX Buffer is a FIFO. Received byte may be read from the buffer at any
* time that it is not empty, but when reading from the buffer before a packet
* has been completely received care must be taken to ensure that error packets
* (for example with bad CRCs) are handled correctly.
*
* When the RX buffer is configured to be overwritten by new packets (the
* alternative is for new packets to be discarded if the RX buffer is not
* empty), similar care must be taken to verify after the packet has been read
* from the buffer that no part of it was overwritten by a newly received
* packet while the RX buffer was being read.
*
* When the RX_CFG(VLD_EN) bit is set, the bytes in the RX buffer alternate -
* the first byte read from the RX buffer is data, the second byte is a valid
* flag for each bit in the first byte, the 3rd byte is data, the 4th byte
* valid flags, etc.  The valid flag for a bit is set if the correlation
* coefficient for the bit exceeded the correlator threshold, and is cleared if
* it did not.
* all - R
*
* -----------------------------------------------------------------------------
*/
#define RX_BUFFER_ADR                                        0x21

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Framing Code - 8 bytes
* ------------------------------------------------------------------------------
* When using 32-chip Framing PN Codes, only the first 4 bytes of this register
* are used; in order to complete the file write process, these 4 bytes must be
* followed by 4 bytes of "random" data. However, a class of codes known as
* "multiplicative codes" may be used; there are 64-chip codes with good
* auto-correlation and cross-correlation properties where the least
* significant 32 chips themselves have good auto-correlation and
* cross-correlation properties when used as 32-chip codes. In this case the
* same 8-byte value may be loaded into this file and used for both 32 and
* 64-chip SOPs.
*
* When reading this file, all 8 bytes must be read; if fewer than 8 bytes are
* read from the file, the contents of the file will have been rotated by the
* number of bytes read.
* all - R/W
*
* -----------------------------------------------------------------------------
*/
#define SOP_CODE_ADR                                         0x22

/* CODESTORE_REG_SOF_RST        64'h17_ff_9e_21_36_90_c7_82 */
#define CODESTORE_BYTE7_SOF_RST                              0x17
#define CODESTORE_BYTE6_SOF_RST                              0xFF
#define CODESTORE_BYTE5_SOF_RST                              0x9E
#define CODESTORE_BYTE4_SOF_RST                              0x21
#define CODESTORE_BYTE3_SOF_RST                              0x36
#define CODESTORE_BYTE2_SOF_RST                              0x90
#define CODESTORE_BYTE1_SOF_RST                              0xC7
#define CODESTORE_BYTE0_SOF_RST                              0x82


/* TODO:DGW TBD Default SOP Code value?

*------------------------------------------------------------------------------

* -----------------------------------------------------------------------------
* Data Code - 16 bytes
* -----------------------------------------------------------------------------
* This file is ignored when using the device in 1Mbps GFSK mode. In 64-chip
* SDR mode, only the first 8 bytes are used; in order to complete the file
* write process, these 8 bytes must be followed by 8 bytes of "random" data.
* In 32-chip SDR mode, only 4 bytes are used, and in 32-chip DDR mode only 8
* bytes are used. In 64-chip DDR and 8DR modes, all 16 bytes are used. Certain
* 16-bite sequences have been calculated that provide excellent
* auto-correlation and cross-correlation properties, and it is recommended
* that such sequences be used; the default value of this register is one such
* sequence. In some applications, all devices use the same data PN codes, and
* devices and systems are addressed by using different Framing Codes; in such
* cases it may never be necessary to change the contents of this register from
* the default that is automatically loaded at Reset.
*
* When reading this file, all 16 bytes must be read; if fewer than 16 bytes
* are read from the file, the contents of the file will have been rotated by
* the number of bytes read.
* all - R/W
*
* --------------------------------------------------------------------------
*/
#define DATA_CODE_ADR                                        0x23

/* CODESTORE_REG_DCODE0_RST            64'h01_2B_F1_DB_01_32_BE_6F */
#define CODESTORE_BYTE7_DCODE0_RST                           0x01
#define CODESTORE_BYTE6_DCODE0_RST                           0x2B
#define CODESTORE_BYTE5_DCODE0_RST                           0xF1
#define CODESTORE_BYTE4_DCODE0_RST                           0xDB
#define CODESTORE_BYTE3_DCODE0_RST                           0x01
#define CODESTORE_BYTE2_DCODE0_RST                           0x32
#define CODESTORE_BYTE1_DCODE0_RST                           0xBE
#define CODESTORE_BYTE0_DCODE0_RST                           0x6F

/* CODESTORE_REG_DCODE1_RST            64'h02_F9_93_97_02_FA_5C_E3*/
#define CODESTORE_BYTE7_DCODE1_RST                           0x02
#define CODESTORE_BYTE6_DCODE1_RST                           0xF9
#define CODESTORE_BYTE5_DCODE1_RST                           0x93
#define CODESTORE_BYTE4_DCODE1_RST                           0x97
#define CODESTORE_BYTE3_DCODE1_RST                           0x02
#define CODESTORE_BYTE2_DCODE1_RST                           0xFA
#define CODESTORE_BYTE1_DCODE1_RST                           0x5C
#define CODESTORE_BYTE0_DCODE1_RST                           0xE3

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Preamble - 3 bytes
* ------------------------------------------------------------------------------
* Byte 2 - Most significant 8 chips of the preamble sequence
* Byte 1 - Least significant 8 chips of the preamble sequence
* Byte 0 - The number of repetitions of the preamble sequence that are to be
* transmitted. The preamble may be disabled by writing 0x00 to this byte.
*
* When reading this file, all 3 bytes must be read; if fewer than 3 bytes
* are read from the file, the contents of the file will have been rotated by
* the number of bytes read.
* all - R/W
*
* ------------------------------------------------------------------------------
*/
#define PREAMBLE_ADR                                      0x24
#define PREAMBLE_CODE_MSB_RST                             0x33
#define PREAMBLE_CODE_LSB_RST                             0x33
#define PREAMBLE_LEN_RST                                  0x02

/*------------------------------------------------------------------------------

* ------------------------------------------------------------------------------
* Laser Fuses - 8 bytes (2 hidden)
* ------------------------------------------------------------------------------
* It is possible to partially read this register without disruption its
* contents. In order to read the contents of this register, it is necessary to
* execute a single-byte SPI write to this address with a non-zero data stage;
* this enables the laser fused read-bias circuit. To minimize current
* consumption, the fuse bias circuit should be disabled after reading by
* executing a single-byte SPI write to this address with a zero data stage.
* By default, the bias circuit is disabled.
*
* 1st byte: 4bits version + 2 bits vendor ID + high 2 bits of Year
* 2nd byte: low 2bits of Year + 6 bits WW
* 3rd byte: high 8bits of lot#
* 4th byte: low 5bits of lot# + high 3 of Wafer#
* 5th byte: low 2bits of Wafer# + high 6 bits of X
* 6th byte: LSB of X + high 7 bits of Y
* all - R
*
* ----------------------------------------------------------------------------
*/
#define MFG_ID_ADR                                         0x25

/*-----------------------------------------------------------------------------*/

#define  AUTO_CAL_OFFSET_ADR                                0x35
#define  AUTO_CAL_TIME_ADR                                  0x32
#define  CLK_OFFSET_ADR                                     0x27
#define  CLK_EN_ADR                                         0x28

#define  RX_ABORT_ADR                                       0x29
#define  ABORT_EN                                           0x20

#define ANALOG_CTRL_ADR (0x39)
#define ALL_SLOW    (0x01)
/**************************Rf6936 factory define end*************************/


#define PA_EN_CLR       GPIOB->ODR &= (~GPIO_Pin_2)     //Rf6936射频模块关闭
#define PA_EN_SET       GPIOB->ODR |= (GPIO_Pin_2)      //Rf6936射频模块使能

#define  SETB_RSET_SET    GPIOD->ODR |= GPIO_Pin_5      //SETB_RSET_SET() (PD_ODR_ODR5)
#define  SETB_RSET_CLR    GPIOD->ODR &= (~GPIO_Pin_5)   //SETB_RSET_SET() (PD_ODR_ODR5)


#define ENABLE_RF_IRQ_INT()  GPIOD->CR2 |= 0x10;        //PD_CR2_C24 = 1
#define DISABLE_RF_IRQ_INT() GPIOD->CR2 &= 0xef         //PD_CR2_C24 = 0


#define LS_CHANNEL_OFFSET                  2


/******************************结构体定义***********************************/


/*******************************函数声明*************************************/

uint8_t Drivers_Rf6936Init(PCONTROL_CONFIG pControlerConfig, uint8_t mRemoval);
void Drivers_Rf6936RegWrite(uint8_t pAddr, uint8_t mData);           //RF6936模块写寄存器
uint8_t Drivers_Rf6936RegRead(uint8_t pAddr);                        //RF6936模块读寄存器
void Drivers_Rf6936Wake();                                           //RF6936模块唤醒
void Drivers_Rf6936RxStart(void);
void Drivers_Rf6936RxDIdle(void);
void Drivers_Rf6396RadioSetSopPnCode(uint8_t remotePnAddr[]);
void Drivers_Rf6396RadioSetChannel(uint8_t mChannel);
void Drivers_Rf6936RadioDataPnCode(uint8_t remoteDataPnAddr[]);
void Drivers_Rf6936RadioSetPreamPnCode(uint8_t remotePreamAddr[], uint8_t mRemoval);
void Drivers_Rf6936RadioSetMfgChannel(uint32_t mChannel);
void Drivers_Rf6936SetPowerAndBitrate(uint8_t mRf0BaundRate, uint8_t remotePower);
void Drivers_Rf6936CheckCryStalStableBack(void);
void Drivers_Rf6936CheckCryStalStable(void);
void Drivers_Rf6936RemotePowerTest(void);                                 //发射功率测试状态
void Drivers_Rf6936RadioIniMap(PCONTROL_CONFIG pControlerConfig);
void Drivers_Rf6936SelectChannel(PCONTROL_CONFIG pControlerConfig);
uint8_t Drivers_Rf6936CheckRssiValue(uint32_t value);
uint8_t Drivers_Rf6936CollisoHneadDeal();
uint8_t Drivers_Rf6936SendPackage(uint8_t retryTimes, uint8_t sendLen , uint8_t packageType, uint8_t sendNodeCount, uint8_t sendPackCount, SCONTROL_SYMPLE tagCng);
uint8_t Drivers_Rf6936GetChannelTimer();

#endif   //_DRIVERS_FR6936_H_
