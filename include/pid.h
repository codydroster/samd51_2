#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define KP_Heading 0.15 // Proportional gain - Heading
#define KI_Heading 0.03 // Integral gain - Heading

#define KP_Pitch 0.4 // Proportional gain - Heading
#define KI_Pitch 0.03 // Integral gain - Heading

#define KP_Roll 0.4 // Proportional gain - Heading
#define KI_Roll 0.03 // Integral gain - Heading

#define KP_Alt 0.4 // Proportional gain - Heading
#define KI_Alt 0.01 // Integral gain - Heading

uint16_t PI_Controller_Heading(double setpoint, int16_t actual, double *integral_error);

uint16_t PI_Controller_Pitch(double setpoint, int16_t actual, double *integral_error, double *heading_error);

uint16_t PI_Controller_Roll(double setpoint, int16_t actual, double *integral_error, double *heading_error);

uint16_t PI_Controller_Alt(double setpoint, int16_t actual, double *integral_error);
