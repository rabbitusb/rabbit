
#include "stdint.h"
#include "MKL26Z4.h"


void hal_watchdog_disable(void)
{
    SIM->COPC = (uint32_t)0x00u;
}
