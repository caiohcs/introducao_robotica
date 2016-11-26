#include "./servo.h"
#include <math.h>

int calc_pul(struct servo *Servo){return (int) roundf((1500 + (Servo->ang - Servo->angoffset)/Servo->angunit));}

float calc_angdeg(struct servo *Servo){return ((Servo->pulso-1500)*Servo->angunit + Servo->angoffset);}
