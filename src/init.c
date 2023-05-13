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
	clk_output();
	gen1_init();
	gen2_init();



	serial0_init();
	serial1_init();
	serial2_init();
	serial3_init();
	serial4_init();
	dmac_init();
	TC0_init();
	TC2_init();
	TC4_init();





return 0;

}


//Transmit to FC periodically
uint8_t TC0_init()
{
	MCLK->APBAMASK.bit.TC0_ = 1;
	MCLK->APBAMASK.bit.TC1_ = 1;

	GCLK->PCHCTRL[9].bit.GEN = 0;	//48Mhz source

	while(TC0->COUNT32.SYNCBUSY.bit.STATUS);
	GCLK->PCHCTRL[9].bit.CHEN = 1;
	TC0->COUNT32.CTRLA.bit.MODE = 2;
	TC1->COUNT32.CTRLA.bit.MODE = 2;

	TC0->COUNT32.CTRLA.bit.CAPTEN0 = 0;
	TC0->COUNT32.CTRLBSET.bit.DIR = 0;
	TC0->COUNT32.WAVE.bit.WAVEGEN = 1;	//TOP = CC0
	TC0->COUNT32.CCBUF[0].reg = (uint32_t) 0x90E00;//0x80E80;//0xdfffffff; 0x249F00

	TC0->COUNT32.INTENSET.bit.OVF = 1;


	TC0->COUNT32.CTRLA.bit.ENABLE = 1;
	while(TC0->COUNT32.SYNCBUSY.bit.ENABLE);




	NVIC_EnableIRQ(TC0_IRQn);	//overflow interrupt

return 0;

}

//GPS
uint8_t TC2_init()
{
	MCLK->APBBMASK.bit.TC2_ = 1;
	MCLK->APBBMASK.bit.TC3_ = 1;

	GCLK->PCHCTRL[26].bit.GEN = 0;	//48Mhz source

	while(TC2->COUNT32.SYNCBUSY.bit.STATUS);
	GCLK->PCHCTRL[26].bit.CHEN = 1;
	TC2->COUNT32.CTRLA.bit.MODE = 2;
	TC2->COUNT32.CTRLA.bit.CAPTEN0 = 0;	//???
	TC2->COUNT32.CTRLBSET.bit.DIR = 0;
	TC2->COUNT32.WAVE.bit.WAVEGEN = 1;	//TOP = CC0


	TC2->COUNT32.CCBUF[0].reg = (uint32_t) 0x4000;//0x1D4C00;//0x80E80;//0xdfffffff;

	TC2->COUNT32.INTENSET.bit.OVF = 1;

	TC2->COUNT32.CTRLA.bit.ENABLE = 1;
	while(TC2->COUNT32.SYNCBUSY.bit.ENABLE);

//	NVIC_EnableIRQ(TC2_IRQn);	//overflow interrupt

return 0;

}

uint8_t TC4_init()
{
	MCLK->APBCMASK.bit.TC4_ = 1;
	MCLK->APBCMASK.bit.TC5_ = 1;

	GCLK->PCHCTRL[30].bit.GEN = 2;	//100Khz source



	while(GCLK->SYNCBUSY.bit.GENCTRL2);
	GCLK->PCHCTRL[30].bit.CHEN = 1;

	TC4->COUNT32.CTRLA.bit.MODE = 2;
	TC5->COUNT32.CTRLA.bit.MODE = 2;

	TC4->COUNT32.CTRLA.bit.CAPTEN0 = 0;
	TC4->COUNT32.CTRLBSET.bit.DIR = 0;
	TC4->COUNT32.WAVE.bit.WAVEGEN = 1;	//TOP = CC0
	while(TC4->COUNT32.SYNCBUSY.bit.CC0);
	TC4->COUNT32.CCBUF[0].reg = (uint32_t) 0x30D40;	//1 seconds


	TC4->COUNT32.INTENSET.bit.MC0 = 1;

	TC4->COUNT32.CTRLA.bit.ENABLE = 1;
	while(TC4->COUNT32.SYNCBUSY.bit.ENABLE);







	NVIC_EnableIRQ(TC4_IRQn);	//overflow interrupt

return 0;

}




uint8_t clk_output()
{
	//gclk[2]
	GCLK->GENCTRL[2].bit.OE |= 1;
	PORT->Group[1].PINCFG[16].bit.PMUXEN |= 1;
	PORT->Group[1].PMUX[8].bit.PMUXE = 0xC;
	PORT->Group[1].PINCFG[16].bit.DRVSTR = 1;

	//gclk[0]
//	GCLK->GENCTRL[0].bit.OE |= 1;
//	PORT->Group[0].PINCFG[14].bit.PMUXEN |= 1;
//	PORT->Group[0].PMUX[7].bit.PMUXE = 0xC;
//	PORT->Group[0].PINCFG[14].bit.DRVSTR = 1;

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
	//48MHZ clock source

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

uint8_t gen2_init()
{
	GCLK->GENCTRL[2].bit.GENEN = 1;
	GCLK->GENCTRL[2].bit.DIV = 495;	//200Khz
	GCLK->GENCTRL[2].bit.IDC = 1;
	GCLK->GENCTRL[2].bit.SRC |= 6;

	while(GCLK->SYNCBUSY.bit.GENCTRL2);

	return 0;
}


uint8_t serial0_init()
{


	//clk setup
	MCLK->APBAMASK.bit.SERCOM0_ = 1;

	GCLK->PCHCTRL[7].bit.GEN = 0;	//SERCOM0 clk gen 0
	GCLK->PCHCTRL[7].bit.CHEN = 1;	//enable SERCOM0



	//SERCOM init xbee


	SERCOM0->USART.CTRLA.bit.DORD = 1;	//LSB first
	SERCOM0->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM0->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM0->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - A05 (A1 - board)
	SERCOM0->USART.CTRLA.bit.TXPO = 0;	//PAD[0] - A04 (A4 - board)
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

	PORT->Group[0].PINCFG[4].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[5].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 1;
	PORT->Group[0].DIRCLR.bit.DIRCLR = 1;
	PORT->Group[0].CTRL.reg |= (1 << 5);

	//interupt enable
	SERCOM0->USART.INTENSET.bit.RXC = 1;
	NVIC_EnableIRQ(SERCOM0_2_IRQn);

	//6MHZ clk configured
	SERCOM0->USART.BAUD.reg = 60503;	//230400 baud (63019:115200)

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



	//SERCOM init FC TX (M4 to FC)


	SERCOM1->USART.CTRLA.bit.DORD = 1;	//LSB first ?????
	SERCOM1->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM1->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM1->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - A17 (SCK - board) NC
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

	PORT->Group[0].PINCFG[16].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[17].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 16;
	PORT->Group[0].DIRCLR.bit.DIRCLR = 17;
	PORT->Group[0].CTRL.bit.SAMPLING = 17;
//	PORT->Group[0].DIRSET.reg |= (1 << 16);
//	PORT->Group[0].DIRCLR.reg |= (1 << 17);
//	PORT->Group[0].CTRL.reg |= (1 << 5);



	//6MHZ clk configured
	SERCOM1->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM1->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM1->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}

uint8_t serial2_init()
{


	//clk setup
	MCLK->APBBMASK.bit.SERCOM2_ = 1;

	GCLK->PCHCTRL[23].bit.GEN = 0;	//SERCOM2 clk gen 0
	GCLK->PCHCTRL[23].bit.CHEN = 1;	//enable SERCOM2



	//GPS


	SERCOM2->USART.CTRLA.bit.DORD = 1;	//LSB first
	SERCOM2->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM2->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM2->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - PA13 (SCL - board)
	SERCOM2->USART.CTRLA.bit.TXPO = 0;	//PAD[0] - PA12 (SDA - board)
	SERCOM2->USART.CTRLA.bit.SAMPR = 0;	//16x oversampling
	SERCOM2->USART.CTRLA.bit.MODE = 1;	//internal clk

//	SERCOM2->USART.INTENSET.bit.RXS = 1;
	SERCOM2->USART.INTENSET.bit.RXC = 1;

	SERCOM2->USART.CTRLB.bit.RXEN = 1;	//RX enable
	SERCOM2->USART.CTRLB.bit.TXEN = 1;	//TX enable
	SERCOM2->USART.CTRLB.bit.SBMODE = 0;	//1 stop bit
	SERCOM2->USART.CTRLB.bit.CHSIZE = 0;	//8-bits
	SERCOM2->USART.CTRLB.bit.SFDE = 1;	//start frame detection
	while(SERCOM2->USART.SYNCBUSY.bit.CTRLB);

	//pin setup
	PORT->Group[0].PMUX[6].bit.PMUXE = 2;
	PORT->Group[0].PMUX[6].bit.PMUXO = 2;
	PORT->Group[0].PINCFG[12].bit.DRVSTR = 1;
	PORT->Group[0].PINCFG[13].bit.INEN = 1;
//	PORT->Group[0].PINCFG[5].bit.PULLEN = 1; //???????
//	PORT->Group[0].CTRL.bit.SAMPLING |= 5;	//rx continuous sample

	PORT->Group[0].PINCFG[12].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[13].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 12;
	PORT->Group[0].DIRCLR.bit.DIRCLR = 13;



	NVIC_EnableIRQ(SERCOM2_2_IRQn);


	//6MHZ clk configured
	SERCOM2->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM2->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM2->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}




uint8_t serial3_init()
{
	//clk setup
	MCLK->APBBMASK.bit.SERCOM3_ = 1;

	GCLK->PCHCTRL[24].bit.GEN = 0;	//SERCOM3 clk gen 0
	GCLK->PCHCTRL[24].bit.CHEN = 1;	//enable SERCOM3

	//SERCOM init FC RX (FC to M4)

	SERCOM3->USART.CTRLA.bit.DORD = 1;	//LSB first
	SERCOM3->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM3->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM3->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - PA23  (D13 - board)
	SERCOM3->USART.CTRLA.bit.TXPO = 0;	//PAD[0] - PA22  (D12 - board) NC
	SERCOM3->USART.CTRLA.bit.SAMPR = 0;	//16x oversampling
	SERCOM3->USART.CTRLA.bit.MODE = 1;	//internal clk


	SERCOM3->USART.CTRLB.bit.RXEN = 1;	//RX enable
	SERCOM3->USART.CTRLB.bit.TXEN = 1;	//TX enable
	SERCOM3->USART.CTRLB.bit.SBMODE = 0;	//1 stop bit
	SERCOM3->USART.CTRLB.bit.CHSIZE = 0;	//8-bits
	while(SERCOM3->USART.SYNCBUSY.bit.CTRLB);

	//pin setup
	PORT->Group[0].PMUX[11].bit.PMUXE = 2;
	PORT->Group[0].PMUX[11].bit.PMUXO = 2;
	PORT->Group[0].PINCFG[23].bit.INEN = 1;
	PORT->Group[0].PINCFG[22].bit.DRVSTR = 1;

//	PORT->Group[0].PINCFG[23].bit.PULLEN = 1; //???????
//	PORT->Group[0].CTRL.bit.SAMPLING |= 5;	//rx continuous sample

	PORT->Group[0].PINCFG[23].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[22].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 22;
	PORT->Group[0].DIRCLR.bit.DIRCLR = 23;
	PORT->Group[0].CTRL.bit.SAMPLING = 23;

	SERCOM3->USART.INTENSET.bit.RXC = 1;
	NVIC_EnableIRQ(SERCOM3_2_IRQn);



	//6MHZ clk configured
	SERCOM3->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM3->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM3->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}

//not in use
uint8_t serial4_init()
{


	//clk setup
	MCLK->APBDMASK.bit.SERCOM4_ = 1;

	GCLK->PCHCTRL[34].bit.GEN = 0;	//SERCOM4 clk gen 0
	GCLK->PCHCTRL[34].bit.CHEN = 1;	//enable SERCOM0



	//GPS


	SERCOM4->USART.CTRLA.bit.DORD = 1;	//LSB first
	SERCOM4->USART.CTRLA.bit.CMODE = 0;	//Async
	SERCOM4->USART.CTRLA.bit.FORM = 0;	//USART frame
	SERCOM4->USART.CTRLA.bit.RXPO = 1;	//PAD[1] - PB9 (A3 - board)
	SERCOM4->USART.CTRLA.bit.TXPO = 0;	//PAD[0] - PB8 (A2 - board)
	SERCOM4->USART.CTRLA.bit.SAMPR = 0;	//16x oversampling
	SERCOM4->USART.CTRLA.bit.MODE = 1;	//internal clk


	SERCOM4->USART.CTRLB.bit.RXEN = 1;	//RX enable
	SERCOM4->USART.CTRLB.bit.TXEN = 1;	//TX enable
	SERCOM4->USART.CTRLB.bit.SBMODE = 0;	//1 stop bit
	SERCOM4->USART.CTRLB.bit.CHSIZE = 0;	//8-bits
	while(SERCOM4->USART.SYNCBUSY.bit.CTRLB);

	//pin setup
	PORT->Group[1].PMUX[4].bit.PMUXE = 3;
	PORT->Group[1].PMUX[4].bit.PMUXO = 3;
	PORT->Group[1].PINCFG[8].bit.DRVSTR = 1;
	PORT->Group[1].PINCFG[9].bit.INEN = 1;
//	PORT->Group[1].PINCFG[5].bit.PULLEN = 1; //???????
//	PORT->Group[1].CTRL.bit.SAMPLING |= 5;	//rx continuous sample

	PORT->Group[1].PINCFG[8].bit.PMUXEN = 1;
	PORT->Group[1].PINCFG[9].bit.PMUXEN = 1;
	PORT->Group[1].DIRSET.bit.DIRSET = 8;
	PORT->Group[1].DIRCLR.bit.DIRCLR = 9;



//	NVIC_EnableIRQ(SERCOM0_4_IRQn);


	//6MHZ clk configured
	SERCOM4->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM4->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM4->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}

