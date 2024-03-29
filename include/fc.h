#include "init.h"


typedef struct {


	uint16_t throttle;
	uint16_t roll;
	uint16_t pitch;
	uint16_t yaw;
	uint16_t AUX1;
	uint16_t AUX2;

} aircraft_ctrl;


typedef struct {

	uint16_t roll;
	uint16_t pitch;
	uint16_t heading;
	uint16_t altitude;

} aircraft_attitude;

extern aircraft_ctrl fc_transmit;
extern aircraft_attitude drone_attitude;



void update_channel_values();


//transmit flight control values to FC
extern uint8_t transmit_data_fc[18];

//receive attitude
extern uint8_t receive_data_fc[10];


//transmit attitude, raw GPS to base station.
extern uint8_t uart_transmit_xbee[12];


//receive flight control values
extern uint8_t xbee_raw_receive[14];
extern uint8_t xbee_rx_sorted[14];

//receive GPS RAW Data
extern uint8_t receive_data_GPS[600];
extern uint16_t GPS_index;


extern uint16_t AUX1_buffer[2];
extern uint16_t AUX2_buffer[2];

