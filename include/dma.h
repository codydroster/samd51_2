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

static DmacDescriptor descriptor_section[3] __attribute__ ((aligned (128)));




uint8_t dmac_init();


