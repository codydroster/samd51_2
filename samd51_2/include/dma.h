#include "init.h"






typedef struct
{
	uint16_t brctrl;
	uint16_t btcnt;
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint32_t descaddr;

} dmacdescriptor;




uint8_t dmac_chan1_init();
uint8_t dmac_chan0_init();

uint8_t  dmac_desc0_init();
uint8_t dmac_desc1_init();

uint8_t dmac_init();


