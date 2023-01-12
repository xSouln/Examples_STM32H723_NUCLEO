//==============================================================================
#ifndef REGISTERS_STM32H7XX_UART_H_
#define REGISTERS_STM32H7XX_UART_H_
//==============================================================================
#include "Registers/registers_config.h"
#ifdef REGISTERS_STM32H7XX_ENABLE
//==============================================================================
#include <stdint.h>
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t Enable: 1; //USART_CR1_UE /*!< USART Enable */
    /* 0x00000002 */ uint32_t EnableInStopMode: 1; //USART_CR1_UESM /*!< USART Enable in STOP Mode */
    /* 0x00000004 */ uint32_t ReceiverEnable: 1; //USART_CR1_RE /*!< Receiver Enable */
    /* 0x00000008 */ uint32_t TransmitterEnable: 1; //USART_CR1_TE /*!< Transmitter Enable */
	
    /* 0x00000010 */ uint32_t IDLE_InterruptEnable: 1; //USART_CR1_IDLEIE /*!< IDLE Interrupt Enable */
    /* 0x00000020 */ uint32_t RxNotEmptyInterruptEnable: 1; //USART_CR1_RXNEIE /*!< RXNE Interrupt Enable */
    /* 0x00000040 */ uint32_t TxCompleteInterruptEnable: 1; //USART_CR1_TCIE /*!< Transmission Complete Interrupt Enable */
    /* 0x00000080 */ uint32_t TxEmptyInterruptEnable: 1; //USART_CR1_TXEIE /*!< TXE Interrupt Enable */
    
    /* 0x00000100 */ uint32_t PE_InterruptEnable: 1; //USART_CR1_PEIE /*!< PE Interrupt Enable */
    /* 0x00000200 */ uint32_t ParitySelection: 1; //USART_CR1_PS /*!< Parity Selection */
    /* 0x00000400 */ uint32_t ParityControlEnable: 1; //USART_CR1_PCE /*!< Parity Control Enable */
    /* 0x00000800 */ uint32_t WakeupMethod: 1; //USART_CR1_WAKE /*!< Receiver Wakeup method */
	
    /* 0x00001000 */ uint32_t WordLength: 1; //USART_CR1_M0 /*!< Word length - Bit 0 */
    /* 0x00002000 */ uint32_t MuteModeEnable: 1; //USART_CR1_UE /*!< Mute Mode Enable */
    /* 0x00004000 */ uint32_t CharacterMatchInterruptEnable: 1; //USART_CR1_CMIE /*!< Character match interrupt enable */
    /* 0x00008000 */ uint32_t OversamplingBy8: 1; //USART_CR1_OVER8 /*!< Oversampling by 8-bit or 16-bit mode */
	
    /* 0x00010000 */ uint32_t DriverEnableDeassertionTime: 5; //USART_CR1_DEDT /*!< DEDT[4:0] bits (Driver Enable Deassertion Time) */
	
    /* 0x00200000 */ uint32_t DriverEnableAssertionTime: 5; //USART_CR1_DEAT /*!< DEAT[4:0] bits (Driver Enable Assertion Time) */
	
    /* 0x04000000 */ uint32_t ReceiveTimeOutInterruptEnable: 1; //USART_CR1_RTOIE /*!< Receive Time Out interrupt enable */
    /* 0x08000000 */ uint32_t EndOfBlockInterruptEnable: 1; //USART_CR1_EOBIE /*!< End of Block interrupt enable */
	
    /* 0x10000000 */ uint32_t WordLength1: 1; //USART_CR1_M1 /*!< Word length - Bit 1 */
    /* 0x20000000 */ uint32_t FIFO_ModeEnable: 1; //USART_CR1_FIFOEN /*!< FIFO mode enable */
    /* 0x40000000 */ uint32_t TXFIFO_EmptyInterruptEnable: 1; //USART_CR1_TXFEIE /*!< TXFIFO empty interrupt enable */
    /* 0x80000000 */ uint32_t RXFIFO_FullInterruptEnable: 1; //USART_CR1_RXFFIE /*!< RXFIFO Full interrupt enable */
  };
  uint32_t Value;
  
} REG_UART_CR1_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t SynchronousSlaveModeEnable: 3; //USART_CR2_SLVEN /*!< Synchronous Slave mode Enable */
    /* 0x00000008 */ uint32_t NegativeSlaveSelectPinLevel: 1; //USART_CR2_DIS_NSS /*!< Negative Slave Select (NSS) pin management */
	
    /* 0x00000010 */ uint32_t AddressDetection : 1; //USART_CR2_ADDM7 /*!< 7-bit or 4-bit Address Detection */
    /* 0x00000020 */ uint32_t LIN_BreakDetectionLength: 1; //USART_CR2_LBDL /*!< LIN Break Detection Length */    
    /* 0x00000040 */ uint32_t LIN_BreakDetectionInterruptEnable: 2; //USART_CR2_LBDIE /*!< LIN Break Detection Interrupt Enable */

    /* 0x00000100 */ uint32_t LastBitClockPulse: 1; //USART_CR2_LBCL /*!< Last Bit Clock pulse */
    /* 0x00000200 */ uint32_t ClockPhase: 1; //USART_CR2_CPHA /*!< Clock Phase */
    /* 0x00000400 */ uint32_t ClockPolarity: 1; //USART_CR2_CPOL /*!< Clock Polarity */
    /* 0x00000800 */ uint32_t ClockEnable: 1; //USART_CR2_CLKEN /*!< Clock Enable */

    /* 0x00001000 */ uint32_t StopBits: 2; //USART_CR2_STOP /*!< STOP[1:0] bits (STOP bits) */
    /* 0x00004000 */ uint32_t LIN_ModeEnable: 1; //USART_CR2_LINEN /*!< LIN mode enable */
    /* 0x00008000 */ uint32_t SwapRxTx: 1; //USART_CR2_SWAP /*!< SWAP TX/RX pins */

    /* 0x00010000 */ uint32_t RxPinLevelInversion: 1; //USART_CR2_RXINV /*!< RX pin active level inversion */
    /* 0x00020000 */ uint32_t TxPinLevelInversion: 1; //USART_CR2_TXINV /*!< TX pin active level inversion */
    /* 0x00040000 */ uint32_t BinaryDataInversion: 1; //USART_CR2_DATAINV /*!< Binary data inversion */
    /* 0x00080000 */ uint32_t MostSignificantBitFirst: 1; //USART_CR2_MSBFIRST /*!< Most Significant Bit First */

    /* 0x00100000 */ uint32_t AutoBaudRateEnable: 1; //USART_CR2_ABREN /*!< Auto Baud-Rate Enable*/
    /* 0x00200000 */ uint32_t AutoBaudRateMode: 2; //USART_CR2_ABRMODE /*!< ABRMOD[1:0] bits (Auto Baud-Rate Mode) */
    /* 0x00800000 */ uint32_t ReceiverTimeOutEnable: 1; //USART_CR2_RTOEN /*!< Receiver Time-Out enable */

    /* 0x01000000 */ uint32_t AddressUSART_Node: 1; //USART_CR2_ADD /*!< Address of the USART node */
  };
  uint32_t Value;
  
} REG_UART_CR2_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t ErrorInterruptEnable: 1; //USART_CR3_EIE /*!< Error Interrupt Enable */
    /* 0x00000002 */ uint32_t IrDA_Enable: 1; //USART_CR3_IREN /*!< IrDA mode Enable */
    /* 0x00000004 */ uint32_t IrDA_LowPower : 1; //USART_CR3_IRLP /*!< IrDA Low-Power */
    /* 0x00000008 */ uint32_t HalfDuplexSelection: 1; //USART_CR3_HDSEL /*!< Half-Duplex Selection */

    /* 0x00000010 */ uint32_t SmartcardNACK_Enable: 1; //USART_CR3_NACK /*!< SmartCard NACK enable */
    /* 0x00000020 */ uint32_t SmartcardEnable: 1; //USART_CR3_SCEN /*!< SmartCard mode enable */
    /* 0x00000040 */ uint32_t RxDMA_Enable: 1; //USART_CR3_DMAR /*!< DMA Enable Receiver */
    /* 0x00000080 */ uint32_t TxDMA_Enable: 1; //USART_CR3_DMAT /*!< DMA Enable Transmitter */

    /* 0x00000100 */ uint32_t RTS_Enable: 1; //USART_CR3_RTSE /*!< RTS Enable */
    /* 0x00000200 */ uint32_t CTS_Enable: 1; //USART_CR3_CTSE /*!< CTS Enable */
    /* 0x00000400 */ uint32_t CTS_InterruptEnable: 1; //USART_CR3_CTSIE /*!< CTS Interrupt Enable */
    /* 0x00000800 */ uint32_t OneSampleBitMethodEnable: 1; //USART_CR3_ONEBIT /*!< One sample bit method enable */

    /* 0x00001000 */ uint32_t OverrunDisable: 1; //USART_CR3_OVRDIS /*!< Overrun Disable */
    /* 0x00002000 */ uint32_t DMA_DisableReceptionError: 1; //USART_CR3_DDRE /*!< DMA Disable on Reception Error */
    /* 0x00004000 */ uint32_t DriverEnableMode: 1; //USART_CR3_DEM /*!< Driver Enable Mode */
    /* 0x00008000 */ uint32_t DriverEnablePolaritySelection: 2; //USART_CR3_DEP /*!< Driver Enable Polarity Selection */
	
    /* 0x00020000 */ uint32_t SmartCardAutoRetryCount: 3; //USART_CR3_SCARCNT /*!< SCARCNT[2:0] bits (SmartCard Auto-Retry Count) */

    /* 0x00100000 */ uint32_t WakeUPInterruptFlagSelection: 2; //USART_CR3_WUS /*!< WUS[1:0] bits (Wake UP Interrupt Flag Selection) */
    /* 0x00400000 */ uint32_t WakeUpInterruptEnable: 1; //USART_CR3_WUFIE /*!< Wake Up Interrupt Enable */
    /* 0x00800000 */ uint32_t TXFIFO_ThresholdInterruptEnabl: 1; //USART_CR3_TXFTIE /*!< TXFIFO threshold interrupt enable */

    /* 0x01000000 */ uint32_t TransmissionCompleteInterruptEnabl: 1; //USART_CR3_TCBGTIE /*!< Transmission Complete before guard time, interrupt enable */
    /* 0x02000000 */ uint32_t RXFIFO_ThresholdConfiguration: 3; //USART_CR3_RXFTCFG /*!< RXFTCFG [2:0]Receive FIFO threshold configuration */

    /* 0x10000000 */ uint32_t RXFIFO_ThresholdInterruptEnable: 1; //USART_CR3_RXFTIE /*!< RXFIFO threshold interrupt enable */
    /* 0x20000000 */ uint32_t TXFIFO_ThresholdConfiguration: 3; //USART_CR3_TXFTCFG /*!< TXFIFO [2:0] threshold configuration */

  };
  uint32_t Value;
  
} REG_UART_CR3_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x0000000F */ uint32_t Fraction: 4; //USART_BRR_DIV_FRACTION /*!< Fraction of USARTDIV */
    /* 0x0000FFF0 */ uint32_t Mantissa: 12; //USART_BRR_DIV_MANTISSA /*!< Mantissa of USARTDIV */
  };
  uint32_t Value;
  
} REG_UART_BRR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x000000FF */ uint32_t Prescaler: 8; //USART_GTPR_PSC /*!< PSC[7:0] bits (Prescaler value) */
    /* 0x0000FFF0 */ uint32_t GuardTime: 8; //USART_GTPR_GT /*!< GT[7:0] bits (Guard time value) */
  };
  uint32_t Value;
  
} REG_UART_GTPR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00FFFFFF */ uint32_t ReceiverTimeOut: 8; //USART_RTOR_RTO /*!< Receiver Time Out Value */
    /* 0xFF000000 */ uint32_t BlockLength: 8; //USART_RTOR_BLEN /*!< Block Length */
  };
  uint32_t Value;
  
} REG_UART_RTOR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t AutoBaudRateRequest: 1; //USART_RQR_ABRRQ /*!<Auto-Baud Rate Request */
    /* 0x00000002 */ uint32_t SendBreakReques: 1; //USART_RQR_SBKRQ /*!< Send Break Request */
    /* 0x00000004 */ uint32_t MuteModeRequest : 1; //USART_RQR_MMRQ /*!< Mute Mode Request */
    /* 0x00000008 */ uint32_t ReceiveDataFlushRequest: 1; //USART_RQR_RXFRQ /*!< Receive Data flush Request */

    /* 0x00000010 */ uint32_t TransmitDataFlushRequest: 1; //USART_RQR_TXFRQ /*!< Transmit data flush Request */
  };
  uint32_t Value;
  
} REG_UART_RQR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t ParityError: 1; //USART_ISR_PE /*!< Parity Error */
    /* 0x00000002 */ uint32_t FramingError: 1; //USART_ISR_FE /*!< Framing Error */
    /* 0x00000004 */ uint32_t NoiseDetected: 1; //USART_ISR_NE /*!< Noise detected Flag */
    /* 0x00000008 */ uint32_t OverRunError: 1; //USART_ISR_ORE /*!< OverRun Error */

    /* 0x00000010 */ uint32_t IDLE_LineDetected: 1; //USART_SR_IDLE /*!< IDLE line detected */
    /* 0x00000020 */ uint32_t RxNotEmpty: 1; //USART_ISR_RXNE /*!< Read Data Register Not Empty */
    /* 0x00000040 */ uint32_t TxComplete: 1; //USART_ISR_TC /*!< Transmission Complete */
    /* 0x00000080 */ uint32_t TxEmpty: 1; //USART_ISR_TXE  /*!< Transmit Data Register Empty */

    /* 0x00000100 */ uint32_t LIN_BreakDetection: 1; //USART_ISR_LBDF /*!< LIN Break Detection Flag */
    /* 0x00000200 */ uint32_t CTS_Interrupt: 1; //USART_ISR_CTSIF /*!< CTS interrupt flag */
    /* 0x00000400 */ uint32_t CTS_Flag: 1; //USART_ISR_CTS /*!< CTS flag */
    /* 0x00000800 */ uint32_t ReceiverTimeOut: 1; //USART_ISR_RTOF /*!< Receiver Time Out */

    /* 0x00001000 */ uint32_t EndOfBlockFlag: 1; //USART_ISR_EOBF /*!< End Of Block Flag */
    /* 0x00002000 */ uint32_t SPI_SlaveUnderrunError: 1; //USART_ISR_UDR /*!< SPI slave underrun error flag */
    /* 0x00004000 */ uint32_t AutoBaudRateError: 1; //USART_ISR_ABRE /*!< Auto-Baud Rate Error */
    /* 0x00008000 */ uint32_t AutoBaudRate: 1; //USART_ISR_ABRF /*!< Auto-Baud Rate Flag */

    /* 0x00010000 */ uint32_t BusyFlag: 1; //USART_ISR_BUSY /*!< Busy Flag */
    /* 0x00020000 */ uint32_t CharacterMatch: 1; //USART_ISR_CMF /*!< Character Match Flag */
    /* 0x00040000 */ uint32_t SendBreak: 1; //USART_ISR_SBKF /*!< Send Break Flag */
    /* 0x00080000 */ uint32_t ReceiveWakeUpMuteMode: 1; //USART_ISR_RWU /*!< Receive Wake Up from mute mode Flag */

    /* 0x00100000 */ uint32_t ReceiveWakeUpStopMode: 1; //USART_ISR_WUF /*!< Wake Up from stop mode Flag */
    /* 0x00200000 */ uint32_t TransmitEnableAcknowledge: 1; //USART_ISR_WUF /*!< Transmit Enable Acknowledge Flag */
    /* 0x00400000 */ uint32_t ReceiveEnableAcknowledge: 1; //USART_ISR_REACK /*!< Receive Enable Acknowledge Flag */
    /* 0x00800000 */ uint32_t TXFIFO_Empty: 1; //USART_ISR_TXFE /*!< TXFIFO Empty */

    /* 0x01000000 */ uint32_t RXFIFO_Full: 1; //USART_ISR_RXFF /*!< TXFIFO Empty */
    /* 0x02000000 */ uint32_t TransmissionComplete: 1; //USART_ISR_TCBGT /*!< Transmission complete before guard time Flag */
    /* 0x04000000 */ uint32_t RXFIFO_Threshold: 1; //USART_ISR_RXFT /*!< RXFIFO threshold Flag */
    /* 0x08000000 */ uint32_t TXFIFO_Threshold: 1; //USART_ISR_TXFT /*!< TXFIFO threshold Flag */

  };
  uint32_t Value;
  
} REG_UART_ISR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t ParityError: 1; //USART_ICR_PECF /*!< Parity Error Clear Flag */
    /* 0x00000002 */ uint32_t FramingError: 1; //USART_ICR_FECF /*!< Framing Error Clear Flag */
    /* 0x00000004 */ uint32_t NoiseDetected: 1; //USART_ICR_NCF /*!< Noise detected Clear Flag */
    /* 0x00000008 */ uint32_t OverRunError: 1; //USART_ICR_ORECF /*!< OverRun Error Clear Flag */

    /* 0x00000010 */ uint32_t IDLE_LineDetected: 1; //USART_ICR_IDLECF /*!< IDLE line detected Clear Flag */
    /* 0x00000020 */ uint32_t TXFIFO_Empty: 1; //USART_ICR_TXFECF /*!< TXFIFO empty clear flag */
    /* 0x00000040 */ uint32_t TxComplete: 1; //USART_ICR_TCCF /*!< Transmission Complete Clear Flag */
    /* 0x00000080 */ uint32_t TxCompleteBeforeGuard: 1; //USART_ICR_TCBGT  /*!< Transmission complete before guard time Clear Flag */

    /* 0x00000100 */ uint32_t LIN_BreakDetection: 1; //USART_ICR_LBDCF /*!< LIN Break Detection Clear Flag */
    /* 0x00000200 */ uint32_t CTS_Interrupt: 2; //USART_ICR_CTSCF /*!< CTS Interrupt Clear Flag */
    /* 0x00000800 */ uint32_t ReceiverTimeOut: 1; //USART_ICR_RTOCF /*!< Receiver Time Out Clear Flag */

    /* 0x00001000 */ uint32_t EndOfBlock: 1; //USART_ICR_EOBCF /*!< End Of Block Clear Flag */
    /* 0x00002000 */ uint32_t SPI_SlaveUnderrunError: 1; //USART_ICR_UDRCF /*!< SPI slave underrun clear flag */

    /* 0x00004000 */ uint32_t Free14_17: 4; //Free14_17

    /* 0x00020000 */ uint32_t CharacterMatch: 1; //USART_ICR_CMCF /*!< Character Match Clear Flag */
    /* 0x00040000 */ uint32_t Free18_19: 2; //Free18_19

    /* 0x00100000 */ uint32_t WakeUpFromStopMode: 1; //USART_ICR_WUCF /*!< Wake Up from stop mode Clear Flag */

  };
  uint32_t Value;
  
} REG_UART_ICR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x000001FF */ uint32_t Data: 9; //USART_RDR_RDR /*!< RDR[8:0] bits (Receive Data value) */
  };
  uint32_t Value;
  
} REG_UART_RDR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x000001FF */ uint32_t Data: 9; //USART_TDR_TDR /*!< TDR[8:0] bits (Transmit Data value) */
  };
  uint32_t Value;
  
} REG_UART_TDR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x0000000F */ uint32_t ClockPrescaler: 4; //USART_PRESC_PRESCALER /*!< PRESCALER[3:0] bits (Clock prescaler) */
  };
  uint32_t Value;
  
} REG_UART_PRESC_T;
//==============================================================================
typedef struct
{
  volatile REG_UART_CR1_T Control1;/*!< USART Control register 1,                 Address offset: 0x00 */
  volatile REG_UART_CR2_T Control2;/*!< USART Control register 2,                 Address offset: 0x04 */
  volatile REG_UART_CR3_T Control3;/*!< USART Control register 3,                 Address offset: 0x08 */
  volatile REG_UART_BRR_T BaudRate;/*!< USART Baud rate register,                 Address offset: 0x0C */

  volatile REG_UART_GTPR_T GuardTimeAndPrescaler;/*!< USART Guard time and prescaler register,  Address offset: 0x10 */
  volatile REG_UART_RTOR_T ReceiverTimeOut;/*!< USART Receiver Time Out register,         Address offset: 0x14 */
  volatile REG_UART_RQR_T Request;/*!< USART Request register,                   Address offset: 0x18 */
  volatile REG_UART_ISR_T InterruptAndStatus;/*!< USART Interrupt and status register,      Address offset: 0x1C */

  volatile REG_UART_ICR_T InterruptClear;/*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
  volatile uint32_t RxData;/*!< USART Receive Data register,              Address offset: 0x24 */
  volatile uint32_t TxData;/*!< USART Transmit Data register,             Address offset: 0x28 */
  volatile REG_UART_PRESC_T ClockPrescaler;/*!< USART clock Prescaler register,           Address offset: 0x2C */
  
} REG_UART_T; //USART_TypeDef
//==============================================================================
#endif //REGISTERS_STM32H7XX_UART_H_
#endif //REGISTERS_STM32H7XX_ENABLE
