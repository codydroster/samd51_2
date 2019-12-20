/*
 * fc.c
 *
 *  Created on: Aug 29, 2019
 *      Author: codydroster
 */


#include "fc.h"


//uint8_t transmit_data_fc[16] = {0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xff};


//DMA transmit to FC periodically. Change TC0 TOP value????
void TC0_Handler(void)
{

	TC0->COUNT32.INTFLAG.bit.OVF = 1;

	//DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;
	DMAC->Channel[1].CHINTFLAG.bit.TCMPL = 1;
	DMAC->Channel[3].CHINTFLAG.bit.TCMPL = 1;
	//DMAC->Channel[2].CHINTFLAG.bit.SUSP = 1;


	DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;
	DMAC->Channel[3].CHCTRLA.bit.ENABLE = 1;
}



//Array  to be transmitted to FC, Spectrum 2048 protocol. DMA transfer periodically TC0
void update_channel_values(void)
{

	transmit_data_fc[0] = 0x0;	// missed frames [1]
	transmit_data_fc[1] = 0x0; // missed frames [2]

	transmit_data_fc[2] = ((uint8_t) fc_transmit.throttle >> 8UL);	// channel 1	//throttle
	transmit_data_fc[3] = ((uint8_t) fc_transmit.throttle  & 0xFFUL);

	transmit_data_fc[4] = ((uint8_t) fc_transmit.roll >> 8UL); // channel 2		//roll
	transmit_data_fc[5] = ((uint8_t) fc_transmit.roll & 0xFFUL);

	transmit_data_fc[6] = ((uint8_t) fc_transmit.pitch >> 8UL); // channel 3		//pitch
	transmit_data_fc[7] = ((uint8_t) fc_transmit.pitch & 0xFFUL);

	transmit_data_fc[8] = ((uint8_t) fc_transmit.yaw >> 8UL); // channel 4		//yaw
	transmit_data_fc[9] = ((uint8_t) fc_transmit.yaw & 0xFFUL);

	transmit_data_fc[10] = ((uint8_t) fc_transmit.AUX1 >> 8UL); // channel 5 AUX1
	transmit_data_fc[11] = ((uint8_t) fc_transmit.AUX1 & 0xFFUL);

	transmit_data_fc[12] = 0xff;
	transmit_data_fc[13] = 0xff;
	transmit_data_fc[14] = 0xff;
	transmit_data_fc[15] = 0xff;


}




//receive from xbee. convert to values for transmission to FC. ONLY FLIGHT CONTROL VALUES

void DMAC_0_Handler(void)	//transfer complete
{
	SERCOM0->USART.CTRLB.bit.RXEN = 0;	//clear RX buffer

	if(xbee_raw_receive[0] == 0x42 && xbee_raw_receive[1] == 0x43){

			uint16_t throttle = (uint16_t) ((xbee_raw_receive[2] << 8) | (xbee_raw_receive[3] & 0xff));
			uint16_t roll = (uint16_t) ((xbee_raw_receive[4] << 8) | (xbee_raw_receive[5] & 0xff));
			uint16_t pitch = (uint16_t) ((xbee_raw_receive[6] << 8) | (xbee_raw_receive[7] & 0xff));
			uint16_t yaw = (uint16_t) ((xbee_raw_receive[8] << 8) | (xbee_raw_receive[9] & 0xff));
			uint16_t AUX1 = (uint16_t) ((xbee_raw_receive[10] << 8) | (xbee_raw_receive[11] & 0xff));


			fc_transmit.throttle = (uint16_t) (0x8000 | (throttle + 24U));
			fc_transmit.roll = (uint16_t) (0x800U | (roll + 24U));
			fc_transmit.pitch = (uint16_t) (0x1000U | (pitch + 24U));
			fc_transmit.yaw = (uint16_t) (0x1800U | (yaw + 24U));
			fc_transmit.AUX1 = (uint16_t) (0x2000U | (AUX1 + 24U));



		} else {
			for(int i = 0; i < 12; i++) {		//if no char match, clear array
				xbee_raw_receive[i] = 0;
			}
		}


	DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;
	DMAC->Channel[0].CHINTFLAG.bit.TCMPL = 1;
	SERCOM0->USART.CTRLB.bit.RXEN = 1;
}



