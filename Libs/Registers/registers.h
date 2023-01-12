//==============================================================================
#ifndef _REGISTERS_H_
#define _REGISTERS_H_
//==============================================================================
#include "registers_config.h"
#include <stdint.h>
#include <stdbool.h>
//==============================================================================
#if defined(REGISTERS_STM32F4XX_ENABLE)
#include "registers_stm32f4xx/registers_stm32f4xx.h"
#elif defined(REGISTERS_STM32H7XX_ENABLE)
#include "registers_stm32h7xx/registers_stm32h7xx.h"
#endif
//==============================================================================
#endif
