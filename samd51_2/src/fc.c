/*
 * fc.c
 *
 *  Created on: Aug 29, 2019
 *      Author: codydroster
 */


#include "fc.h"


uint8_t transmit_data_fc[16] = {0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xab,0xcd,0xef,0xff, 0xff};



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



//Array  to be transmitted to FC, Spectrum 2048 protocol.
void update_channel_values(void)
{

	transmit_data_fc[0] = 0x0;	// missed frames [1]
	transmit_data_fc[1] = 0x0; // missed frames [2]

	transmit_data_fc[2] = ((uint8_t) *throttle_trans >> 8UL);	// channel 1	//throttle
	transmit_data_fc[3] = ((uint8_t) *throttle_trans  & 0xFFUL);

	transmit_data_fc[4] = ((uint8_t) *roll_trans >> 8UL); // channel 2		//roll
	transmit_data_fc[5] = ((uint8_t) *roll_trans & 0xFFUL);

	transmit_data_fc[6] = ((uint8_t) *pitch_trans >> 8UL); // channel 3		//pitch
	transmit_data_fc[7] = ((uint8_t) *pitch_trans & 0xFFUL);

	transmit_data_fc[8] = ((uint8_t) *yaw_trans >> 8UL); // channel 4		//yaw
	transmit_data_fc[9] = ((uint8_t) *yaw_trans & 0xFFUL);

	transmit_data_fc[10] = ((uint8_t) *AUX1_trans >> 8UL); // channel 5 AUX1
	transmit_data_fc[11] = ((uint8_t) *AUX1_trans & 0xFFUL);

	transmit_data_fc[12] = 0xff;
	transmit_data_fc[13] = 0xff;
	transmit_data_fc[14] = 0xff;
	transmit_data_fc[15] = 0xff;


}
