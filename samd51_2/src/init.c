#include "init.h"
#include "serial.h"
#include "dma.h"
/*
 * init.c
 *
 *  Created on: Jul 18, 2019
 *      Author: cody
 */






//DPLL0 alias



uint8_t init()
{

/*
 * GCLK_MAIN set to 48Mhz
 * Generic-Gen-0 uses DFLL48M to generate GCLK_MAIN
 */
	//wait on MCLK.CKRDY
	while(GCLK->SYNCBUSY.bit.GENCTRL0);
	while(!MCLK->INTFLAG.bit.CKRDY);

//	osc_init();
//	clk_output();
	gen1_init();



	serial0_init();

	dmac_init();
	TC0_init();





return 0;

}

uint8_t TC0_init()
{
	MCLK->APBAMASK.bit.TC0_ = 1;
	MCLK->APBAMASK.bit.TC1_ = 1;

	GCLK->PCHCTRL[9].bit.GEN = 0;	//48Mhz source

	while(TC0->COUNT32.SYNCBUSY.bit.STATUS);
	GCLK->PCHCTRL[9].bit.CHEN = 1;
	TC0->COUNT32.CTRLA.bit.MODE = 2;
	TC1->COUNT32.CTRLA.bit.MODE = 2;
	TC0->COUNT32.CTRLA.bit.CAPTEN0 = 1;
	TC0->COUNT32.CTRLBSET.bit.DIR = 0;
	TC0->COUNT32.WAVE.bit.WAVEGEN = 1;	//TOP = CC0
	TC0->COUNT32.CCBUF[0].reg = (uint32_t) 0x18d800;//0xdfffffff;

	TC0->COUNT32.INTENSET.bit.OVF = 1;

	TC1->COUNT16.CTRLA.bit.ENABLE = 1;
	while(TC1->COUNT16.SYNCBUSY.bit.ENABLE);

	TC0->COUNT32.CTRLA.bit.ENABLE = 1;
	while(TC0->COUNT32.SYNCBUSY.bit.ENABLE);




	NVIC_EnableIRQ(TC0_IRQn);	//overflow interrupt

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

	OSCCTRL->Dpll[0].DPLLCTRLA.bit.ENABLE = 1;
	while(OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.ENABLE);
	OSCCTRL->Dpll[0].DPLLCTRLA.bit.ONDEMAND = 0;

	OSCCTRL->Dpll[0].DPLLRATIO.bit.LDR = 1499;//3499;	//48MhZ

	while(OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.ENABLE);
;



	return 0;
}


uint8_t gen1_init()
{
	//112MHZ clock source

	GCLK->GENCTRL[1].bit.GENEN = 1;
	GCLK->GENCTRL[1].bit.DIV = 6;	//8Mhz
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


