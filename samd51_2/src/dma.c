/*
 * dma.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */

#include "dma.h"

uint8_t dmac_init()
{

	pDmac->BASEADDR.reg = (uint32_t) &pDmadesc;
	pDmac->WRBADDR.reg = (uint32_t) &pDmadesc;





	return 0;
}

uint8_t dmac_desc0_init()
{


	pDmadesc->SRCADDR.bit.SRCADDR = (uint32_t) &SERCOM0->USART.DATA.reg;	//src address
	pDmadesc->DSTADDR.bit.DSTADDR = 0;	//destination address
	pDmadesc->DESCADDR.bit.DESCADDR = 0;	//next descriptor address


	return 0;
}
