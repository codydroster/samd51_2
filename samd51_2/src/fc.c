/*
 * fc.c
 *
 *  Created on: Aug 29, 2019
 *      Author: codydroster
 */


#include "fc.h"






void TC0_Handler(void)
{

	TC0->COUNT32.INTFLAG.bit.OVF = 1;
	DMAC->Channel[1].CHINTFLAG.bit.TCMPL = 1;
	//DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;

	DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;
}
