#include "init.h"

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
	serial1_init();
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
	TC0->COUNT32.CCBUF[0].reg = (uint32_t) 0xffff;//0xdfffffff;

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


uint8_t serial0_init()
{


	//clk setup
	MCLK->APBAMASK.bit.SERCOM0_ = 1;

	GCLK->PCHCTRL[7].bit.GEN = 0;	//SERCOM0 clk gen 0
	GCLK->PCHCTRL[7].bit.CHEN = 1;	//enable SERCOM0



	//SERCOM init


	SERCOM0->USART.CTRLA.bit.DORD = 1;	//LSB first
	SERCOM0->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM0->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM0->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - A5 (A1 - board)
	SERCOM0->USART.CTRLA.bit.TXPO = 0;	//PAD[0] - A4 (A4 - board)
	SERCOM0->USART.CTRLA.bit.SAMPR = 0;	//16x oversampling
	SERCOM0->USART.CTRLA.bit.MODE = 1;	//internal clk


	SERCOM0->USART.CTRLB.bit.RXEN = 1;	//RX enable
	SERCOM0->USART.CTRLB.bit.TXEN = 1;	//TX enable
	SERCOM0->USART.CTRLB.bit.SBMODE = 0;	//1 stop bit
	SERCOM0->USART.CTRLB.bit.CHSIZE = 0;	//8-bits
	while(SERCOM0->USART.SYNCBUSY.bit.CTRLB);

	//pin setup
	PORT->Group[0].PMUX[2].bit.PMUXE = 3;
	PORT->Group[0].PMUX[2].bit.PMUXO = 3;
	PORT->Group[0].PINCFG[4].bit.DRVSTR = 1;
	PORT->Group[0].PINCFG[5].bit.INEN = 1;
//	PORT->Group[0].PINCFG[5].bit.PULLEN = 1; //???????
//	PORT->Group[0].CTRL.bit.SAMPLING |= 5;	//rx continuous sample

	PORT->Group[0].PINCFG[4].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[5].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 1;
	PORT->Group[0].DIRCLR.bit.DIRCLR = 1;
	PORT->Group[0].CTRL.reg |= (1 << 5);


	//6MHZ clk configured
	SERCOM0->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM0->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM0->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}



uint8_t serial1_init()
{


	//clk setup
	MCLK->APBAMASK.bit.SERCOM1_ = 1;

	GCLK->PCHCTRL[8].bit.GEN = 0;	//SERCOM1 clk gen 0
	GCLK->PCHCTRL[8].bit.CHEN = 1;	//enable SERCOM1



	//SERCOM init


	SERCOM1->USART.CTRLA.bit.DORD = 1;	//LSB first
	SERCOM1->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM1->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM1->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - A17 (SCK - board)
	SERCOM1->USART.CTRLA.bit.TXPO = 0;	//PAD[0] - A16 (D5 - board)
	SERCOM1->USART.CTRLA.bit.SAMPR = 0;	//16x oversampling
	SERCOM1->USART.CTRLA.bit.MODE = 1;	//internal clk


	SERCOM1->USART.CTRLB.bit.RXEN = 1;	//RX enable
	SERCOM1->USART.CTRLB.bit.TXEN = 1;	//TX enable
	SERCOM1->USART.CTRLB.bit.SBMODE = 0;	//1 stop bit
	SERCOM1->USART.CTRLB.bit.CHSIZE = 0;	//8-bits
	while(SERCOM1->USART.SYNCBUSY.bit.CTRLB);

	//pin setup
	PORT->Group[0].PMUX[8].bit.PMUXE = 2;
	PORT->Group[0].PMUX[8].bit.PMUXO = 2;
	PORT->Group[0].PINCFG[16].bit.DRVSTR = 1;
	PORT->Group[0].PINCFG[17].bit.INEN = 1;
//	PORT->Group[0].PINCFG[5].bit.PULLEN = 1; //???????
//	PORT->Group[0].CTRL.bit.SAMPLING |= 5;	//rx continuous sample

	PORT->Group[0].PINCFG[16].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[17].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 16;
	PORT->Group[0].DIRCLR.bit.DIRCLR = 17;
	PORT->Group[0].CTRL.bit.SAMPLING = 17;
//	PORT->Group[0].DIRSET.reg |= (1 << 4);
//	PORT->Group[0].DIRCLR.reg |= (1 << 5);
//	PORT->Group[0].CTRL.reg |= (1 << 5);



	//6MHZ clk configured
	SERCOM1->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM1->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM1->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}





