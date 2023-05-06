#include "pid.h"


uint16_t PI_Controller_Heading(double setpoint, double actual, double *integral_error) {
    double error = setpoint - actual;
    *integral_error += error;

    // PI calculation
    double output = KP * error + KI * (*integral_error);

//    // Output saturation
//    if (output < OUTPUT_MIN) {
//        output = OUTPUT_MIN;
//    } else if (output > OUTPUT_MAX) {
//        output = OUTPUT_MAX;


    return output + 1000;
}
