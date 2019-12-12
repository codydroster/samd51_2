
#include <stdint.h>
#include "sam.h"
#include "component/sercom.h"




uint8_t init();

uint8_t osc32_init();

uint8_t gen1_init();

uint8_t osc_init();

uint8_t led_init();

uint8_t clk_output();

uint8_t dmac_init();

uint8_t dmac_desc0_init();


Dmac *pDmac;

Sercom *pSercom0;

Gclk *pGclk;

Mclk *pMclk;

Port *pPort;

Osc32kctrl *pOsc32;

Oscctrl *pOsc;


