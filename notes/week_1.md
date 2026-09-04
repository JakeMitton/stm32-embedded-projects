# Embedded Programming: Week 1 - The Basics
### This week we install the needed tools and set up our learning environment. 

## *Needed Tools*
- CubeIDE
- F446 Reference manual (RM0390 Reference Manual)
- STM32F446xC/E Data Sheet
- UM1724 User manual

## *Environment Set-up*
- `arm-none-eabi-gcc`
- `openocd`
- `make`
- `arm-none-eabi-gdb`

### How to set up the environment

First we need xcode command line tools since we are using an apple silicon mac, this gives us tools like git and make. To get xcode we would run `xcode-select --install`, however we already have it. Next we install homebrew using `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`, however once again it is already installed. 

Now we need to install our tool chain, this includes gcc, gdb, and open-ocd. We will also install stlink to flash when we don't want to use open-ocd. To install these run `brew install arm-none-eabi-gcc arm-none-eabi-gdb open-ocd stlink`. 

Finally, we want to double check our installation. We will run the following commands:
```bash
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version
openocd --version
make --version
which arm-none-eabi-gcc
```

Note that when we install stlink it creates four binaries called `st-info`, `st-flash`, `st-util`, and `st-trace`. You can check the existence of these files with the following commands.

```bash
st-info --version
st-flash --version
st-util --version
st-trace --version
```
---
### What Even Is The STM32 Nucleo-F446RE?
 It contains an ARM Cortex-M4F core which is a 32 bit core with no memory management unit (MMU) which prevents it from using virtual memory. Out of the box it does not have an operating system, which is how we will use it initially. Later on we will experiment with real time operating systems. It uses the ARMv7E-M architecture, which defines the instruction set as being Thumb-2 as opposed to the full 32 bit ARM instruction set (A32) used on A and R profile ARM cores. In Thumb-2 vector table entries and function pointers' least significant bit (LSB) is a state bit which tells the chip which instruction set to use, in this case that state bit must be set to `1` to indicate we are using the Thumb-2 instruction set. Setting this state bit to 0 on this chip will result in a UsageFault with the INVSTATE flag, however on M series cores the USGFAULTENA bit in the System Handler Control and State Register (SHCSR) is set to not recognize UsageFaults by default, and thus this fault is elevated to a HardFault. 

---

## *Blinky on Bare Metal*

### Locating the addresses

To being we need to find the memory addresses we're going to need. Most of these addresses are found in the RM0390 reference manual. The addresses are as follows.

| Address Name | Address | Offset | Page # | Manual |
| ------------ | ------- | ------ | ------ | ------ |
| GPIO Addresses | `0x40020000 - 0x400203FF` | | p58 | RM0390 |
| GPIOA MODER | | `0x00` | p191 | RM0390 |
| RCC | `0x40023800` | | p58 | RM0390 |
| RCC AHB1 | | `0x30` | p142 | RM0390 |
| GPIOA BSRR | | `0x18` | p192 | RM0390 | 
| STK_CTRL | `0xE000E010 ` | | p230 | PM0214 |
| STK_LOAD |  | `0x04` | p232 | PM0214 |
| STK_VAL |  | `0x08` | p233 | PM0214 |
| STK_CALIB |  | `0x0C` | p234 | PM0214 |

We also need to know the frequency of the HSI oscilator clock, which we find to be 16 MHz on page 118 of RM0390.

### Modifying the binaries

We are going to need to modify the binary in several registers in order to get the light to blink. As we work on this we will use the read-modify-write pattern on macros we define as pointers to registers. 

We can define these macros as follows.
```C
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
#define GPIOA_BSRR (*(volatile uint32_t *) 0x40020018)
#define STK_CTRL  (*(volatile uint32_t *)0xE000E010)
#define STK_LOAD  (*(volatile uint32_t *)0xE000E014)
#define STK_VAL   (*(volatile uint32_t *)0xE000E018)

```
In these macros we define the type to be a volatile unsigned 32 bit integer pointer `(volatile uint32_t *)`, this is then pointed at the address, for GPIOA MODER the address is `0x40020000`, then the pointer is finally dereferenced with the leading `*` which gives us the register contents at our chosen memory address. We now have access to the register to read or modify the contents.

Now that we have pointers to our registers we can modify the binary using masks. 

```C
GPIOA_MODER &= ~(0b11 << 10); //clear mask
GPIOA_MODER |= (0b01 << 10);  //set mask
```

To understand these masks, notice in the clear mask we are clearing 2 bits, and we are inverting the mask so we start with bits `11` which gives us `00000000 00000000 00000000 00000011`, then we bit shift 10 bits left giving `00000000 00000000 00001100 00000000`, then we invert with `~` giving `11111111 11111111 11110011 11111111`. We now use the binary AND operation (`&=`) to compare the mask and the register value, clearing the two positions with zeros in the mask (bits 10 and 11). Next, we use the set mask to put `01` into those now available bit positions using binary OR (`|=`) then write the new binary string back into the address specified by the pointer at GPIO_MODER. 

Next, in order for a peripheral to work on the SMT32 we need to first send the clock signal to the port for that peripheral. To do this we use the RCC or the Reset and Control Clock. RCC contains several AHB busses, including AHB1 (Advanced High-performance Bus 1) which contains the ENR (enable registers) for GPIOA's clock (this bit is called the GPIOAEN) at bit position 0. 

We use a modify-write pattern:
```C
RCC_AHB1ENR |= (0b1 << 0);
```

With the clock signal going to the port we can move onto actually blinking the light. In order to do this we need to vary the output of pin A5 between high and low. We can do this using the BSRR register, which is an atomic write only register where each of the first 16 bits correspond to the the pin matching the bit's index being set to high , and each of the bits in positions 16-31 correspond to setting the same pin to low. 
```C
GPIOA_BSRR = (0b1 << 5);
GPIOA_BSRR = (0b1 << 5 + 16);
```

To make the light actually blink we need a delay function, but since we're running bare metal we don't have a library function for this. To get around this we use the SYSTICK timer built into ARM chips. We will use this timer to measure 1ms intervals then write our delay function based on that set interval. We find the load value for SYSTICK by taking our HSI frequency of 16 MHz and dividing by 1000 then subtracting 1 since SYSTICK counts to 0. This gives us our load value of `15,999`. We set STK_LOAD to the binary equivalent of 15,999, then we set STK_VAL to 0 since any write to this register clears the entire field. Finally we set STK_CTRL's bit 0 (enable countdown) to `1`, bit 1 to `0` (do not enable exception request), and bit 2 to `1` (select processor clock as clock source). 
```C
STK_LOAD = (0b11111001111111);
STK_VAL = 0;  
STK_CTRL &= ~(0b111);  
STK_CTRL |= (0b101);
```
We will run this set-up code once at the top of the file then when we need to use the delay we will call the following delay function we have written.
```C
void delay(int k) {
    for (int i = k; i>0; i--) {
        while (!(STK_CTRL & (0b1 << 16))) { }
    }
}

```

### The Linker Script

```C
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 0x80000
    RAM (wrx)   : ORIGIN = 0x20000000, LENGTH = 0x20000
}

_estack= ORIGIN(RAM)+ LENGTH(RAM);

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text)
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        _sdata = .;
        *(.data)
        _edata = .;
    } > RAM AT> FLASH

    .bss :
    {
        _sbss = .;
        *(.bss)
        _ebss = .;
    } > RAM
}
```

Explaining the code above, the syntax `_sdata = .;` defines a symbol called `_sdata` and assigns it to the current position in memory when it's created (that is what the `= .;` syntax does).

`_sidata` -> where the .data section starts in flash

`_sdata` -> where the .data section should start in SRAM

`_edata` -> the address in SRAM where the .data section ends
so `_sdata` and `_edata` together give you the total length of the .data section. 

`_sbss` where the .bss section starts in SRAM

`_ebss` where the .bss section ends in SRAM

`_estack` where the stack begins. Note that the SMT32 stack grows downwards and by convention we give an address one byte higher then the start of RAM.

### startup.c - including the vector table and reset and default handlers

```C
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

int main(void); 

void Reset_Handler(void) {

    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    uint32_t *bss_strt = &_sbss;

    for (;dst < &_edata; dst++, src++) {
        *dst = *src;
    }

    for (; bss_strt < &_ebss; bss_strt++) {
        *bss_strt = 0;
    }

    main();
}

void Default_Handler(void) {
    while (1) { }
}

void (* const vector_table[])(void) __attribute__((section(".isr_vector"))) = {
    (void (*)(void))&_estack,
    Reset_Handler,
    Default_Handler, // NMI - 0x0000 0008
    Default_Handler, // HardFault - 0x0000 000C
    Default_Handler, // Memory Management - 0x0000 0010
    Default_Handler, // Bus Fault - 0x0000 0014
    Default_Handler, // Usage Fault - 0x0000 0018
    Default_Handler, // Reserved - 0x0000 001C
    Default_Handler, // Reserved - 0x0000 002B
    Default_Handler, // SVCall - 0x0000 002C
    Default_Handler, // Debug Monitor - 0x0000 0030
    Default_Handler, // Reserved - 0x0000 0034
    Default_Handler, // PendSV - 0x0000 0038
    Default_Handler, // Systick - 0x0000 003C
    Default_Handler, // WWDG - 0x0000 0040
    Default_Handler, // PVD - 0x0000 0044
    Default_Handler, // TAMP_STAMP - 0x0000 0048 
    Default_Handler, // RTC_WKUP - 0x0000 004C
    Default_Handler, // FLASH - 0x0000 0050
    Default_Handler, // RCC - 0x0000 0054
    Default_Handler, // EXTI0 - 0x0000 0058
    Default_Handler, // EXTI1 - 0x0000 005C
    Default_Handler, // EXTI2 - 0x0000 0060
    Default_Handler, // EXTI3 - 0x0000 0064
    Default_Handler, // EXTI4 - 0x0000 0068
    Default_Handler, // DMA1_Stream0 - 0x0000 006C
    Default_Handler, // DMA1_Stream1 - 0x0000 0070
    Default_Handler, // DMA1_Stream2 - 0x0000 0074
    Default_Handler, // DMA1_Stream3 - 0x0000 0078
    Default_Handler, // DMA1_Stream4 - 0x0000 007C
    Default_Handler, // DMA1_Stream5 - 0x0000 0080
    Default_Handler, // DMA1_Stream6 - 0x0000 0084
    Default_Handler, // ADC - 0x0000 0088
    Default_Handler, // CAN1_TX - 0x0000 008C
    Default_Handler, // CAN1_RX0 - 0x0000 0090
    Default_Handler, // CAN1_RX1 - 0x0000 0094
    Default_Handler, // CAN1_SCE - 0x0000 0098
    Default_Handler, // EXTI9_5 - 0x0000 009C
    Default_Handler, // TIM1_BRK_TIM9 - 0x0000 00A0
    Default_Handler, // TIM1_UP_TIM10 - 0x0000 00A4
    Default_Handler, // TIM1_TRG_COM_TIM11 - 0x0000 00A8
    Default_Handler, // TIM1_CC - 0x0000 00AC
    Default_Handler, // TIM2 - 0x0000 00B0
    Default_Handler, // TIM3 - 0x0000 00B4
    Default_Handler, // TIM4 - 0x0000 00B8
    Default_Handler, // I2C1_EV - 0x0000 00BC
    Default_Handler, // I2C1_ER - 0x0000 00C0
    Default_Handler, // I2C2_EV - 0x0000 00C4
    Default_Handler, // I2C2_ER - 0x0000 00C8
    Default_Handler, // SPI1 - 0x0000 00CC
    Default_Handler, // SPI2 - 0x0000 00D0
    Default_Handler, // USART1 - 0x0000 00D4
    Default_Handler, // USART2 - 0x0000 00D8
    Default_Handler, // USART3 - 0x0000 00DC
    Default_Handler, // EXTI15_10 - 0x0000 00E0
    Default_Handler, // RTC_Alarm - 0x0000 00E4
    Default_Handler, // OTG_FS_WKUP - 0x0000 00E8
    Default_Handler, // TIM8_BRK_TIM12 - 0x0000 00EC
    Default_Handler, // TIM8_UP_TIM13 - 0x0000 00F0
    Default_Handler, // TIM8_TRG_COM_TIM14 - 0x0000 00F4
    Default_Handler, // TIM8_CC - 0x0000 00F8
    Default_Handler, // DMA1_Stream7 - 0x0000 00FC
    Default_Handler, // FMC - 0x0000 0100
    Default_Handler, // SDIO - 0x0000 0104
    Default_Handler, // TIM5 - 0x0000 0108
    Default_Handler, // SPI3 - 0x0000 010C
    Default_Handler, // UART4 - 0x0000 0110
    Default_Handler, // UART5 - 0x0000 0114
    Default_Handler, // TIM6_DAC - 0x0000 0118
    Default_Handler, // TIM7 - 0x0000 011C
    Default_Handler, // DMA2_Stream0 - 0x0000 0120
    Default_Handler, // DMA2_Stream1 - 0x0000 0124
    Default_Handler, // DMA2_Stream2 - 0x0000 0128
    Default_Handler, // DMA2_Stream3 - 0x0000 012C
    Default_Handler, // DMA2_Stream4 - 0x0000 0130
    Default_Handler, // Reserved - 0x0000 0134
    Default_Handler, // Reserved - 0x0000 0138
    Default_Handler, // CAN2_TX - 0x0000 013C
    Default_Handler, // CAN2_RX0 - 0x0000 0140
    Default_Handler, // CAN2_RX1 - 0x0000 0144
    Default_Handler, // CAN2_SCE - 0x0000 0148
    Default_Handler, // OTG_FS - 0x0000 014C
    Default_Handler, // DMA2_Stream5 - 0x0000 0150
    Default_Handler, // DMA2_Stream6 - 0x0000 0154
    Default_Handler, // DMA2_Stream7 - 0x0000 0158
    Default_Handler, // USART6 - 0x0000 015C
    Default_Handler, // I2C3_EV - 0x0000 0160
    Default_Handler, // I2C3_ER - 0x0000 0164
    Default_Handler, // OTG_HS_EP1_OUT - 0x0000 0168
    Default_Handler, // OTG_HS_EP1_IN - 0x0000 016C
    Default_Handler, // OTG_HS_WKUP - 0x0000 0170
    Default_Handler, // OTG_HS - 0x0000 0174
    Default_Handler, // DCMI - 0x0000 0178
    Default_Handler, // Reserved - 0x0000 017C
    Default_Handler, // Reserved - 0x0000 0180
    Default_Handler, // FPU - 0x0000 0184
    Default_Handler, // Reserved - 0x0000 0188
    Default_Handler, // Reserved - 0x0000 018C
    Default_Handler, // SPI4 - 0x0000 0190
    Default_Handler, // Reserved - 0x0000 0194
    Default_Handler, // Reserved - 0x0000 0198
    Default_Handler, // SAI1 global interrupt - 0x0000 019C
    Default_Handler, // Reserved - 0x0000 01A0
    Default_Handler, // Reserved - 0x0000 01A4
    Default_Handler, // Reserved - 0x0000 01A8
    Default_Handler, // SAI2 - 0x0000 01AC
    Default_Handler, // QuadSPI - 0x0000 01B0
    Default_Handler, // HDMI-CEC - 0x0000 01B4
    Default_Handler, // SPDIF-Rx - 0x0000 01B8
    Default_Handler, // FMPI2C1 - 0x0000 01BC
    Default_Handler, // FMPI2C1 error - 0x0000 01C0
};
```

## Using the B1 Button

To control the light LD2 using the button B1 we need several pieces of information. I have collected the needed information and will list it below.

- B1 is wired to `PC13` (GPIOC pin 13) (memory address `0x40020800`)
- To enable the clock to GPIOC we use bit 2 of RCC_AHB1ENR (memory address `0x40023830`)
- The read register for PC13 is bit 13 of GPIOC_IDR (memory address `0x40020810`)
- PC13 has a built in pull up resistor meaning we don't need to use PUPDR

```C
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
#define GPIOC_IDR (*(volatile uint32_t *)0x40020810)
#define GPIOA_BSRR (*(volatile uint32_t *) 0x40020018)
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)

int main(void) {
    //first we need to activate the clock to GPIOA and GPIOC
    RCC_AHB1ENR |= (0b1 << 0);
    RCC_AHB1ENR |= (0b1 << 2);

    //Set pin PA5 to output mode
    GPIOA_MODER &= ~(0b11 << 10); //clear mask
    GPIOA_MODER |= (0b01 << 10);  //set mask

    while (1) {
        //to find out when the button is pressed we need to read bit 13 of GPIOC_IDR
        if (!(GPIOC_IDR & (0b1 << 13))) {
            //button is pushed: light off
            GPIOA_BSRR = (0b1 << (5 + 16));
        } else {
            //button is not pushed: light on
            GPIOA_BSRR = (0b1 << 5);
        }
    }
}

```

---
### There's a fly in my register! Lessons learned this week.
- I accidentally ordered a micro-usb cable when the nucleo board uses a mini-usb. Lesson is to read the data sheet more carefully before ordering part
- In ARM reserved bits are typically set to their reset value which is denoted in the documentation. In SYSTICK's case they are market "reserved, must be kept cleared" so writing `0` to them is fine. 
- The behaviour of the COUNTFLAG bit in the STM_CTRL was not documented in RM0390, it was only written in the ARMv7-M reference manual. This is a good lesson that for chip level knowledge it's important to check the chip manual, not the vendor manual. 
- I had a toolchain issue where arm-none-eabi-gcc installed through homebrew installs only the compiler and not newlib meaning I don't have access to stdint.h which is where I get uint32_t. To fix this I uninstalled the homebrew version using `brew uninstall arm-none-eabi-gcc` then went to `https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads` to download the macos version of the tooling directly from ARM (the one I needed was `arm-gnu-toolchain-15.3.rel1-darwin-arm64-arm-none-eabi.pkg`).
---


