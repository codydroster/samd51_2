/*
 * fc.c
 *
 *  Created on: Aug 29, 2019
 *      Author: codydroster
 */


#include "fc.h"
#include "dma.h"
#include "samd51j19a.h"
#include "pid.h"




//transmit flight control values to FC
 uint8_t transmit_data_fc[18];

//receive attitude
 uint8_t receive_data_fc[11];


//transmit attitude, raw GPS to base station.
 uint8_t uart_transmit_xbee[12];


//receive flight control values
 uint8_t xbee_raw_receive[23];
 uint8_t xbee_rx_sorted[14];

//receive GPS RAW Data
 uint8_t receive_data_GPS[600];
 uint16_t GPS_index;


uint16_t AUX1_buffer[2] = {0,0};
uint16_t AUX2_buffer[2] = {0,0};

aircraft_ctrl fc_transmit;
aircraft_ctrl fc_transmit_auto;

aircraft_attitude drone_attitude;
aircraft_error gps_error;


// 0-2000
aircraft_ctrl fc_transmit = {
		.throttle = 0x3e7,
		.roll = 0x3E8,
		.pitch = 0x3E8,
		.yaw = 0x3E8,
		.AUX1 = 0x3E8,
		.AUX2 = 0x3E8,
		.AUX3 = 0x3e8

};




uint16_t GPS_index = 0;

double heading_error_value = 0;
double *heading_error = &heading_error_value;

double roll_error_value = 0;
double *roll_error = &roll_error_value;

double pitch_error_value = 0;
double *pitch_error = &pitch_error_value;

double alt_error_value = 0;
double *alt_error = &alt_error_value;



//DMA transmit to FC periodically. Change TC0 TOP value????
void TC0_Handler(void)
{

	TC0->COUNT32.INTFLAG.bit.MC0 = 1;

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

	TC2->COUNT32.INTFLAG.bit.MC0 = 1;
	NVIC_DisableIRQ(TC2_IRQn);

}

void TC3_Handler(void)
{

	nav_update_autonomous();

//	PORT->Group[0].OUT.reg = (1 << 21);




	TC3->COUNT32.INTFLAG.bit.MC0 = 1;


}


void TC4_Handler(void)
{


	TC4->COUNT32.INTFLAG.bit.MC0 = 1;

}

//receive complete
void SERCOM2_2_Handler(void)
{
	//GPS to Xbee
	SERCOM0->USART.DATA.reg = SERCOM2->USART.DATA.reg;

	//enable interrupt to send xbee heading -SERCOM 0 transmit complete

	//if TC2 interrupt is disabled.
	if (!(NVIC->ISER[(uint32_t)((int32_t)TC2_IRQn) >> 5] & (uint32_t)(1 << ((uint32_t)((int32_t)TC2_IRQn) & (uint32_t)0x1F)))) {

		TC2->COUNT32.INTENSET.bit.MC0 = 1;
		TC2->COUNT32.INTFLAG.bit.MC0 = 0xff;

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
	if(fc_transmit.AUX1 < 1400){
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

		//knob?
		transmit_data_fc[14] = (uint8_t) ((0x3000U | (fc_transmit.AUX3 + 24U)) >> 8UL); // AUX3
		transmit_data_fc[15] = ((uint8_t) (0x3000U | (fc_transmit.AUX3 + 24U)) & 0xFFUL);

		transmit_data_fc[16] = 0xff;
		transmit_data_fc[17] = 0xff;
	}


	if(fc_transmit.AUX1 > 1400){
		transmit_data_fc[0] = 0x0;	// missed frames [1]
		transmit_data_fc[1] = 0x0; // missed frames [2]

		transmit_data_fc[2] = (uint8_t) ((0x8000U | (fc_transmit.throttle + 24U)) >> 8UL);	// channel 1	//throttle
		transmit_data_fc[3] = ((uint8_t) (0x8000U | (fc_transmit.throttle + 24U))  & 0xFFUL);

		transmit_data_fc[4] = (uint8_t) ((0x800U | (fc_transmit_auto.roll + 24U)) >> 8UL); // channel 2		//roll
		transmit_data_fc[5] = ((uint8_t) (0x800U | (fc_transmit_auto.roll + 24U)) & 0xFFUL);

		transmit_data_fc[6] = (uint8_t) ((0x1000U | (fc_transmit_auto.pitch + 24U)) >> 8UL); // channel 3		//pitch
		transmit_data_fc[7] = (uint8_t) (0x1000U | (fc_transmit_auto.pitch + 24U));

		transmit_data_fc[8] = (uint8_t) ((0x1800U | (fc_transmit_auto.yaw + 24U)) >> 8UL); // channel 4		//yaw
		transmit_data_fc[9] = ((uint8_t) (0x1800U | (fc_transmit_auto.yaw + 24U)) & 0xFFUL);

		transmit_data_fc[10] = (uint8_t) ((0x2000U | (fc_transmit.AUX1 + 24U)) >> 8UL); // channel 5 AUX1
		transmit_data_fc[11] = ((uint8_t) (0x2000U | (fc_transmit.AUX1 + 24U)) & 0xFFUL);

		transmit_data_fc[12] = (uint8_t) ((0x2800U | (fc_transmit.AUX2 + 24U)) >> 8UL); // channel 6 AUX2
		transmit_data_fc[13] = ((uint8_t) (0x2800U | (fc_transmit.AUX2 + 24U)) & 0xFFUL);

		//knob?
		transmit_data_fc[14] = (uint8_t) ((0x3000U | (fc_transmit.AUX3 + 24U)) >> 8UL); // AUX3
		transmit_data_fc[15] = ((uint8_t) (0x3000U | (fc_transmit.AUX3 + 24U)) & 0xFFUL);

		transmit_data_fc[16] = 0xff;
		transmit_data_fc[17] = 0xff;

	}

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



	if (xbee_raw_receive[0] == 0x42 && xbee_raw_receive[1] == 0x43 && xbee_raw_receive[22] == 0x43) {
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

		uint16_t AUX3 = (uint16_t) ((xbee_raw_receive[14] << 8) | (xbee_raw_receive[15] & 0xff));
		fc_transmit.AUX3 = AUX3;

		int16_t error_lat = (int16_t) ((xbee_raw_receive[16] << 8) | (xbee_raw_receive[17] & 0xff));
		gps_error.latitude = error_lat;

		int16_t error_long = (int16_t) ((xbee_raw_receive[18] << 8) | (xbee_raw_receive[19] & 0xff));
		gps_error.longitude = error_long;

		int16_t error_alt = (int16_t) ((xbee_raw_receive[20] << 8) | (xbee_raw_receive[21] & 0xff));
		gps_error.altitude = error_alt;

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

	if(receive_data_fc[0] == 0x43 && receive_data_fc[1] == 0x44){
		drone_attitude.roll = (int16_t) ((receive_data_fc[2] << 8) | (receive_data_fc[3] & 0xff));
		drone_attitude.pitch = (int16_t) ((receive_data_fc[4] << 8) | (receive_data_fc[5] & 0xff));
		drone_attitude.heading = (int16_t) ((receive_data_fc[6] << 8) | (receive_data_fc[7] & 0xff));
		drone_attitude.altitude = (int16_t) ((receive_data_fc[8] << 8) | (receive_data_fc[9] & 0xff));

		uart_transmit_xbee[0] = 0x42;
		uart_transmit_xbee[1] = 0x43;
		uart_transmit_xbee[2] = 0x44;
		uart_transmit_xbee[3] = (uint8_t) (drone_attitude.roll >> 8);
		uart_transmit_xbee[4] = (uint8_t) (drone_attitude.roll & 0xff);
		uart_transmit_xbee[5] = (uint8_t) (drone_attitude.pitch >> 8);
		uart_transmit_xbee[6] = (uint8_t) (drone_attitude.pitch & 0xff);
		uart_transmit_xbee[7] = (uint8_t) (drone_attitude.heading >> 8);
		uart_transmit_xbee[8] = (uint8_t) (drone_attitude.heading & 0xff);
		uart_transmit_xbee[9] = (uint8_t) (drone_attitude.altitude >> 8);
		uart_transmit_xbee[10] = (uint8_t) (drone_attitude.altitude & 0xff);
		uart_transmit_xbee[11] = 0x45;

	} else {
	for(int i = 0; i < 12; i++) {		//if no char match, clear array
			receive_data_fc[i] = 0;
		}
	}


//		DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;
		DMAC->Channel[1].CHINTFLAG.bit.TCMPL = 1;
		SERCOM3->USART.CTRLB.bit.RXEN = 1;
		SERCOM3->USART.INTENSET.bit.RXC = 1;


}

void nav_update_autonomous(void) {
	fc_transmit_auto.yaw = PI_Controller_Heading(0.0, drone_attitude.heading, heading_error);
	fc_transmit_auto.roll = PI_Controller_Roll(0.0, gps_error.longitude, roll_error);
	fc_transmit_auto.pitch = PI_Controller_Pitch(0.0, gps_error.latitude, pitch_error);
	fc_transmit_auto.throttle = PI_Controller_Alt(0.0, gps_error.altitude, alt_error);

}

