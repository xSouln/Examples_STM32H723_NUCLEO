//==============================================================================
#ifndef REGISTERS_STM32H7XX_GPIO_H_
#define REGISTERS_STM32H7XX_GPIO_H_
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
      /* 0x00000001 */ uint32_t Pin0 : 2; //GPIO_MODER_MODE0
      /* 0x00000004 */ uint32_t Pin1 : 2; //GPIO_MODER_MODE1

      /* 0x00000010 */ uint32_t Pin2 : 2; //GPIO_MODER_MODE2
      /* 0x00000040 */ uint32_t Pin3 : 2; //GPIO_MODER_MODE3

      /* 0x00000100 */ uint32_t Pin4 : 2; //GPIO_MODER_MODE4
      /* 0x00000400 */ uint32_t Pin5 : 2; //GPIO_MODER_MODE5

      /* 0x00001000 */ uint32_t Pin6 : 2; //GPIO_MODER_MODE6
      /* 0x00004000 */ uint32_t Pin7 : 2; //GPIO_MODER_MODE7

      /* 0x00010000 */ uint32_t Pin8 : 2; //GPIO_MODER_MODE8
      /* 0x00040000 */ uint32_t Pin9 : 2; //GPIO_MODER_MODE9

      /* 0x00100000 */ uint32_t Pin10 : 2; //GPIO_MODER_MODE10
      /* 0x00400000 */ uint32_t Pin11 : 2; //GPIO_MODER_MODE11

      /* 0x01000000 */ uint32_t Pin12 : 2; //GPIO_MODER_MODE12
      /* 0x04000000 */ uint32_t Pin13 : 2; //GPIO_MODER_MODE13

      /* 0x10000000 */ uint32_t Pin14 : 2; //GPIO_MODER_MODE14
      /* 0x40000000 */ uint32_t Pin15 : 2; //GPIO_MODER_MODE15
   };
   uint32_t Value;

} REG_GPIO_MODER_T; //
//==============================================================================
#define REG_GPIO_MODER_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 2;\
        /* 0x00000004 */ uint32_t pin1 : 2;\
        /* 0x00000010 */ uint32_t pin2 : 2;\
        /* 0x00000040 */ uint32_t pin3 : 2;\
        /* 0x00000100 */ uint32_t pin4 : 2;\
        /* 0x00000400 */ uint32_t pin5 : 2;\
        /* 0x00001000 */ uint32_t pin6 : 2;\
        /* 0x00004000 */ uint32_t pin7 : 2;\
        /* 0x00010000 */ uint32_t pin8 : 2;\
        /* 0x00040000 */ uint32_t pin9 : 2;\
        /* 0x00100000 */ uint32_t pin10 : 2;\
        /* 0x00400000 */ uint32_t pin11 : 2;\
        /* 0x01000000 */ uint32_t pin12 : 2;\
        /* 0x04000000 */ uint32_t pin13 : 2;\
        /* 0x10000000 */ uint32_t pin14 : 2;\
        /* 0x40000000 */ uint32_t pin15 : 2;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_MODER_T
//==============================================================================
typedef union
{
   struct
   {
      /* 0x00000001 */ uint32_t Pin0 : 1; //GPIO_OTYPER_OT0
      /* 0x00000002 */ uint32_t Pin1 : 1; //GPIO_OTYPER_OT1
      /* 0x00000004 */ uint32_t Pin2 : 1; //GPIO_OTYPER_OT2
      /* 0x00000008 */ uint32_t Pin3 : 1; //GPIO_OTYPER_OT3

      /* 0x00000010 */ uint32_t Pin4 : 1; //GPIO_OTYPER_OT4
      /* 0x00000020 */ uint32_t Pin5 : 1; //GPIO_OTYPER_OT5
      /* 0x00000040 */ uint32_t Pin6 : 1; //GPIO_OTYPER_OT6
      /* 0x00000080 */ uint32_t Pin7 : 1; //GPIO_OTYPER_OT7

      /* 0x00000100 */ uint32_t Pin8 : 1; //GPIO_OTYPER_OT8
      /* 0x00000200 */ uint32_t Pin9 : 1; //GPIO_OTYPER_OT9
      /* 0x00000400 */ uint32_t Pin10 : 1; //GPIO_OTYPER_OT10
      /* 0x00000800 */ uint32_t Pin11 : 1; //GPIO_OTYPER_OT11

      /* 0x00001000 */ uint32_t Pin12 : 1; //GPIO_OTYPER_OT12
      /* 0x00002000 */ uint32_t Pin13 : 1; //GPIO_OTYPER_OT13
      /* 0x00004000 */ uint32_t Pin14 : 1; //GPIO_OTYPER_OT14
      /* 0x00008000 */ uint32_t Pin15 : 1; //GPIO_OTYPER_OT15
   };
   uint32_t Value;

} REG_GPIO_OTYPER_T; //
//==============================================================================
#define REG_GPIO_OTYPER_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 1;\
        /* 0x00000002 */ uint32_t pin1 : 1;\
        /* 0x00000004 */ uint32_t pin2 : 1;\
        /* 0x00000008 */ uint32_t pin3 : 1;\
        /* 0x00000010 */ uint32_t pin4 : 1;\
        /* 0x00000020 */ uint32_t pin5 : 1;\
        /* 0x00000040 */ uint32_t pin6 : 1;\
        /* 0x00000080 */ uint32_t pin7 : 1;\
        /* 0x00000100 */ uint32_t pin8 : 1;\
        /* 0x00000200 */ uint32_t pin9 : 1;\
        /* 0x00000400 */ uint32_t pin10 : 1;\
        /* 0x00000800 */ uint32_t pin11 : 1;\
        /* 0x00001000 */ uint32_t pin12 : 1;\
        /* 0x00002000 */ uint32_t pin13 : 1;\
        /* 0x00004000 */ uint32_t pin14 : 1;\
        /* 0x00008000 */ uint32_t pin15 : 1;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_OTYPER_T
//==============================================================================
typedef union
{
   struct
   {
      /* 0x00000001 */ uint32_t Pin0 : 2; //GPIO_OSPEEDR_OSPEED0
      /* 0x00000004 */ uint32_t Pin1 : 2; //GPIO_OSPEEDR_OSPEED1

      /* 0x00000010 */ uint32_t Pin2 : 2; //GPIO_OSPEEDR_OSPEED2
      /* 0x00000040 */ uint32_t Pin3 : 2; //GPIO_OSPEEDR_OSPEED3

      /* 0x00000100 */ uint32_t Pin4 : 2; //GPIO_OSPEEDR_OSPEED4
      /* 0x00000400 */ uint32_t Pin5 : 2; //GPIO_OSPEEDR_OSPEED5

      /* 0x00001000 */ uint32_t Pin6 : 2; //GPIO_OSPEEDR_OSPEED6
      /* 0x00004000 */ uint32_t Pin7 : 2; //GPIO_OSPEEDR_OSPEED7

      /* 0x00010000 */ uint32_t Pin8 : 2; //GPIO_OSPEEDR_OSPEED8
      /* 0x00040000 */ uint32_t Pin9 : 2; //GPIO_OSPEEDR_OSPEED9

      /* 0x00100000 */ uint32_t Pin10 : 2; //GPIO_OSPEEDR_OSPEED10
      /* 0x00400000 */ uint32_t Pin11 : 2; //GPIO_OSPEEDR_OSPEED11

      /* 0x01000000 */ uint32_t Pin12 : 2; //GPIO_OSPEEDR_OSPEED12
      /* 0x04000000 */ uint32_t Pin13 : 2; //GPIO_OSPEEDR_OSPEED13

      /* 0x10000000 */ uint32_t Pin14 : 2; //GPIO_OSPEEDR_OSPEED14
      /* 0x40000000 */ uint32_t Pin15 : 2; //GPIO_OSPEEDR_OSPEED15
   };
   uint32_t Value;

} REG_GPIO_OSPEEDR_T; //
//==============================================================================
#define REG_GPIO_OSPEEDR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 2;\
        /* 0x00000004 */ uint32_t pin1 : 2;\
        /* 0x00000010 */ uint32_t pin2 : 2;\
        /* 0x00000040 */ uint32_t pin3 : 2;\
        /* 0x00000100 */ uint32_t pin4 : 2;\
        /* 0x00000400 */ uint32_t pin5 : 2;\
        /* 0x00001000 */ uint32_t pin6 : 2;\
        /* 0x00004000 */ uint32_t pin7 : 2;\
        /* 0x00010000 */ uint32_t pin8 : 2;\
        /* 0x00040000 */ uint32_t pin9 : 2;\
        /* 0x00100000 */ uint32_t pin10 : 2;\
        /* 0x00400000 */ uint32_t pin11 : 2;\
        /* 0x01000000 */ uint32_t pin12 : 2;\
        /* 0x04000000 */ uint32_t pin13 : 2;\
        /* 0x10000000 */ uint32_t pin14 : 2;\
        /* 0x40000000 */ uint32_t pin15 : 2;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_OSPEEDR_T
//==============================================================================
typedef union
{
   struct
   {
      /* 0x00000001 */ uint32_t Pin0 : 2; //GPIO_PUPDR_PUPD0
      /* 0x00000004 */ uint32_t Pin1 : 2; //GPIO_PUPDR_PUPD1

      /* 0x00000010 */ uint32_t Pin2 : 2; //GPIO_PUPDR_PUPD2
      /* 0x00000040 */ uint32_t Pin3 : 2; //GPIO_PUPDR_PUPD3

      /* 0x00000100 */ uint32_t Pin4 : 2; //GPIO_PUPDR_PUPD4
      /* 0x00000400 */ uint32_t Pin5 : 2; //GPIO_PUPDR_PUPD5

      /* 0x00001000 */ uint32_t Pin6 : 2; //GPIO_PUPDR_PUPD6
      /* 0x00004000 */ uint32_t Pin7 : 2; //GPIO_PUPDR_PUPD7

      /* 0x00010000 */ uint32_t Pin8 : 2; //GPIO_PUPDR_PUPD8
      /* 0x00040000 */ uint32_t Pin9 : 2; //GPIO_PUPDR_PUPD9

      /* 0x00100000 */ uint32_t Pin10 : 2; //GPIO_PUPDR_PUPD10
      /* 0x00400000 */ uint32_t Pin11 : 2; //GPIO_PUPDR_PUPD11

      /* 0x01000000 */ uint32_t Pin12 : 2; //GPIO_PUPDR_PUPD12
      /* 0x04000000 */ uint32_t Pin13 : 2; //GPIO_PUPDR_PUPD13

      /* 0x10000000 */ uint32_t Pin14 : 2; //GPIO_PUPDR_PUPD14
      /* 0x40000000 */ uint32_t Pin15 : 2; //GPIO_PUPDR_PUPD15
   };
   uint32_t Value;

} REG_GPIO_PUPDR_T; //
//==============================================================================
#define REG_GPIO_PUPDR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 2;\
        /* 0x00000004 */ uint32_t pin1 : 2;\
        /* 0x00000010 */ uint32_t pin2 : 2;\
        /* 0x00000040 */ uint32_t pin3 : 2;\
        /* 0x00000100 */ uint32_t pin4 : 2;\
        /* 0x00000400 */ uint32_t pin5 : 2;\
        /* 0x00001000 */ uint32_t pin6 : 2;\
        /* 0x00004000 */ uint32_t pin7 : 2;\
        /* 0x00010000 */ uint32_t pin8 : 2;\
        /* 0x00040000 */ uint32_t pin9 : 2;\
        /* 0x00100000 */ uint32_t pin10 : 2;\
        /* 0x00400000 */ uint32_t pin11 : 2;\
        /* 0x01000000 */ uint32_t pin12 : 2;\
        /* 0x04000000 */ uint32_t pin13 : 2;\
        /* 0x10000000 */ uint32_t pin14 : 2;\
        /* 0x40000000 */ uint32_t pin15 : 2;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_PUPDR_T
//==============================================================================
/* GPIO_IDR_IDX */
#define REG_GPIO_IDR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 1;\
        /* 0x00000002 */ uint32_t pin1 : 1;\
        /* 0x00000004 */ uint32_t pin2 : 1;\
        /* 0x00000008 */ uint32_t pin3 : 1;\
        /* 0x00000010 */ uint32_t pin4 : 1;\
        /* 0x00000020 */ uint32_t pin5 : 1;\
        /* 0x00000040 */ uint32_t pin6 : 1;\
        /* 0x00000080 */ uint32_t pin7 : 1;\
        /* 0x00000100 */ uint32_t pin8 : 1;\
        /* 0x00000200 */ uint32_t pin9 : 1;\
        /* 0x00000400 */ uint32_t pin10 : 1;\
        /* 0x00000800 */ uint32_t pin11 : 1;\
        /* 0x00001000 */ uint32_t pin12 : 1;\
        /* 0x00002000 */ uint32_t pin13 : 1;\
        /* 0x00004000 */ uint32_t pin14 : 1;\
        /* 0x00008000 */ uint32_t pin15 : 1;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_IDR_T
//==============================================================================
/* GPIO_ODR_ODX */
#define REG_GPIO_ODR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 1;\
        /* 0x00000002 */ uint32_t pin1 : 1;\
        /* 0x00000004 */ uint32_t pin2 : 1;\
        /* 0x00000008 */ uint32_t pin3 : 1;\
        /* 0x00000010 */ uint32_t pin4 : 1;\
        /* 0x00000020 */ uint32_t pin5 : 1;\
        /* 0x00000040 */ uint32_t pin6 : 1;\
        /* 0x00000080 */ uint32_t pin7 : 1;\
        /* 0x00000100 */ uint32_t pin8 : 1;\
        /* 0x00000200 */ uint32_t pin9 : 1;\
        /* 0x00000400 */ uint32_t pin10 : 1;\
        /* 0x00000800 */ uint32_t pin11 : 1;\
        /* 0x00001000 */ uint32_t pin12 : 1;\
        /* 0x00002000 */ uint32_t pin13 : 1;\
        /* 0x00004000 */ uint32_t pin14 : 1;\
        /* 0x00008000 */ uint32_t pin15 : 1;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_ODR_T
//==============================================================================
/* GPIO_BSRR_BSX and GPIO_BSRR_BRX */
#define REG_GPIO_BSRR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef struct\
{\
    union\
    {\
        struct\
        {\
        /* 0x00000001 */ uint16_t pin0 : 1;\
        /* 0x00000002 */ uint16_t pin1 : 1;\
        /* 0x00000004 */ uint16_t pin2 : 1;\
        /* 0x00000008 */ uint16_t pin3 : 1;\
        /* 0x00000010 */ uint16_t pin4 : 1;\
        /* 0x00000020 */ uint16_t pin5 : 1;\
        /* 0x00000040 */ uint16_t pin6 : 1;\
        /* 0x00000080 */ uint16_t pin7 : 1;\
        /* 0x00000100 */ uint16_t pin8 : 1;\
        /* 0x00000200 */ uint16_t pin9 : 1;\
        /* 0x00000400 */ uint16_t pin10 : 1;\
        /* 0x00000800 */ uint16_t pin11 : 1;\
        /* 0x00001000 */ uint16_t pin12 : 1;\
        /* 0x00002000 */ uint16_t pin13 : 1;\
        /* 0x00004000 */ uint16_t pin14 : 1;\
        /* 0x00008000 */ uint16_t pin15 : 1;\
        };\
        uint16_t zValue;\
    } BS;\
    union\
    {\
        struct\
        {\
            /* 0x00010000 */ uint16_t pin0 : 1;\
            /* 0x00020000 */ uint16_t pin1 : 1;\
            /* 0x00040000 */ uint16_t pin2 : 1;\
            /* 0x00080000 */ uint16_t pin3 : 1;\
            /* 0x00100000 */ uint16_t pin4 : 1;\
            /* 0x00200000 */ uint16_t pin5 : 1;\
            /* 0x00400000 */ uint16_t pin6 : 1;\
            /* 0x00800000 */ uint16_t pin7 : 1;\
            /* 0x01000000 */ uint16_t pin8 : 1;\
            /* 0x02000000 */ uint16_t pin9 : 1;\
            /* 0x04000000 */ uint16_t pin10 : 1;\
            /* 0x08000000 */ uint16_t pin11 : 1;\
            /* 0x10000000 */ uint16_t pin12 : 1;\
            /* 0x20000000 */ uint16_t pin13 : 1;\
            /* 0x40000000 */ uint16_t pin14 : 1;\
            /* 0x80000000 */ uint16_t pin15 : 1;\
        };\
        uint16_t zValue;\
    } BR;\
} REG_GPIO_##name##_BSRR_T
//==============================================================================
/* GPIO_LCKR_LCKX */
#define REG_GPIO_LCKR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef union\
{\
    struct\
    {\
        /* 0x00000001 */ uint32_t pin0 : 1;\
        /* 0x00000002 */ uint32_t pin1 : 1;\
        /* 0x00000004 */ uint32_t pin2 : 1;\
        /* 0x00000008 */ uint32_t pin3 : 1;\
        /* 0x00000010 */ uint32_t pin4 : 1;\
        /* 0x00000020 */ uint32_t pin5 : 1;\
        /* 0x00000040 */ uint32_t pin6 : 1;\
        /* 0x00000080 */ uint32_t pin7 : 1;\
        /* 0x00000100 */ uint32_t pin8 : 1;\
        /* 0x00000200 */ uint32_t pin9 : 1;\
        /* 0x00000400 */ uint32_t pin10 : 1;\
        /* 0x00000800 */ uint32_t pin11 : 1;\
        /* 0x00001000 */ uint32_t pin12 : 1;\
        /* 0x00002000 */ uint32_t pin13 : 1;\
        /* 0x00004000 */ uint32_t pin14 : 1;\
        /* 0x00008000 */ uint32_t pin15 : 1;\
        /* 0x00010000 */ uint32_t Lock : 1;\
    };\
    uint32_t zValue;\
} REG_GPIO_##name##_LCKR_T
//==============================================================================
/* GPIO_AFRL and GPIO_AFRH*/
#define REG_GPIO_AFR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
typedef struct\
{\
    union\
    {\
        struct\
        {\
            /* 0x0000000F */ uint32_t pin0 : 4;\
            /* 0x000000F0 */ uint32_t pin1 : 4;\
            /* 0x00000F00 */ uint32_t pin2 : 4;\
            /* 0x0000F000 */ uint32_t pin3 : 4;\
            /* 0x000F0000 */ uint32_t pin4 : 4;\
            /* 0x00F00000 */ uint32_t pin5 : 4;\
            /* 0x0F000000 */ uint32_t pin6 : 4;\
            /* 0xF0000000 */ uint32_t pin7 : 4;\
        };\
        uint32_t LowRegister;\
    };\
    union\
    {\
        struct\
        {\
            /* 0x0000000F */ uint32_t pin8 : 4;\
            /* 0x000000F0 */ uint32_t pin9 : 4;\
            /* 0x00000F00 */ uint32_t pin10 : 4;\
            /* 0x0000F000 */ uint32_t pin11 : 4;\
            /* 0x000F0000 */ uint32_t pin12 : 4;\
            /* 0x00F00000 */ uint32_t pin13 : 4;\
            /* 0x0F000000 */ uint32_t pin14 : 4;\
            /* 0xF0000000 */ uint32_t pin15 : 4;\
        };\
        uint32_t HighRegister;\
    };\
} REG_GPIO_##name##_AFR_T
//==============================================================================
#define REG_GPIO_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15)\
REG_GPIO_MODER_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_OTYPER_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_OSPEEDR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_PUPDR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_IDR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_ODR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_BSRR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_LCKR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
REG_GPIO_AFR_TYPEDEF(name, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, pin8, pin9, pin10, pin11, pin12, pin13, pin14, pin15);\
typedef struct\
{\
    volatile REG_GPIO_##name##_MODER_T Mode;\
    volatile REG_GPIO_##name##_OTYPER_T Type;\
    volatile REG_GPIO_##name##_OSPEEDR_T Speed;\
    volatile REG_GPIO_##name##_PUPDR_T Pull;\
    volatile REG_GPIO_##name##_IDR_T Input;\
    volatile REG_GPIO_##name##_ODR_T Output;\
    volatile REG_GPIO_##name##_BSRR_T BSRR;\
    volatile REG_GPIO_##name##_LCKR_T Lock;\
    volatile REG_GPIO_##name##_AFR_T AlternateFunction;\
\
} REG_GPIO_##name##_T

/*
//==============================================================================
REG_GPIO_TYPEDEF(EXAMPLE,
                   zPin0,
                   zPin1,
                   zPin2,
                   zPin3,
                   
                   zPin4,
                   zPin5,
                   zPin6,
                   zPin7,
                   
                   zPin8,
                   zPin9,
                   zPin10,
                   zPin11,
                   
                   zPin12,
                   zPin13,
                   zPin14,
                   zPin15);
*/
//==============================================================================
#endif //REGISTERS_STM32H7XX_GPIO_H_
#endif //REGISTERS_STM32H7XX_ENABLE
