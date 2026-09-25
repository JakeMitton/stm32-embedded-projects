#include <stdint.h>
#include "globals.h"
#include "registers.h"

#define CLOCK_SPEED 16000000
#define SYSTICK_HZ 1000
#define TOGGLE_INTERVAL 1000

static inline int toggle_now(uint32_t tics_at_last_toggle, uint32_t toggle_interval_in_ms) {
	if (global_counter - tics_at_last_toggle >= toggle_interval_in_ms) {
		return 1;
	} else {
		return 0;
	}
}

int main(void) {
	//Enable the clock to GPIOA
	RCC_AHB1ENR |= (0b1 << GPIOAEN);

    //Set PendSV to the lowest priority
    SCB_SHPR3 &= ~(255 << PENDSV_PRI_POS); 
    SCB_SHPR3 |= (255 << PENDSV_PRI_POS);

	//Set pin PA5 to output mode
	GPIOA_MODER &= ~(0b11 << (PA5 *2)); //clear mask
	GPIOA_MODER |= (0b01 << (PA5 *2));  //set mask

	//Initialise SYSTICK to run a delay of 1 milisecond
	STK_LOAD = ((CLOCK_SPEED / SYSTICK_HZ)-1);
	STK_VAL = 0;  
	STK_CTRL |= (0b111);

    
    SCB_ICSR = (0b1 << PENDSVSET);
    /*
	uint32_t tics_at_last_toggle = 0;
	int led_state = 0;
    */

	//Blink the LED on pin PA5
	while (1) {
        /*
		if (toggle_now(tics_at_last_toggle, TOGGLE_INTERVAL) == 1) {
			if (led_state == 0) {
				//LED is off, toggle it on
				GPIOA_BSRR = (0b1 << PA5);
				led_state = 1;
			} else {
				//LED is on, toggle it off
				GPIOA_BSRR = (0b1 << (PA5 + BSRR_RESET_SHIFT));
				led_state = 0;
			}
			tics_at_last_toggle += TOGGLE_INTERVAL;
		}
        */
	}
}
