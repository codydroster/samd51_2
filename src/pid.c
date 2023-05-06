#include "pid.h"


uint16_t PI_Controller_Heading(double setpoint, int16_t actual, double *integral_error) {
	double actual_neg = actual;
	if(actual_neg > 1800){
		actual_neg = actual_neg - 3600.0;
	}
    double error = setpoint - actual_neg;
    *integral_error += error;

    // PI calculation
    double output = KP_Heading * error + KI_Heading * (*integral_error);

    return output + 1000;
}


uint16_t PI_Controller_Pitch(double setpoint, int16_t actual, double *integral_error) {
	double actual_neg = actual;
	if(actual_neg > 1800){
		actual_neg = actual_neg - 3600.0;
	}
    double error = setpoint - actual_neg;
    *integral_error += error;

    // PI calculation
    double output = KP_Heading * error + KI_Heading * (*integral_error);

    return output + 1000;
}


uint16_t PI_Controller_Roll(double setpoint, int16_t actual, double *integral_error) {
	double actual_neg = actual;
	if(actual_neg > 1800){
		actual_neg = actual_neg - 3600.0;
	}
    double error = setpoint - actual_neg;
    *integral_error += error;

    // PI calculation
    double output = KP_Heading * error + KI_Heading * (*integral_error);

    return output + 1000;
}
