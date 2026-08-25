#include <stdint.h>

#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
#define GPIOA_BSRR (*(volatile uint32_t *) 0x40020018)
#define STK_CTRL  (*(volatile uint32_t *)0xE000E010)
#define STK_LOAD  (*(volatile uint32_t *)0xE000E014)
#define STK_VAL   (*(volatile uint32_t *)0xE000E018)

void delay(int k) {
	for (int i = k; i>0; i--) {
		while (!(STK_CTRL & (0b1 << 16))) { }
	}
}

int main(void) {
	//Enable the clock to GPIOA
	RCC_AHB1ENR |= (0b1 << 0);

	//Set pin PA5 to output mode
	GPIOA_MODER &= ~(0b11 << 10); //clear mask
	GPIOA_MODER |= (0b01 << 10);  //set mask

	//Initialise SYSTICK to run a delay of 1 milisecond
	//Used by delay()
	STK_LOAD = (0b11111001111111);
	STK_VAL = 0;  
	STK_CTRL &= ~(0b111);  
	STK_CTRL |= (0b101);

	//Blink the LED on pin PA5
	while (1) {
		GPIOA_BSRR = (0b1 << 5);
		delay(1000);
		GPIOA_BSRR = (0b1 << (5 + 16));
		delay(1000);
	}
}