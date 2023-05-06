/*
 * fc.c
 *
 *  Created on: Aug 29, 2019
 *      Author: codydroster
 */


#include "fc.h"
#include "dma.h"
#include "samd51j19a.h"

//transmit flight control values to FC
 uint8_t transmit_data_fc[18];

//receive attitude
 uint8_t receive_data_fc[12];


//transmit attitude, raw GPS to base station.
 uint8_t uart_transmit_xbee[12];


//receive flight control values
 uint8_t xbee_raw_receive[15];
 uint8_t xbee_rx_sorted[14];

//receive GPS RAW Data
 uint8_t receive_data_GPS[600];
 uint16_t GPS_index;


uint16_t AUX1_buffer[2] = {0,0};
uint16_t AUX2_buffer[2] = {0,0};

aircraft_ctrl fc_transmit;
aircraft_attitude drone_attitude;




aircraft_ctrl fc_transmit = {
		.throttle = 0x3e7,
		.roll = 0x3E8,
		.pitch = 0x3E8,
		.yaw = 0x3E8,
		.AUX1 = 0x3E8,
		.AUX2 = 0x3E8,

};


uint16_t GPS_index = 0;



//DMA transmit to FC periodically. Change TC0 TOP value????
void TC0_Handler(void)
{

	TC0->COUNT32.INTFLAG.bit.OVF = 1;

	//DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;
//	DMAC->Channel[1].CHINTFLAG.bit.TCMPL = 1;
	DMAC->Channel[2].CHINTFLAG.bit.TCMPL = 1;



//	DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;
	DMAC->Channel[2].CHCTRLA.bit.ENABLE = 1;

}

//transmit after ~600byte RAW GPS packet
//SERCOM2 ISR controls when this executes
void TC2_Handler(void)
{


	for(uint8_t i = 0; i < 12; i++) {

		SERCOM0->USART.DATA.reg = uart_transmit_xbee[i];
		while(!(SERCOM0->USART.INTFLAG.bit.TXC));
	}

	TC2->COUNT32.INTFLAG.bit.OVF = 1;
	NVIC_DisableIRQ(TC2_IRQn);

}

//receive complete
void SERCOM2_2_Handler(void)
{
	//GPS to Xbee
	SERCOM0->USART.DATA.reg = SERCOM2->USART.DATA.reg;

	//enable interrupt to send xbee heading -SERCOM 0 transmit complete

	//if TC2 interrupt is disabled.
	if (!(NVIC->ISER[(uint32_t)((int32_t)TC2_IRQn) >> 5] & (uint32_t)(1 << ((uint32_t)((int32_t)TC2_IRQn) & (uint32_t)0x1F)))) {

		TC2->COUNT32.INTENSET.bit.OVF = 1;
		TC2->COUNT32.INTFLAG.bit.OVF = 0xff;

		NVIC_ClearPendingIRQ(TC2_IRQn);
		NVIC_EnableIRQ(TC2_IRQn);

	}
//	}
	TC2->COUNT32.COUNT.reg = 0;



}



//Array  to be transmitted to FC, Spectrum 2048 protocol. DMA transfer periodically TC0
void update_channel_values(void)
{

	TC2->COUNT32.INTFLAG.bit.MC1 = 1;

	transmit_data_fc[0] = 0x0;	// missed frames [1]
	transmit_data_fc[1] = 0x0; // missed frames [2]

	transmit_data_fc[2] = (uint8_t) ((0x8000U | (fc_transmit.throttle + 24U)) >> 8UL);	// channel 1	//throttle
	transmit_data_fc[3] = ((uint8_t) (0x8000U | (fc_transmit.throttle + 24U))  & 0xFFUL);

	transmit_data_fc[4] = (uint8_t) ((0x800U | (fc_transmit.roll + 24U)) >> 8UL); // channel 2		//roll
	transmit_data_fc[5] = ((uint8_t) (0x800U | (fc_transmit.roll + 24U)) & 0xFFUL);

	transmit_data_fc[6] = (uint8_t) ((0x1000U | (fc_transmit.pitch + 24U)) >> 8UL); // channel 3		//pitch
	transmit_data_fc[7] = (uint8_t) (0x1000U | (fc_transmit.pitch + 24U));

	transmit_data_fc[8] = (uint8_t) ((0x1800U | (fc_transmit.yaw + 24U)) >> 8UL); // channel 4		//yaw
	transmit_data_fc[9] = ((uint8_t) (0x1800U | (fc_transmit.yaw + 24U)) & 0xFFUL);

	transmit_data_fc[10] = (uint8_t) ((0x2000U | (fc_transmit.AUX1 + 24U)) >> 8UL); // channel 5 AUX1
	transmit_data_fc[11] = ((uint8_t) (0x2000U | (fc_transmit.AUX1 + 24U)) & 0xFFUL);

	transmit_data_fc[12] = (uint8_t) ((0x2800U | (fc_transmit.AUX2 + 24U)) >> 8UL); // channel 6 AUX2
	transmit_data_fc[13] = ((uint8_t) (0x2800U | (fc_transmit.AUX2 + 24U)) & 0xFFUL);

	transmit_data_fc[14] = (uint8_t) ((0x3000U | (0x00 + 24U)) >> 8UL); // AUX3
	transmit_data_fc[15] = ((uint8_t) (0x3000U | (0x00 + 24U)) & 0xFFUL);


	transmit_data_fc[16] = 0xff;
	transmit_data_fc[17] = 0xff;


}



//receive from xbee, ISR
void SERCOM0_2_Handler(void)
{
	uint8_t received_value = SERCOM0->USART.DATA.reg;
	if(received_value == 0x42) {
		SERCOM0->USART.INTENCLR.bit.RXC = 1;
		DMAC->Channel[0].CHCTRLA.bit.ENABLE = 1;

	}
}

//receive from xbee. convert to values for transmission to FC. ONLY FLIGHT CONTROL VALUES
void DMAC_0_Handler(void)	//transfer complete
{
	SERCOM0->USART.CTRLB.bit.RXEN = 0;	//clear RX buffer



	if (xbee_raw_receive[0] == 0x42 && xbee_raw_receive[1] == 0x43 && xbee_raw_receive[14] == 0x43) {
		uint16_t throttle = (uint16_t) ((xbee_raw_receive[2] << 8) | (xbee_raw_receive[3] & 0xff));
		fc_transmit.throttle = throttle;

		uint16_t roll = (uint16_t) ((xbee_raw_receive[4] << 8) | (xbee_raw_receive[5] & 0xff));
		fc_transmit.roll = roll;

		uint16_t pitch = (uint16_t) ((xbee_raw_receive[6] << 8) | (xbee_raw_receive[7] & 0xff));
		fc_transmit.pitch = pitch;

		uint16_t yaw = (uint16_t) ((xbee_raw_receive[8] << 8) | (xbee_raw_receive[9] & 0xff));
		fc_transmit.yaw = yaw;

		uint16_t AUX1 = (uint16_t) ((xbee_raw_receive[10] << 8) | (xbee_raw_receive[11] & 0xff));
		fc_transmit.AUX1 = AUX1;

		uint16_t AUX2 = (uint16_t) ((xbee_raw_receive[12] << 8) | (xbee_raw_receive[13] & 0xff));
		fc_transmit.AUX2 = AUX2;

/*	} else {
	for(int i = 0; i < 9; i++) {		//if no char match, clear array
			xbee_raw_receive[i] = 0;

	}*/


	}
	DMAC->Channel[0].CHCTRLA.bit.ENABLE = 0;
	DMAC->Channel[0].CHINTFLAG.bit.TCMPL = 1;
	SERCOM0->USART.CTRLB.bit.RXEN = 1;
	SERCOM0->USART.INTENSET.bit.RXC = 1;



}


//FC first header byte RXC
void SERCOM3_2_Handler(void)
{
	uint8_t received_value = SERCOM3->USART.DATA.reg;
	if(received_value == 0x42) {
		SERCOM3->USART.INTENCLR.bit.RXC = 1;
		DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;


	}
}

void DMAC_1_Handler(void)	//transfer complete
{

	SERCOM3->USART.CTRLB.bit.RXEN = 0;	//clear RX buffer

	if(receive_data_fc[0] == 0x42 && receive_data_fc[1] == 0x43){
		drone_attitude.pitch = (uint16_t) ((receive_data_fc[2] << 8) | (receive_data_fc[3] & 0xff));
		drone_attitude.roll = (uint16_t) ((receive_data_fc[4] << 8) | (receive_data_fc[5] & 0xff));
		drone_attitude.heading = (uint16_t) ((receive_data_fc[6] << 8) | (receive_data_fc[7] & 0xff));
		drone_attitude.altitude = (uint16_t) ((receive_data_fc[8] << 8) | (receive_data_fc[9] & 0xff));

		uart_transmit_xbee[0] = 0x42;
		uart_transmit_xbee[1] = 0x43;
		uart_transmit_xbee[2] = 0x44;
		uart_transmit_xbee[3] = (uint8_t) (drone_attitude.pitch >> 8);
		uart_transmit_xbee[4] = (uint8_t) (drone_attitude.pitch & 0xff);
		uart_transmit_xbee[5] = (uint8_t) (drone_attitude.roll>> 8);
		uart_transmit_xbee[6] = (uint8_t) (drone_attitude.roll & 0xff);
		uart_transmit_xbee[7] = (uint8_t) (drone_attitude.heading >> 8);
		uart_transmit_xbee[8] = (uint8_t) (drone_attitude.heading & 0xff);
		uart_transmit_xbee[9] = (uint8_t) (drone_attitude.altitude >> 8);
		uart_transmit_xbee[10] = (uint8_t) (drone_attitude.altitude & 0xff);
		uart_transmit_xbee[11		] = 0x45;

	} else {
	for(int i = 0; i < 12; i++) {		//if no char match, clear array
			receive_data_fc[i] = 0;
		}
	}


		DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;
		DMAC->Channel[1].CHINTFLAG.bit.TCMPL = 1;
		SERCOM3->USART.CTRLB.bit.RXEN = 1;
		SERCOM3->USART.INTENSET.bit.RXC = 1;



}



