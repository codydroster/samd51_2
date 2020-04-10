/*
 * fc.c
 *
 *  Created on: Aug 29, 2019
 *      Author: codydroster
 */


#include "fc.h"


//uint8_t uart_transmit_xbee[14] = {0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xab};

aircraft_ctrl fc_transmit = {
		.throttle = 0x00,
		.roll = 0x3E8,
		.pitch = 0x3E8,
		.yaw = 0x3E8,
		.AUX1 = 0x3E8,
		.AUX2 = 0x3E8,

};

uint8_t uart_transmit_xbee[10] = {0x42,0x43,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

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


void TC2_Handler(void)
{

	TC2->COUNT32.INTFLAG.bit.OVF = 1;

	//DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;
//	DMAC->Channel[1].CHINTFLAG.bit.TCMPL = 1;
//	DMAC->Channel[3].CHINTFLAG.bit.TCMPL = 1;
	//DMAC->Channel[2].CHINTFLAG.bit.SUSP = 1;


//	DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;
//	DMAC->Channel[3].CHCTRLA.bit.ENABLE = 1;
}



//Array  to be transmitted to FC, Spectrum 2048 protocol. DMA transfer periodically TC0
void update_channel_values(void)
{

	transmit_data_fc[0] = 0x0;	// missed frames [1]
	transmit_data_fc[1] = 0x0; // missed frames [2]

	transmit_data_fc[2] = (uint8_t) ((uint16_t) (0x8000U | (fc_transmit.throttle + 24U)) >> 8UL);	// channel 1	//throttle
	transmit_data_fc[3] = ((uint16_t) (0x8000U | (fc_transmit.throttle + 24U))  & 0xFFUL);

	transmit_data_fc[4] = (uint8_t) ((uint16_t) (0x800U | (fc_transmit.roll + 24U)) >> 8UL); // channel 2		//roll
	transmit_data_fc[5] = ((uint16_t) (0x800U | (fc_transmit.roll + 24U)) & 0xFFUL);

	transmit_data_fc[6] = (uint8_t) ((uint16_t) (0x1000U | (fc_transmit.pitch + 24U)) >> 8UL); // channel 3		//pitch
	transmit_data_fc[7] = (uint16_t) (0x1000U | (fc_transmit.pitch + 24U));

	transmit_data_fc[8] = (uint8_t) ((uint16_t) (0x1800U | (fc_transmit.yaw + 24U)) >> 8UL); // channel 4		//yaw
	transmit_data_fc[9] = ((uint16_t) (0x1800U | (fc_transmit.yaw + 24U)) & 0xFFUL);

	transmit_data_fc[10] = (uint8_t) ((uint16_t) (0x2000U | (fc_transmit.AUX1 + 24U)) >> 8UL); // channel 5 AUX1
	transmit_data_fc[11] = ((uint16_t) (0x2000U | (fc_transmit.AUX1 + 24U)) & 0xFFUL);

	transmit_data_fc[12] = (uint8_t) ((uint16_t) (0x2800U | (fc_transmit.AUX2 + 24U)) >> 8UL); // channel 5 AUX1
	transmit_data_fc[13] = ((uint16_t) (0x2800U | (fc_transmit.AUX2 + 24U)) & 0xFFUL);
	transmit_data_fc[14] = 0xff;
	transmit_data_fc[15] = 0xff;



}




//receive from xbee. convert to values for transmission to FC. ONLY FLIGHT CONTROL VALUES

void DMAC_0_Handler(void)	//transfer complete
{
	SERCOM0->USART.CTRLB.bit.RXEN = 0;	//clear RX buffer

	uint8_t index_start;
	uint8_t xbee_rx_sorted[14];

	for (uint8_t i = 0; i< 13; i++) {
		if(xbee_raw_receive[i] == 0x42) {
			index_start = i;
			break;
		}

	}
	if (index_start == 14) {					//not sure if this loop actually works.
												//special case, not sure how to loop back to 0
		if(xbee_raw_receive[0] == 0x43) {
			xbee_rx_sorted[0] = xbee_raw_receive[14];

			for (uint8_t i = 0; i < 14; i++) {
				xbee_rx_sorted[i+1] = xbee_raw_receive[i];
			}

		}


	} else {									///this cant work???	//maybe now it does? 4/9
		if(xbee_raw_receive[index_start + 1] == 0x43) {
			for (uint8_t i = 0; i < (14 - index_start); i++) {
				xbee_rx_sorted[i] = xbee_raw_receive[index_start + i];

			}
			for (uint8_t i = 0; i < index_start; i++) {
				xbee_rx_sorted[i + (14 - index_start)] = xbee_raw_receive[i];
		}
	}







	}







	fc_transmit.throttle = (uint16_t) ((xbee_rx_sorted[2] << 8) | (xbee_rx_sorted[3] & 0xff));
	fc_transmit.roll = (uint16_t) ((xbee_rx_sorted[4] << 8) | (xbee_rx_sorted[5] & 0xff));
	fc_transmit.pitch = (uint16_t) ((xbee_rx_sorted[6] << 8) | (xbee_rx_sorted[7] & 0xff));
	fc_transmit.yaw = (uint16_t) ((xbee_rx_sorted[8] << 8) | (xbee_rx_sorted[9] & 0xff));
	fc_transmit.AUX1 = (uint16_t) ((xbee_rx_sorted[10] << 8) | (xbee_rx_sorted[11] & 0xff));
	fc_transmit.AUX2 = (uint16_t) ((xbee_rx_sorted[12] << 8) | (xbee_rx_sorted[13] & 0xff));






/*	if(xbee_raw_receive[0] == 0x42 && xbee_raw_receive[1] == 0x43) {





			 fc_transmit.throttle = (uint16_t) ((xbee_raw_receive[2] << 8) | (xbee_raw_receive[3] & 0xff));
			 fc_transmit.roll = (uint16_t) ((xbee_raw_receive[4] << 8) | (xbee_raw_receive[5] & 0xff));
			 fc_transmit.pitch = (uint16_t) ((xbee_raw_receive[6] << 8) | (xbee_raw_receive[7] & 0xff));
			 fc_transmit.yaw = (uint16_t) ((xbee_raw_receive[8] << 8) | (xbee_raw_receive[9] & 0xff));
			 fc_transmit.AUX1 = (uint16_t) ((xbee_raw_receive[10] << 8) | (xbee_raw_receive[11] & 0xff));
			 fc_transmit.AUX2 = (uint16_t) ((xbee_raw_receive[12] << 8) | (xbee_raw_receive[13] & 0xff));
*/





	/*
		else {
		for(int i = 0; i < 12; i++) {		//if no char match, clear array
				xbee_raw_receive[i] = 0;
			}
		}
*/

	DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;
	DMAC->Channel[0].CHINTFLAG.bit.TCMPL = 1;
	SERCOM0->USART.CTRLB.bit.RXEN = 1;
}



void DMAC_2_Handler(void)	//transfer complete
{

	SERCOM3->USART.CTRLB.bit.RXEN = 0;	//clear RX buffer

	if(receive_data_fc[0] == 0x42 && receive_data_fc[1] == 0x43){
		drone_attitude.pitch = (uint16_t) ((receive_data_fc[2] << 8) | (receive_data_fc[3] & 0xff));
		drone_attitude.roll = (uint16_t) ((receive_data_fc[4] << 8) | (receive_data_fc[5] & 0xff));
		drone_attitude.heading = (uint16_t) ((receive_data_fc[6] << 8) | (receive_data_fc[7] & 0xff));
		drone_attitude.altitude = (uint16_t) ((receive_data_fc[8] << 8) | (receive_data_fc[9] & 0xff));


		uart_transmit_xbee[2] = (uint8_t) (drone_attitude.pitch >> 8);
		uart_transmit_xbee[3] = (uint8_t) (drone_attitude.pitch & 0xff);
		uart_transmit_xbee[4] = (uint8_t) (drone_attitude.roll>> 8);
		uart_transmit_xbee[5] = (uint8_t) (drone_attitude.roll & 0xff);
		uart_transmit_xbee[6] = (uint8_t) (drone_attitude.heading >> 8);
		uart_transmit_xbee[7] = (uint8_t) (drone_attitude.heading & 0xff);
		uart_transmit_xbee[8] = (uint8_t) (drone_attitude.altitude >> 8);
		uart_transmit_xbee[9] = (uint8_t) (drone_attitude.altitude & 0xff);

	} else {
	for(int i = 0; i < 12; i++) {		//if no char match, clear array
			receive_data_fc[i] = 0;
		}
	}


		DMAC->Channel[2].CHCTRLA.bit.ENABLE = 1;
		DMAC->Channel[2].CHINTFLAG.bit.TCMPL = 1;
		SERCOM3->USART.CTRLB.bit.RXEN = 1;



}


