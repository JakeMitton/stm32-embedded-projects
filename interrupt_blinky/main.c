#include <stdint.h>
#include "globals.h"

#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
#define GPIOA_BSRR (*(volatile uint32_t *) 0x40020018)
#define STK_CTRL  (*(volatile uint32_t *)0xE000E010)
#define STK_LOAD  (*(volatile uint32_t *)0xE000E014)
#define STK_VAL   (*(volatile uint32_t *)0xE000E018)
#define toggle_interval 1000

static inline int toggle_now(uint32_t tics_at_last_toggle, uint32_t toggle_interval_in_ms) {
	if (global_counter - tics_at_last_toggle >= toggle_interval_in_ms) {
		return 1;
	} else {
		return 0;
	}
}

int main(void) {
	//Enable the clock to GPIOA
	RCC_AHB1ENR |= (0b1 << 0);

	//Set pin PA5 to output mode
	GPIOA_MODER &= ~(0b11 << 10); //clear mask
	GPIOA_MODER |= (0b01 << 10);  //set mask

	//Initialise SYSTICK to run a delay of 1 milisecond
	STK_LOAD = (0b11111001111111);
	STK_VAL = 0;  
	STK_CTRL |= (0b111);

	uint32_t tics_at_last_toggle = 0;
	int led_state = 0;

	//Blink the LED on pin PA5
	while (1) {
		if (toggle_now(tics_at_last_toggle, toggle_interval) == 1) {
			if (led_state == 0) {
				//LED is off, toggle it on
				GPIOA_BSRR = (0b1 << 5);
				led_state = 1;
			} else {
				//LED is on, toggle it off
				GPIOA_BSRR = (0b1 << (5 + 16));
				led_state = 0;
			}
			tics_at_last_toggle += toggle_interval;
		}
	}
}