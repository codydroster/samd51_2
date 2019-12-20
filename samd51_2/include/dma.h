#include "init.h"






typedef struct
{
	uint16_t brctrl;
	uint16_t btcnt;
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint32_t descaddr;

} dmacdescriptor;


DmacDescriptor dmac_descriptor_init(uint32_t *srcaddr, uint32_t *destaddr, uint16_t bytecnt, _Bool srcinc);

uint8_t dmac_channel_init(uint8_t chan, uint8_t trigger, _Bool tcmpl);


uint8_t dmac_chan0_init();
uint8_t dmac_chan1_init();
uint8_t dmac_chan2_init();
uint8_t dmac_chan3_init();

uint8_t  dmac_desc0_init();
uint8_t dmac_desc1_init();
uint8_t dmac_desc2_init();
uint8_t dmac_desc3_init();


uint8_t dmac_init();


