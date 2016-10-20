#include "./servo.h"
#include <math.h>

float deg_to_rad(float ang) {return ((ang/180)*M_PI);}  // converte grau pra radiano

float rad_to_deg(float ang) {return ((ang/M_PI)*180);}  // converte radiano pra grau

float calc_ang_pul(struct servo *Servo){return (1500 + Servo->ang/Servo->angunit);}  // usa o angulo pra calcular o pulso (cinematica inversa)

float calc_ang_deg(struct servo *Servo){return ( (Servo->pulso-1500)*Servo->angunit + Servo->angoffset);}  // usa o pulso pra calcular o angulo (cinematica direta)

void calc_all_ang(void)
{
	calc_ang_deg(&base);
        calc_ang_deg(&ombro);
        calc_ang_deg(&cotovelo);
        calc_ang_deg(&punho);
        calc_ang_deg(&garra);
}
