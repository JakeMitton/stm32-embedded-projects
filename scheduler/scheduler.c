#include "globals.h"
#include "registers.h"

void PendSV_Handler(void) {
    GPIOA_BSRR = (1 << PA5);
    return;
}
