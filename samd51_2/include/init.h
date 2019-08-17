
#include <stdint.h>
#include "sam.h"
#include "component/sercom.h"




uint8_t init();

uint8_t osc32_init();

uint8_t gen2_init();

uint8_t osc_init();




SercomUsart *serUsart;

Sercom *usart0;

Sercom *sercom0;

Gclk *pGclk;

Mclk *pMclk;

Port *pPort;

Osc32kctrl *pOsc32;

Oscctrl *pOsc;


