#include <stdio.h>
#include <stdbool.h>

#define KP 0.5 // Proportional gain
#define KI 0.01 // Integral gain
#define SETPOINT_MIN 0
#define SETPOINT_MAX 3600
#define OUTPUT_MIN 0
#define OUTPUT_MAX 1000

uint16_t PI_Controller_Heading(double setpoint, double actual, double *integral_error);
