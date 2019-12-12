#include "init.h"





//DmacDescriptor *pDmadesc0;

uint32_t dma0[4];
uint32_t *pdma;


typedef struct
{
	uint16_t brctrl;
	uint16_t btcnt;
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint32_t descaddr;

} dmacdescriptor;







uint8_t  dmac_desc0_init();


uint8_t dmac_init();


uint8_t data_a[16];

uint8_t data_b[16];
