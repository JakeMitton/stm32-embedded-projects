#include <stdint.h>

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

    while (1) { }
}

void PendSV_Handler(void);

void SysTick_Handler(void);

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
	Default_Handler, // Reserved - 0x0000 0020
	Default_Handler, // Reserved - 0x0000 0024
	Default_Handler, // Reserved - 0x0000 0028
	Default_Handler, // SVCall - 0x0000 002C
	Default_Handler, // Debug Monitor - 0x0000 0030
	Default_Handler, // Reserved - 0x0000 0034
	PendSV_Handler, // PendSV - 0x0000 0038    
	SysTick_Handler, // Systick - 0x0000 003C
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
