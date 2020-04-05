#include "init.h"


typedef struct {
	uint16_t tx_throttle;
	uint16_t tx_roll;
	uint16_t tx_pitch;
	uint16_t tx_yaw;
	uint16_t tx_AUX1;
	uint16_t tx_AUX2;

	uint16_t throttle;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
	uint16_t AUX1;
	uint16_t AUX2;

} aircraft_values;

uint16_t test;



aircraft_values fc_transmit;




void update_channel_values();


//transmit flight control values to FC
 uint8_t transmit_data_fc[16];

//receive attitude
uint8_t receive_data_fc[16];


//transmit attitude, raw GPS to base station.
uint8_t uart_transmit_xbee[14];

//receive flight control values
uint8_t xbee_raw_receive[14];



