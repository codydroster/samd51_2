/*
 * serial.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */
#include "serial.h"

uint8_t uart_transmit_xbee[12] = {0x42,0x42,0x42,0x32,0x42,0x42,0x42,0x32,0x42,0x42,0x42,0x32};


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
	PORT->Group[0].DIRSET.reg |= (1 << 4);
	PORT->Group[0].DIRCLR.reg |= (1 << 5);
	PORT->Group[0].CTRL.reg |= (1 << 5);


//	pUsart0->INTENSET.bit.RXC = 1;
//	pUsart0->INTENSET.bit.DRE = 1;
//	pUsart0->INTENSET.bit.RXS = 1;

//	NVIC_EnableIRQ(SERCOM0_2_IRQn);

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


//	pUsart0->INTENSET.bit.RXC = 1;
//	pUsart0->INTENSET.bit.DRE = 1;
//	pUsart0->INTENSET.bit.RXS = 1;

//	NVIC_EnableIRQ(SERCOM0_2_IRQn);

	//6MHZ clk configured
	SERCOM1->USART.BAUD.reg = 63019;	//115200 baud

	SERCOM1->USART.CTRLA.bit.ENABLE = 1;	//enable

	while(SERCOM1->USART.SYNCBUSY.bit.ENABLE);


	return 0;
}




