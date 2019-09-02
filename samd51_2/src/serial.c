/*
 * serial.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */
#include "serial.h"

uint8_t serial0_init()
{
	//clk setup
	pGclk->PCHCTRL[7].bit.CHEN = 1;	//enable SERCOM0
	pGclk->PCHCTRL[7].bit.GEN = 0;	//SERCOM0 clk gen 0



	//SERCOM init
	pUsart0->CTRLA.bit.DORD = 0;	//MSB first
	pUsart0->CTRLA.bit.CMODE = 0;	//Async
	pUsart0->CTRLA.bit.FORM = 0;	//USART frame
	pUsart0->CTRLA.bit.RXPO = 0;	/****** CHANGE PINOUT ******/
	pUsart0->CTRLA.bit.TXPO = 0;	/****** CHANGE PINOUT ******/
	pUsart0->CTRLA.bit.SAMPR = 0;	//16x oversampling
	pUsart0->CTRLA.bit.MODE = 1;	//internal clk
	pUsart0->CTRLA.bit.ENABLE = 1;	//enable

	pUsart0->CTRLB.bit.RXEN = 1;	//RX enable
	pUsart0->CTRLB.bit.TXEN = 1;	//TX enable
	pUsart0->CTRLB.bit.SBMODE = 0;	//1 stop bit
	pUsart0->CTRLB.bit.CHSIZE = 0;	//8-bits


	//6MHZ clk configured
	pUsart0->BAUD.bit.BAUD = 63858;	//9600 baud
//	pUsart0->BAUD.bit.BAUD = 45403;	//115200 baud




	return 0;
}
