/*
 * dma.c
 *
 *  Created on: Aug 25, 2019
 *      Author: codydroster
 */

#include "dma.h"
#include "stdint.h"
#include <string.h>
#include "serial.h"
#include "fc.h"



static DmacDescriptor wrb[4] __attribute ((aligned (128)));
static DmacDescriptor descriptor_section[4] __attribute__ ((aligned (128)));
DmacDescriptor desc0 __attribute__((aligned (sizeof(DmacDescriptor))));
DmacDescriptor desc1 __attribute__((aligned (sizeof(DmacDescriptor))));
DmacDescriptor desc2 __attribute__((aligned (sizeof(DmacDescriptor))));
DmacDescriptor desc3 __attribute__((aligned (sizeof(DmacDescriptor))));

uint8_t data_a[12];

uint8_t dmac_init()
{

	MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
	DMAC->CTRL.reg = DMAC_CTRL_SWRST;



	dmac_desc0_init();
	dmac_desc1_init();
	dmac_desc2_init();
	dmac_desc3_init();

	DMAC->BASEADDR.reg = (uint32_t) descriptor_section;
	DMAC->WRBADDR.reg = (uint32_t) wrb;
	DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);	//enable dmac


	dmac_chan0_init();
	dmac_chan1_init();
	dmac_chan2_init();
	dmac_chan3_init();

	NVIC_EnableIRQ(DMAC_0_IRQn);
	NVIC_EnableIRQ(DMAC_2_IRQn);

	return 0;
}



uint8_t dmac_chan0_init()
{

	DMAC->Channel[0].CHCTRLA.reg |= (4 << 8);	//RX trigger SERCOM0
	DMAC->Channel[0].CHCTRLA.bit.BURSTLEN = 0x0;	//single beat
	DMAC->Channel[0].CHINTENSET.bit.TCMPL = 1;		//transfer complete interrupt enable;
	DMAC->Channel[0].CHCTRLA.bit.TRIGACT = 2;		//trigger per block
	DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;

	return 0;


}

uint8_t dmac_chan1_init()
{
		DMAC->Channel[1].CHCTRLA.reg |= (5 << 8);	//TX trigger SERCOM0
		DMAC->Channel[1].CHCTRLA.bit.BURSTLEN = 0x0;	//single beat
//		DMAC->Channel[1].CHINTENSET.bit.TCMPL = 1;	//transfer complete interrupt enable;
//		DMAC->Channel[1].CHINTENSET.bit.TERR = 1;
		DMAC->Channel[1].CHCTRLA.bit.TRIGACT = 2;		//trigger per block
		DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;


		return 0;

}




uint8_t dmac_chan2_init()
{

	DMAC->Channel[2].CHCTRLA.reg |= (6 << 8);	//RX trigger SERCOM0
	DMAC->Channel[2].CHCTRLA.bit.BURSTLEN = 0x0;	//single beat
	DMAC->Channel[2].CHINTENSET.bit.TCMPL = 1;		//transfer complete interrupt enable;
	DMAC->Channel[2].CHCTRLA.bit.TRIGACT = 2;		//trigger per block
	DMAC->Channel[2].CHCTRLA.bit.ENABLE = 1;

	return 0;


}
uint8_t dmac_chan3_init()
{
		DMAC->Channel[3].CHCTRLA.reg |= (7 << 8);	//TX trigger SERCOM1
		DMAC->Channel[3].CHCTRLA.bit.BURSTLEN = 0x0;	//single beat
//		DMAC->Channel[3].CHINTENSET.bit.TCMPL = 1;	//transfer complete interrupt enable;
//		DMAC->Channel[3].CHINTENSET.bit.TERR = 1;
		DMAC->Channel[3].CHCTRLA.bit.TRIGACT = 2;		//trigger per block
		DMAC->Channel[3].CHCTRLA.bit.ENABLE = 1;


		return 0;

}

uint8_t dmac_desc0_init()
{

	desc0.BTCNT.reg = 12;
	desc0.BTCTRL.bit.BEATSIZE = 0;	//8bit
	desc0.BTCTRL.bit.DSTINC = 1;
	desc0.SRCADDR.reg = (uint32_t) &(SERCOM0->USART.DATA.reg);	//src address
	desc0.DSTADDR.reg = (uint32_t) &uart_receive_xbee + sizeof(uart_receive_xbee);	//destination address
	desc0.DESCADDR.reg = (uint32_t) 0;		//no additional descriptors in channel
	desc0.BTCTRL.bit.VALID = 1;
	desc0.BTCTRL.bit.BLOCKACT = 0;
	descriptor_section[0] = desc0;

	return 0;

}


uint8_t dmac_desc1_init()
{

	desc1.BTCNT.reg = 12;
	desc1.BTCTRL.bit.BEATSIZE = 0;	//8bit
	desc1.BTCTRL.bit.SRCINC = 1;
//	desc1.BTCTRL.bit.DSTINC = 0;
	desc1.SRCADDR.reg = (uint32_t) &uart_transmit_xbee + sizeof(uart_transmit_xbee);
	desc1.DSTADDR.reg = (uint32_t) &(SERCOM0->USART.DATA.reg);	//dst address
	desc1.DESCADDR.reg = (uint32_t) 0;		//no additional descriptors in channel
	desc1.BTCTRL.bit.VALID = 1;
	desc1.BTCTRL.bit.BLOCKACT = 0;
	descriptor_section[1] = desc1;


	return 0;

}




uint8_t dmac_desc2_init()
{

	desc2.BTCNT.reg = 12;
	desc2.BTCTRL.bit.BEATSIZE = 0;	//8bit
	desc2.BTCTRL.bit.DSTINC = 1;
	desc2.SRCADDR.reg = (uint32_t) &(SERCOM1->USART.DATA.reg);	//src address
	desc2.DSTADDR.reg = (uint32_t) &receive_data_fc + sizeof(receive_data_fc);	//destination address
	desc2.DESCADDR.reg = (uint32_t) 0;		//no additional descriptors in channel
	desc2.BTCTRL.bit.VALID = 1;
	desc2.BTCTRL.bit.BLOCKACT = 0;
	descriptor_section[2] = desc2;

	return 0;

}

uint8_t dmac_desc3_init()
{

	desc3.BTCNT.reg = 16;
	desc3.BTCTRL.bit.BEATSIZE = 0;	//8bit
	desc3.BTCTRL.bit.SRCINC = 1;
//	desc3.BTCTRL.bit.DSTINC = 0;
	desc3.SRCADDR.reg = (uint32_t) &transmit_data_fc + sizeof(transmit_data_fc);
	desc3.DSTADDR.reg = (uint32_t) &(SERCOM1->USART.DATA.reg);	//dst address
	desc3.DESCADDR.reg = (uint32_t) 0;		//no additional descriptors in channel
	desc3.BTCTRL.bit.VALID = 1;
	desc3.BTCTRL.bit.BLOCKACT = 0;
	descriptor_section[3] = desc3;


	return 0;

}




void DMAC_0_Handler(void)	//transfer complete
{
	SERCOM0->USART.CTRLB.bit.RXEN = 0;

	if(uart_receive_xbee[0] == 0x42 && uart_receive_xbee[1] == 0x43){

			throttle_value = (uint16_t) ((uart_receive_xbee[2] << 8) | (uart_receive_xbee[3] & 0xff));
			roll_value = (uint16_t) ((uart_receive_xbee[4] << 8) | (uart_receive_xbee[5] & 0xff));
			pitch_value = (uint16_t) ((uart_receive_xbee[6] << 8) | (uart_receive_xbee[7] & 0xff));
			yaw_value = (uint16_t) ((uart_receive_xbee[8] << 8) | (uart_receive_xbee[9] & 0xff));
			AUX1_value = (uint16_t) ((uart_receive_xbee[10] << 8) | (uart_receive_xbee[1] & 0xff));


			throttle_trans = (uint16_t) (0x8000 | (*throttle_value + 24U));
			roll_trans = (uint16_t) (0x800U | (*roll_value + 24U));
			pitch_trans = (uint16_t) (0x1000U | (*pitch_value + 24U));
			yaw_trans = (uint16_t) (0x1800U | (*yaw_value + 24U));
			AUX1_trans = (uint16_t) (0x2000U | (*AUX1_value + 24U));



		} else {
			for(int i = 0; i < 12; i++) {		//if no char match, clear array
				uart_receive_xbee[i] = 0;
			}
		}





	DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;
	DMAC->Channel[0].CHINTFLAG.bit.TCMPL = 1;
	SERCOM0->USART.CTRLB.bit.RXEN = 1;
}

void DMAC_2_Handler(void)	//transfer complete
{

	DMAC->Channel[2].CHCTRLA.bit.ENABLE = 1;
	DMAC->Channel[2].CHINTFLAG.bit.TCMPL = 1;

}

