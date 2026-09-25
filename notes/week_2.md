# Embedded Programming: Week 2 - More Bare Metal
###  This week I'm going to write an interrupt handler and switch the blinky_from_scratch and button projects to use interrupt based timing for the delay function. 

## Terminology

- SCB -> System Control Block
    - Handles many of the exceptions in entries 1-15 on the vector table

- NVIC -> Nested Vector Interrupt Controller
    - Handles external exceptions (interrupts), which are exceptions 16+ on the vector table

## Writing an interrupt handler

First, I added a new `globals.c` file with it's corresponding `globals.h` header file to contain the global counter. I decided to use a file for this since I am pretty confident in future expansions of this codebase there will be more global variables needed. 

Next, I added a `systick.c` file to contain the logic for the systick exception handler to keep the code properly contained with only startup functions in `startup.c`. Both `globals.c` and `systick.c` were added to our makefile recipe.

Following the creation of the new files I added a forward declaration for the `Systick_Handler` function to `startup.c` to be able to connect the function definition to the call from the vector table. 

The next change was the largest chunk of work in this update where I reworked main() to switch from busy waiting to non-blocking timing for the LED toggle. I updated the value set to the `TICKINT` bit of the `SYST_CSR` register to enable the exception instead of flipping the `COUNTFLAG` bit. In updating the logic in the main loop to use a helper function I introduced two new variables, `tics_at_last_toggle` and `led_state` both of which are declared locally as they are not needed outside the main functions core logic loop and are passed to the helper function, `toggle_now()`, as parameters. In `toggle_now()` a subtraction based comparison is used for wraparound safety. Additionally, in the core logic loop we chose to realign the timing interval each time the LED is toggled (line 48 in `main.c`) to ensure any timing drift does not compound and result in a majorly misaligned LED blink cadence. 

I have a logic analyzer en route which I will use to verify the timing of the LED is programmed correctly when it arrives. 

### Background

What is actually happening when I enable tickint in bit 1 of the `SYST_CSR` register for SYSTICK? This bit switches SYSTICK from flipping the COUNTFLAG bit (bit 16) to 'set the SYSTICK exception status to pending.' (p.B3-622 in ARMv7-M)

### Excuse me sir, you seem to be off by one! Lessons learned this week.
- When adding the Systick_Handler into the vector table I noticed that I had made a mistake reading the entries in RM0390 and had 2 reserved places instead of 4 between UsageFault and SVCall, fixed by adding missing reserves entries. This is a good reminder to pay careful attention to memory addresses in the documentation. 


