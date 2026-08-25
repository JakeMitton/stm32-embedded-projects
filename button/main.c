#include <stdint.h>

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