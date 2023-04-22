/*
 * dma.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */

#include "dma.h"
#include <stdint.h>
#include "stdbool.h"
#include <string.h>

#include "fc.h"





static DmacDescriptor wrb[4] __attribute ((aligned (128)));
static DmacDescriptor descriptor_section[3] __attribute__ ((aligned (128)));



uint8_t dmac_init()
{

	MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
	DMAC->CTRL.reg = DMAC_CTRL_SWRST;


	descriptor_section[0] = dmac_descriptor_init(&(SERCOM0->USART.DATA.reg), xbee_raw_receive, 13, 0);

	descriptor_section[1] = dmac_descriptor_init(&(SERCOM3->USART.DATA.reg), receive_data_fc, 12, 0);
	descriptor_section[2] = dmac_descriptor_init(transmit_data_fc, &(SERCOM1->USART.DATA.reg) , 18, 1);

//	descriptor_section[1] = dmac_descriptor_init(uart_transmit_xbee, &(SERCOM0->USART.DATA.reg), 10, 1);
//	descriptor_section[4] = dmac_descriptor_init(&(SERCOM2->USART.DATA.reg), receive_data_GPS, 800, 0);


	dmac_channel_init(0, 4, 1);
//	dmac_channel_init(1, 5, 0);
	dmac_channel_init(1, 0xA, 1);
	dmac_channel_init(2, 7, 0);
//	dmac_channel_init(4, 8, 1);


	NVIC_EnableIRQ(DMAC_0_IRQn);
	NVIC_EnableIRQ(DMAC_1_IRQn);




	DMAC->BASEADDR.reg = (uint32_t) descriptor_section;
	DMAC->WRBADDR.reg = (uint32_t) wrb;
	DMAC->DBGCTRL.bit.DBGRUN = 1;
	DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);	//enable dmac


	return 0;
}




uint8_t dmac_channel_init(uint8_t chan, uint8_t trigger, _Bool tcmpl)
{
	DMAC->Channel[chan].CHCTRLA.reg |= (trigger << 8);	//trigger
	DMAC->Channel[chan].CHCTRLA.bit.BURSTLEN = 0x0;	//single beat

	if(tcmpl) {
		DMAC->Channel[chan].CHINTENSET.bit.TCMPL = 1;		//transfer complete interrupt enable;
	}

	DMAC->Channel[chan].CHCTRLA.bit.TRIGACT = 2;		//trigger per block
	DMAC->Channel[chan].CHCTRLA.bit.ENABLE = 1;

		return 0;

}


DmacDescriptor dmac_descriptor_init(uint32_t *srcaddr, uint32_t *destaddr, uint16_t bytecnt, _Bool srcinc)
{
	DmacDescriptor desc __attribute__((aligned (sizeof(DmacDescriptor))));

	desc.BTCNT.reg = bytecnt;
	desc.BTCTRL.bit.BEATSIZE = 0;	//8bit
	desc.BTCTRL.bit.SRCINC = 0;
	desc.BTCTRL.bit.STEPSIZE = 0;
	desc.BTCTRL.bit.DSTINC = 0;
	if(srcinc){
		desc.BTCTRL.bit.SRCINC = 1;
		desc.SRCADDR.reg = (uint32_t) (srcaddr) + bytecnt;	//src address
		desc.DSTADDR.reg = (uint32_t) destaddr;
	} else {
		desc.BTCTRL.bit.DSTINC = 1;
		desc.SRCADDR.reg = (uint32_t) srcaddr;
		desc.DSTADDR.reg = (uint32_t) (destaddr) + bytecnt;	//destination address
	}



	desc.DESCADDR.reg = (uint32_t) 0;		//no additional descriptors in channel
	desc.BTCTRL.bit.VALID = 1;
	desc.BTCTRL.bit.BLOCKACT = 0;



	return desc;

}





