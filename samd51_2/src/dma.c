/*
 * dma.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */

#include "dma.h"
#include "stdint.h"
#include <string.h>



//uint32_t dma0[4];// __attribute((aligned(16)));
//static uint32_t *pdma0 = dma0;

//static DmacDescriptor *pDmadesc0 __attribute__((aligned(16)));

//DmacDescriptor *pDmadesc0;




volatile dmacdescriptor wrb[12] __attribute ((aligned (16)));
dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));
dmacdescriptor desc0 __attribute__ ((aligned (16)));


uint8_t dmac_init()
{

	pDmac->BASEADDR.reg = (uint32_t) &descriptor_section;//pDmadesc0;//dma0;
	pDmac->WRBADDR.reg = (uint32_t) &wrb;//pDmadesc0;//;

	pDmac->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);	//enable dmac

//	NVIC_SetPriority(DMAC_0_Handler, 0);


	dmac_desc0_init();





	return 0;
}

uint8_t dmac_desc0_init()
{



//	desc0->BTCTRL.bit.DSTINC = 1;
//	desc0->BTCTRL.reg |= (1 << 11);
//	desc0->SRCADDR.reg = (uint32_t) &SERCOM0->USART.DATA.reg;	//src address
//	desc0->DSTADDR.reg = (uint32_t) &data_a;	//destination address
//	desc0->DESCADDR.bit.DESCADDR = (uint32_t) 0;	//next descriptor address. No next descriptor



	desc0.brctrl = (uint16_t) (1 << 11);// | (2 << 3);
	desc0.btcnt = 0x100;
	desc0.srcaddr = (uint32_t) &SERCOM0->USART.DATA.reg;
	desc0.dstaddr = (uint32_t) &data_a;
	desc0.descaddr = 0;



	memcpy(&descriptor_section[0], &desc0, sizeof(dmacdescriptor));


	pDmac->Channel[0].CHINTENSET.bit.SUSP = 1;	//suspend interrupt enable;
	pDmac->Channel[0].CHCTRLA.bit.TRIGSRC = 0x4;	//RX trigger



	pDmac->Channel[0].CHCTRLA.bit.ENABLE = 1;







NVIC_EnableIRQ(DMAC_0_IRQn);




	pDmac->Channel[0].CHCTRLA.bit.ENABLE = 1;

	return 0;

}


void DMAC_0_Handler(void)
{







//	pDmac->INTPEND.bit.PEND = 1;
//	pDmac->Channel[0].CHINTFLAG.bit.SUSP = 1;

	pDmac->Channel[0].CHCTRLB.bit.CMD = 2;


}


