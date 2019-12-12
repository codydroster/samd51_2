/*
 * serial.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */
#include "serial.h"

SercomUsart *pUsart0 = &SERCOM0->USART;

uint8_t serial0_init()
{

//	pUsart0->CTRLA.bit.ENABLE = 1;	//enable

//	while(pUsart0->SYNCBUSY.bit.ENABLE);

	//clk setup
	pMclk->APBAMASK.bit.SERCOM0_ = 1;

	pGclk->PCHCTRL[7].bit.GEN = 0;	//SERCOM0 clk gen 0
	pGclk->PCHCTRL[7].bit.CHEN = 1;	//enable SERCOM0



	//SERCOM init
//	pSercom0->USART.CTRLA.bit.ENABLE = 1;

	pUsart0->CTRLA.bit.DORD = 0;	//MSB first
	pUsart0->CTRLA.bit.CMODE = 0;	//Async
	pUsart0->CTRLA.bit.FORM = 0;	//USART frame
	pUsart0->CTRLA.bit.RXPO = 1;	//PAD[1] - A5 (A1 - board)
	pUsart0->CTRLA.bit.TXPO = 0;	//PAD[0] - A4 (A4 - board)
	pUsart0->CTRLA.bit.SAMPR = 0;	//16x oversampling
	pUsart0->CTRLA.bit.MODE = 1;	//internal clk


	pUsart0->CTRLB.bit.RXEN = 1;	//RX enable
	pUsart0->CTRLB.bit.TXEN = 1;	//TX enable
	pUsart0->CTRLB.bit.SBMODE = 0;	//1 stop bit
	pUsart0->CTRLB.bit.CHSIZE = 0;	//8-bits


	//pin setup
	PORT->Group[0].PMUX[2].bit.PMUXE = 3;
	PORT->Group[0].PMUX[2].bit.PMUXO = 3;
	PORT->Group[0].PINCFG[4].bit.DRVSTR = 1;
	PORT->Group[0].PINCFG[5].bit.DRVSTR = 1;

	PORT->Group[0].PINCFG[4].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[5].bit.PMUXEN = 1;
	PORT->Group[0].DIRSET.bit.DIRSET = 4;
	PORT->Group[0].DIRSET.bit.DIRSET = 5;



	pUsart0->INTENSET.bit.DRE = 1;
	pUsart0->INTENSET.bit.TXC = 1;




	//6MHZ clk configured
	pUsart0->BAUD.reg = 63019;	//115200 baud

	pUsart0->CTRLA.bit.ENABLE = 1;	//enable

	while(pUsart0->SYNCBUSY.bit.ENABLE);


	return 0;
}
