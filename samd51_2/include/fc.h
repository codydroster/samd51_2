#include "init.h"


typedef struct {
	uint16_t throttle;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
	uint16_t AUX1;

} aircraft_values;


aircraft_values fc_transmit;



void update_channel_values();


//transmit flight control values to FC
 uint8_t transmit_data_fc[16];

//receive attitude
uint8_t receive_data_fc[16];


//transmit attitude, raw GPS to base station.
uint8_t uart_transmit_xbee[12];

//receive flight control values
uint8_t xbee_raw_receive[12];



