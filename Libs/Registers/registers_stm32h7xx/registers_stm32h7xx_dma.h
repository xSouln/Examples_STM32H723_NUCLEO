//==============================================================================
#ifndef REGISTERS_STM32H7XX_DMA_H_
#define REGISTERS_STM32H7XX_DMA_H_
//==============================================================================
#include "Registers/registers_config.h"
#ifdef REGISTERS_STM32H7XX_ENABLE
//==============================================================================
//typedef struct
//{
//  __IO uint32_t CR;     /*!< DMA stream x configuration register      */
//  __IO uint32_t NDTR;   /*!< DMA stream x number of data register     */
//  __IO uint32_t PAR;    /*!< DMA stream x peripheral address register */
//  __IO uint32_t M0AR;   /*!< DMA stream x memory 0 address register   */
//  __IO uint32_t M1AR;   /*!< DMA stream x memory 1 address register   */
//  __IO uint32_t FCR;    /*!< DMA stream x FIFO control register       */
//} DMA_Stream_TypeDef;
/*
struct
{
  DMA_Stream_TypeDef *Stream;
  DMAMUX_Channel_TypeDef *Channel;
  
  REG_DMA_Stream_T *fStream;
  REG_DMA_T *DMA;
  
} DMA_STATE;
*/
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t StreamEnable: 1; //DMA_SxCR_EN /*!< Stream enable / flag stream ready when read low */
    /* 0x00000002 */ uint32_t DirectModeErrorInterruptEnable: 1; //DMA_SxCR_DMEIE /*!< Direct mode error interrupt enable */
    /* 0x00000004 */ uint32_t TransferErrorInterruptEnable: 1; //DMA_SxCR_TEIE /*!< Transfer error interrupt enable */
    /* 0x00000008 */ uint32_t HalfTransferInterruptEnable: 1; //DMA_SxCR_HTIE /*!< Half transfer interrupt enable */

    /* 0x00000010 */ uint32_t TransferCompleteInterruptEnable: 1; //DMA_SxCR_TCIE /*!< Transfer complete interrupt enable */
    /* 0x00000020 */ uint32_t PeripheralFlowController: 1; //DMA_SxCR_PFCTRL /*!< Peripheral flow controller */
    /* 0x00000040 */ uint32_t DataTransferDirection: 2; //DMA_SxCR_DIR /*!< Data transfer direction */

    /* 0x00000100 */ uint32_t CircularMod: 1; //DMA_SxCR_CIRC /*!< Circular mode */
    /* 0x00000200 */ uint32_t PeripheralIncrementMode: 1; //DMA_SxCR_PINC /*!< Peripheral increment mode */
    /* 0x00000400 */ uint32_t MemoryIncrementMode: 1; //DMA_SxCR_MINC /*!< Memory increment mode */
    /* 0x00000800 */ uint32_t PeripheralDataSize: 2; //DMA_SxCR_PSIZE /*< Peripheral data size */

    /* 0x00002000 */ uint32_t MemoryDataSize: 2; //DMA_SxCR_MSIZE /*!< Memory data size */
    /* 0x00008000 */ uint32_t PeripheralIncrementOffsetSize: 1; //DMA_SxCR_PINCOS /*!< Peripheral increment offset size */

    /* 0x00010000 */ uint32_t PriorityLevel: 2; //DMA_SxCR_PL /*!< Priority level */
    /* 0x00040000 */ uint32_t DoubleBufferMode: 1; //DMA_SxCR_DBM /*!< Double buffer mode */
    /* 0x00080000 */ uint32_t CurrentTarget: 1; //DMA_SxCR_CT /*!< Current target (only in double buffer mode) */

    /* 0x00100000 */ uint32_t Free20: 1; //Free20
    /* 0x00200000 */ uint32_t PeripheralBurstTransferConfiguration: 2; //DMA_SxCR_PBURST /*!< Peripheral burst transfer configuration */
    /* 0x00800000 */ uint32_t MemoryBurstTransferConfiguration: 2; //DMA_SxCR_MBURST /*!< Memory burst transfer configuration */

  };
  uint32_t Value;

} REG_DMA_SxCR_T;
//==============================================================================
typedef union{
  struct{
    /* 0x00000001 */ uint32_t Stream0_FIFO_Error: 2; //DMA_LISR_FEIF0 /*!<  Stream 0 FIFO error interrupt flag */
    /* 0x00000004 */ uint32_t Stream0_DirectModeError: 1; //DMA_LISR_DMEIF0 /*!<  Stream 0 direct mode error interrupt flag */
    /* 0x00000008 */ uint32_t Stream0_TransferError: 1; //DMA_LISR_TEIF0 /*!<  Stream 0 transfer error interrupt flag */
    /* 0x00000010 */ uint32_t Stream0_HalfTransferComplite: 1; //DMA_LISR_HTIF0 /*!<  Stream 0 half transfer interrupt flag */
    /* 0x00000020 */ uint32_t Stream0_TransferComplite: 1; //DMA_LISR_TCIF0 *!<  Stream 0 transfer complete interrupt flag */

    /* 0x00000040 */ uint32_t Stream1_FIFO_Error: 2; //DMA_LISR_FEIF1 /*!<  Stream 1 FIFO error interrupt flag */
    /* 0x00000100 */ uint32_t Stream1_DirectModeError: 1; //DMA_LISR_DMEIF1 /*!<  Stream 1 direct mode error interrupt flag */
    /* 0x00000200 */ uint32_t Stream1_TransferError: 1; //DMA_LISR_TEIF1 /*!<  Stream 1 transfer error interrupt flag */
    /* 0x00000400 */ uint32_t Stream1_HalfTransferComplite: 1; //DMA_LISR_HTIF1 /*!<  Stream 1 half transfer interrupt flag */
    /* 0x00000800 */ uint32_t Stream1_TransferComplite: 1; //DMA_LISR_TCIF1 *!<  Stream 1 transfer complete interrupt flag */

    /* 0x00001000 */ uint32_t Free12_15: 4; //Free12_15

    /* 0x00010000 */ uint32_t Stream2_FIFO_Error: 2; //DMA_LISR_FEIF2 /*!<  Stream 2 FIFO error interrupt flag */
    /* 0x00040000 */ uint32_t Stream2_DirectModeError: 1; //DMA_LISR_DMEIF2 /*!<  Stream 2 direct mode error interrupt flag */
    /* 0x00080000 */ uint32_t Stream2_TransferError: 1; //DMA_LISR_TEIF2 /*!<  Stream 2 transfer error interrupt flag */
    /* 0x00100000 */ uint32_t Stream2_HalfTransferComplite: 1; //DMA_LISR_HTIF2 /*!<  Stream 2 half transfer interrupt flag */
    /* 0x00200000 */ uint32_t Stream2_TransferComplite: 1; //DMA_LISR_TCIF2 *!<  Stream 2 transfer complete interrupt flag */

    /* 0x00400000 */ uint32_t Stream3_FIFO_Error: 2; //DMA_LISR_FEIF3 /*!<  Stream 3 FIFO error interrupt flag */
    /* 0x01000000 */ uint32_t Stream3_DirectModeError: 1; //DMA_LISR_DMEIF3 /*!<  Stream 3 direct mode error interrupt flag */
    /* 0x02000000 */ uint32_t Stream3_TransferError: 1; //DMA_LISR_TEIF3 /*!<  Stream 3 transfer error interrupt flag */
    /* 0x04000000 */ uint32_t Stream3_HalfTransferComplite: 1; //DMA_LISR_HTIF3 /*!<  Stream 3 half transfer interrupt flag */
    /* 0x08000000 */ uint32_t Stream3_TransferComplite: 1; //DMA_LISR_TCIF3 *!<  Stream 3 transfer complete interrupt flag */

  };
  uint32_t Value;

} REG_DMA_LISR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t Stream4_FIFO_Error: 2; //DMA_LISR_FEIF4 /*!<  Stream 4 FIFO error interrupt flag */
    /* 0x00000004 */ uint32_t Stream4_DirectModeError: 1; //DMA_LISR_DMEIF4 /*!<  Stream 4 direct mode error interrupt flag */
    /* 0x00000008 */ uint32_t Stream4_TransferError: 1; //DMA_LISR_TEIF4 /*!<  Stream 4 transfer error interrupt flag */
    /* 0x00000010 */ uint32_t Stream4_HalfTransferComplite: 1; //DMA_LISR_HTIF4 /*!<  Stream 4 half transfer interrupt flag */
    /* 0x00000020 */ uint32_t Stream4_TransferComplite: 1; //DMA_LISR_TCIF4 *!<  Stream 4 transfer complete interrupt flag */

    /* 0x00000040 */ uint32_t Stream5_FIFO_Error: 2; //DMA_LISR_FEIF5 /*!<  Stream 5 FIFO error interrupt flag */
    /* 0x00000100 */ uint32_t Stream5_DirectModeError: 1; //DMA_LISR_DMEIF5 /*!<  Stream 5 direct mode error interrupt flag */
    /* 0x00000200 */ uint32_t Stream5_TransferError: 1; //DMA_LISR_TEIF5 /*!<  Stream 5 transfer error interrupt flag */
    /* 0x00000400 */ uint32_t Stream5_HalfTransferComplite: 1; //DMA_LISR_HTIF5 /*!<  Stream 5 half transfer interrupt flag */
    /* 0x00000800 */ uint32_t Stream5_TransferComplite: 1; //DMA_LISR_TCIF5 *!<  Stream 5 transfer complete interrupt flag */

    /* 0x00001000 */ uint32_t Free12_15: 4; //Free12_15

    /* 0x00010000 */ uint32_t Stream6_FIFO_Error: 2; //DMA_LISR_FEIF6 /*!<  Stream 6 FIFO error interrupt flag */
    /* 0x00040000 */ uint32_t Stream6_DirectModeError: 1; //DMA_LISR_DMEIF6 /*!<  Stream 6 direct mode error interrupt flag */
    /* 0x00080000 */ uint32_t Stream6_TransferError: 1; //DMA_LISR_TEIF6 /*!<  Stream 6 transfer error interrupt flag */
    /* 0x00100000 */ uint32_t Stream6_HalfTransferComplite: 1; //DMA_LISR_HTIF6 /*!<  Stream 6 half transfer interrupt flag */
    /* 0x00200000 */ uint32_t Stream6_TransferComplite: 1; //DMA_LISR_TCIF6 *!<  Stream 6 transfer complete interrupt flag */

    /* 0x00400000 */ uint32_t Stream7_FIFO_Error: 2; //DMA_LISR_FEIF7 /*!<  Stream 7 FIFO error interrupt flag */
    /* 0x01000000 */ uint32_t Stream7_DirectModeError: 1; //DMA_LISR_DMEIF7 /*!<  Stream 7 direct mode error interrupt flag */
    /* 0x02000000 */ uint32_t Stream7_TransferError: 1; //DMA_LISR_TEIF7 /*!<  Stream 7 transfer error interrupt flag */
    /* 0x04000000 */ uint32_t Stream7_HalfTransferComplite: 1; //DMA_LISR_HTIF7 /*!<  Stream 7 half transfer interrupt flag */
    /* 0x08000000 */ uint32_t Stream7_TransferComplite: 1; //DMA_LISR_TCIF7 *!<  Stream 7 transfer complete interrupt flag */

  };
  uint32_t Value;

} REG_DMA_HISR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t Stream0_FIFO_Error: 2; //DMA_LISR_FEIF0 /*!<  Stream 0 FIFO error interrupt flag */
    /* 0x00000004 */ uint32_t Stream0_DirectModeError: 1; //DMA_LISR_DMEIF0 /*!<  Stream 0 direct mode error interrupt flag */
    /* 0x00000008 */ uint32_t Stream0_TransferError: 1; //DMA_LISR_TEIF0 /*!<  Stream 0 transfer error interrupt flag */
    /* 0x00000010 */ uint32_t Stream0_HalfTransferComplite: 1; //DMA_LISR_HTIF0 /*!<  Stream 0 half transfer interrupt flag */
    /* 0x00000020 */ uint32_t Stream0_TransferComplite: 1; //DMA_LISR_TCIF0 *!<  Stream 0 transfer complete interrupt flag */

    /* 0x00000040 */ uint32_t Stream1_FIFO_Error: 2; //DMA_LISR_FEIF1 /*!<  Stream 1 FIFO error interrupt flag */
    /* 0x00000100 */ uint32_t Stream1_DirectModeError: 1; //DMA_LISR_DMEIF1 /*!<  Stream 1 direct mode error interrupt flag */
    /* 0x00000200 */ uint32_t Stream1_TransferError: 1; //DMA_LISR_TEIF1 /*!<  Stream 1 transfer error interrupt flag */
    /* 0x00000400 */ uint32_t Stream1_HalfTransferComplite: 1; //DMA_LISR_HTIF1 /*!<  Stream 1 half transfer interrupt flag */
    /* 0x00000800 */ uint32_t Stream1_TransferComplite: 1; //DMA_LISR_TCIF1 *!<  Stream 1 transfer complete interrupt flag */

    /* 0x00001000 */ uint32_t Free12_15: 4; //Free12_15

    /* 0x00010000 */ uint32_t Stream2_FIFO_Error: 2; //DMA_LISR_FEIF2 /*!<  Stream 2 FIFO error interrupt flag */
    /* 0x00040000 */ uint32_t Stream2_DirectModeError: 1; //DMA_LISR_DMEIF2 /*!<  Stream 2 direct mode error interrupt flag */
    /* 0x00080000 */ uint32_t Stream2_TransferError: 1; //DMA_LISR_TEIF2 /*!<  Stream 2 transfer error interrupt flag */
    /* 0x00100000 */ uint32_t Stream2_HalfTransferComplite: 1; //DMA_LISR_HTIF2 /*!<  Stream 2 half transfer interrupt flag */
    /* 0x00200000 */ uint32_t Stream2_TransferComplite: 1; //DMA_LISR_TCIF2 *!<  Stream 2 transfer complete interrupt flag */

    /* 0x00400000 */ uint32_t Stream3_FIFO_Error: 2; //DMA_LISR_FEIF3 /*!<  Stream 3 FIFO error interrupt flag */
    /* 0x01000000 */ uint32_t Stream3_DirectModeError: 1; //DMA_LISR_DMEIF3 /*!<  Stream 3 direct mode error interrupt flag */
    /* 0x02000000 */ uint32_t Stream3_TransferError: 1; //DMA_LISR_TEIF3 /*!<  Stream 3 transfer error interrupt flag */
    /* 0x04000000 */ uint32_t Stream3_HalfTransferComplite: 1; //DMA_LISR_HTIF3 /*!<  Stream 3 half transfer interrupt flag */
    /* 0x08000000 */ uint32_t Stream3_TransferComplite: 1; //DMA_LISR_TCIF3 *!<  Stream 3 transfer complete interrupt flag */

  };
  uint32_t Value;

} REG_DMA_LIFCR_T;
//==============================================================================
typedef union
{
  struct
  {
    /* 0x00000001 */ uint32_t Stream4_FIFO_Error: 2; //DMA_LISR_FEIF4 /*!<  Stream 4 FIFO error interrupt flag */
    /* 0x00000004 */ uint32_t Stream4_DirectModeError: 1; //DMA_LISR_DMEIF4 /*!<  Stream 4 direct mode error interrupt flag */
    /* 0x00000008 */ uint32_t Stream4_TransferError: 1; //DMA_LISR_TEIF4 /*!<  Stream 4 transfer error interrupt flag */
    /* 0x00000010 */ uint32_t Stream4_HalfTransferComplite: 1; //DMA_LISR_HTIF4 /*!<  Stream 4 half transfer interrupt flag */
    /* 0x00000020 */ uint32_t Stream4_TransferComplite: 1; //DMA_LISR_TCIF4 *!<  Stream 4 transfer complete interrupt flag */

    /* 0x00000040 */ uint32_t Stream5_FIFO_Error: 2; //DMA_LISR_FEIF5 /*!<  Stream 5 FIFO error interrupt flag */
    /* 0x00000100 */ uint32_t Stream5_DirectModeError: 1; //DMA_LISR_DMEIF5 /*!<  Stream 5 direct mode error interrupt flag */
    /* 0x00000200 */ uint32_t Stream5_TransferError: 1; //DMA_LISR_TEIF5 /*!<  Stream 5 transfer error interrupt flag */
    /* 0x00000400 */ uint32_t Stream5_HalfTransferComplite: 1; //DMA_LISR_HTIF5 /*!<  Stream 5 half transfer interrupt flag */
    /* 0x00000800 */ uint32_t Stream5_TransferComplite: 1; //DMA_LISR_TCIF5 *!<  Stream 5 transfer complete interrupt flag */

    /* 0x00001000 */ uint32_t Free12_15: 4; //Free12_15

    /* 0x00010000 */ uint32_t Stream6_FIFO_Error: 2; //DMA_LISR_FEIF6 /*!<  Stream 6 FIFO error interrupt flag */
    /* 0x00040000 */ uint32_t Stream6_DirectModeError: 1; //DMA_LISR_DMEIF6 /*!<  Stream 6 direct mode error interrupt flag */
    /* 0x00080000 */ uint32_t Stream6_TransferError: 1; //DMA_LISR_TEIF6 /*!<  Stream 6 transfer error interrupt flag */
    /* 0x00100000 */ uint32_t Stream6_HalfTransferComplite: 1; //DMA_LISR_HTIF6 /*!<  Stream 6 half transfer interrupt flag */
    /* 0x00200000 */ uint32_t Stream6_TransferComplite: 1; //DMA_LISR_TCIF6 *!<  Stream 6 transfer complete interrupt flag */

    /* 0x00400000 */ uint32_t Stream7_FIFO_Error: 2; //DMA_LISR_FEIF7 /*!<  Stream 7 FIFO error interrupt flag */
    /* 0x01000000 */ uint32_t Stream7_DirectModeError: 1; //DMA_LISR_DMEIF7 /*!<  Stream 7 direct mode error interrupt flag */
    /* 0x02000000 */ uint32_t Stream7_TransferError: 1; //DMA_LISR_TEIF7 /*!<  Stream 7 transfer error interrupt flag */
    /* 0x04000000 */ uint32_t Stream7_HalfTransferComplite: 1; //DMA_LISR_HTIF7 /*!<  Stream 7 half transfer interrupt flag */
    /* 0x08000000 */ uint32_t Stream7_TransferComplite: 1; //DMA_LISR_TCIF7 *!<  Stream 7 transfer complete interrupt flag */

  };
  uint32_t Value;

} REG_DMA_HIFCR_T;
//==============================================================================
//typedef struct
//{
//  __IO uint32_t ISR;   /*!< DMA interrupt status register */
//  __IO uint32_t Reserved0;
//  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
//} DMA_Base_Registers;
//==============================================================================
typedef union
{
  struct
  {
    volatile REG_DMA_LISR_T LISR; /*!< DMA low interrupt status register,      Address offset: 0x00 */
    volatile REG_DMA_HISR_T HISR; /*!< DMA high interrupt status register,     Address offset: 0x04 */
    volatile REG_DMA_LIFCR_T LIFCR; /*!< DMA low interrupt flag clear register,  Address offset: 0x08 */
    volatile REG_DMA_HIFCR_T HIFCR; /*!< DMA high interrupt flag clear register, Address offset: 0x0C */

  };
  uint32_t Value;

} REG_DMA_T; //DMA_TypeDef
//==============================================================================
typedef struct
{
    volatile REG_DMA_SxCR_T CR;
    volatile uint32_t NDTR;
    volatile uint32_t PAR;
    volatile uint32_t M0AR;

    volatile uint32_t M1AR;
    volatile uint32_t FCR;

} REG_DMA_StreamT;
//==============================================================================
#endif //REGISTERS_STM32H7XX_DMA_H_
#endif //REGISTERS_STM32H7XX_ENABLE
