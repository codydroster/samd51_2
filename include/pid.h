#include <stdio.h>
#include <stdbool.h>

#define KP_Heading 0.15 // Proportional gain - Heading
#define KI_Heading 0.01 // Integral gain - Heading

#define KP_Pitch 0.15 // Proportional gain - Heading
#define KI_Pitch 0.01 // Integral gain - Heading

#define KP_Roll 0.15 // Proportional gain - Heading
#define KI_Roll 0.01 // Integral gain - Heading

uint16_t PI_Controller_Heading(double setpoint, int16_t actual, double *integral_error);

uint16_t PI_Controller_Pitch(double setpoint, int16_t actual, double *integral_error);

uint16_t PI_Controller_Roll(double setpoint, int16_t actual, double *integral_error);
