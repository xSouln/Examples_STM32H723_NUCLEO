//==============================================================================
#ifndef REGISTERS_STM32H7XX_DMA2D_H_
#define REGISTERS_STM32H7XX_DMA2D_H_
//==============================================================================
#include "Registers/registers_config.h"
#ifdef REGISTERS_STM32H7XX_ENABLE
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

} REG_DMA2D_CR_T;
//==============================================================================
#endif //REGISTERS_STM32H7XX_DMA_H_
#endif //REGISTERS_STM32H7XX_ENABLE
