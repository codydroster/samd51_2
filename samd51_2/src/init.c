#include "init.h"

/*
 * init.c
 *
 *  Created on: Jul 18, 2019
 *      Author: cody
 */

Port *pPort = PORT;
Mclk *pMclk = MCLK;
Gclk *pGclk = GCLK;


//Sercom usart0 = SERCOM0_USART;
;
int init()
{

//	pGclk->GENCTRL[0].bit.SRC = 6U;
//	pGclk->GENCTRL[0].bit.GENEN = 1U;

// pMclk->CPUDIV.reg = 1U;

	pPort->Group[0].DIRSET.reg |= (1 << 23);






return 0;

}


//sercom0->USART = serUsart;

//serUsart = &SERCOM0_BASE_ADDRESS;
