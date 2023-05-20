#include "pid.h"


uint16_t PI_Controller_Heading(double setpoint, int16_t actual, double *integral_error) {
	double actual_neg = actual;
	if(actual_neg > 1800){
		actual_neg = actual_neg - 3600.0;
	}
    double error = setpoint - actual_neg;
    if((error < 500.0) && (error > -500.0)) {
    	*integral_error += error * KI_Heading;
    }

    if(*integral_error > 200.0) {
 	*integral_error = 200.0;
    }
    if(*integral_error < -200.0){
 	*integral_error  = -200.0;
    }

    // PI calculation
    double output = KP_Heading * error + (*integral_error);

    return output + 1000;
}


uint16_t PI_Controller_Pitch(double setpoint, int16_t actual, double *integral_error) {

    double error = setpoint - actual;
    if((error < 500.0) && (error > -500.0)) {
    *integral_error += error * (KI_Pitch);
    }

    if(*integral_error > 300.0) {
    	*integral_error = 300.0;
    }
    if(*integral_error < -300.0){
    	*integral_error  = -300.0;
    }

    // PI calculation
    double output = KP_Pitch * error + (*integral_error);


    return output + 1000;
}


uint16_t PI_Controller_Roll(double setpoint, int16_t actual, double *integral_error) {

    double error = setpoint - actual;
    if((error < 500.0) && (error > -500.0)) {
    *integral_error += (error * KI_Roll);
    }

    if(*integral_error > 300.0) {
    	*integral_error = 300.0;
    }
    if(*integral_error < -300.0){
    	*integral_error  = -300.0;
    }

    // PI calculation
    double output = KP_Roll * error + (*integral_error);

    return output + 1000;
}

uint16_t PI_Controller_Alt(double setpoint, int16_t actual, double *integral_error) {

    double error = setpoint - actual;
    if((error < 500.0) && (error > -500.0)) {
    *integral_error += (error * KI_Alt);
    }

    if(*integral_error > 300.0) {
    	*integral_error = 300.0;
    }
    if(*integral_error < -300.0){
    	*integral_error  = -300.0;
    }

    // PI calculation
    double output = KP_Alt * error + (*integral_error);

    return output + 1000;
}
