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
Osc32kctrl *pOsc32 = OSC32KCTRL;
Oscctrl *pOsc = OSCCTRL;
Sercom *pSercom0 = SERCOM0;
Dmac *pDmac = DMAC;





//DPLL0 alias

OscctrlDpll *dpll0 = &OSCCTRL->Dpll[0];


uint8_t init()
{

/*
 * GCLK_MAIN set to 48Mhz
 * Generic-Gen-0 uses DFLL48M to generate GCLK_MAIN
 */


	//output to pin enable
	pGclk->GENCTRL[0].bit.OE |= 1;

	pPort->Group[0].DIRSET.reg |= (1 << 23);
	pPort->Group[0].PINCFG[14].bit.PMUXEN |= 1;
	pPort->Group[0].PMUX[7].bit.PMUXE = 0xC;









//wait on MCLK.CKRDY
	while(!pMclk->INTFLAG.bit.CKRDY);

	osc32_init();

//	gen2_init();

	osc_init();

//	pGclk->GENCTRL[0].bit.SRC = 7;
//	while(pGclk->SYNCBUSY.bit.GENCTRL0);

return 0;

}



uint8_t osc32_init()

{
	pOsc32->XOSC32K.bit.ENABLE |= 1;
	pOsc32->XOSC32K.bit.XTALEN |= 1;

	pOsc32->XOSC32K.bit.EN32K |= 1;
	pOsc32->XOSC32K.bit.ONDEMAND = 0;

	while(!pOsc32->STATUS.bit.XOSC32KRDY);


return 0;
}



uint8_t osc_init()
{

	pOsc->Dpll[0].DPLLCTRLA.bit.ENABLE = 1;
	while(pOsc->Dpll[0].DPLLSYNCBUSY.bit.ENABLE);
	pOsc->Dpll[0].DPLLCTRLA.bit.ONDEMAND = 0;

	pOsc->Dpll[0].DPLLRATIO.bit.LDR = 3499;




	//wait until frequency lock
//	while(!dpll0->DPLLSTATUS.bit.LOCK);







	return 0;
}


uint8_t gen2_init()
{
	//OSC32K SRC
	pGclk->GENCTRL[1].bit.GENEN = 1;
	pGclk->GENCTRL[1].bit.DIV = 0;
	pGclk->GENCTRL[1].bit.SRC |= 5;

//DPLL PERIPH

	pGclk->PCHCTRL[1].bit.GEN = 1;
	pGclk->PCHCTRL[1].bit.CHEN = 1;

	pGclk->PCHCTRL[3].bit.GEN = 1;
	pGclk->PCHCTRL[3].bit.CHEN = 1;



	return 0;
}




//sercom0->USART = serUsart;

//serUsart = &SERCOM0_BASE_ADDRESS;
