#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h> 

/* Defined in RM0390 */
// RCC Registers (page 170)
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)

// RCC Bit Positions
#define GPIOAEN 0

// GPIO Registers (page 191)
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
#define GPIOA_BSRR (*(volatile uint32_t *) 0x40020018)

// GPIO Bit Positions
#define PA5 5
#define BSRR_RESET_SHIFT 16

/* Defined in PM0214 */
// SCB Registers (page 206)
#define SCB_ICSR (*(volatile uint32_t *)0xE000ED04)
#define SCB_SHPR3 (*(volatile uint32_t *)0xE000ED20)

// SCB Bit positions
// note the PendSV and SysTick fields are byte-wide
#define PENDSV_PRI_POS 16
#define SYSTICK_PRI_POS 24
#define PENDSVSET 28

// STK Registers (page 230)
#define STK_CTRL  (*(volatile uint32_t *)0xE000E010)
#define STK_LOAD  (*(volatile uint32_t *)0xE000E014)
#define STK_VAL   (*(volatile uint32_t *)0xE000E018)

#endif
