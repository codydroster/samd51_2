#include "init.h"
#include "serial.h"
#include "dma.h"
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



uint8_t init()
{

/*
 * GCLK_MAIN set to 48Mhz
 * Generic-Gen-0 uses DFLL48M to generate GCLK_MAIN
 */
	//wait on MCLK.CKRDY
	while(pGclk->SYNCBUSY.bit.GENCTRL0);
	while(!pMclk->INTFLAG.bit.CKRDY);

//	osc_init();
//	clk_output();
	gen1_init();



	serial0_init();

	dmac_desc0_init();
	dmac_init();






return 0;

}



uint8_t clk_output()
{

	GCLK->GENCTRL[0].bit.OE |= 1;
	PORT->Group[0].PINCFG[14].bit.PMUXEN |= 1;
	PORT->Group[0].PMUX[7].bit.PMUXE = 0xC;
	PORT->Group[0].PINCFG[14].bit.DRVSTR = 1;

return 0;

}



uint8_t osc_init()
{

	pOsc->Dpll[0].DPLLCTRLA.bit.ENABLE = 1;
	while(pOsc->Dpll[0].DPLLSYNCBUSY.bit.ENABLE);
	pOsc->Dpll[0].DPLLCTRLA.bit.ONDEMAND = 0;

	pOsc->Dpll[0].DPLLRATIO.bit.LDR = 3499;//3499;

	while(pOsc->Dpll[0].DPLLSYNCBUSY.bit.ENABLE);
;



	return 0;
}


uint8_t gen1_init()
{
	//112MHZ clock source

	GCLK->GENCTRL[1].bit.GENEN = 1;
	GCLK->GENCTRL[1].bit.DIV = 1;
	GCLK->GENCTRL[1].bit.IDC = 1;

	GCLK->GENCTRL[1].bit.SRC |= 6;

//DPLL PERIPH

	GCLK->PCHCTRL[1].bit.GEN = 1;
	GCLK->PCHCTRL[1].bit.CHEN = 1;

	GCLK->PCHCTRL[3].bit.GEN = 1;
	GCLK->PCHCTRL[3].bit.CHEN = 1;

	while(GCLK->SYNCBUSY.bit.GENCTRL1);



	return 0;
}


