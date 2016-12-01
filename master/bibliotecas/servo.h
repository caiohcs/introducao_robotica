#ifndef SERVO_H
#define SERVO_H
#include <math.h>

float deg_to_rad(float ang) {return ((ang/180)*M_PI);}  // converte grau pra radiano

float rad_to_deg(float ang) {return ((ang/M_PI)*180);}  // converte radiano pra grau


char *buffer;

struct servo
{
  	int num;
	int pulso;
	float ang;
	int pulsomin;
	int pulsomax;
	int resang;
	float angmin;
	float angmax;
	float angunit;		//Resolução do servo, em °/pulso.
	float angoffset;	//Correção no ângulo do servo necessária devido ao desgaste do braço.
};


void define_servos(struct servo *ptrservo[5])
{

	struct servo *base = ptrservo[0],
		*ombro = ptrservo[1],
		*cotovelo = ptrservo[2],
		*punho = ptrservo[3],
		*garra = ptrservo[4];

	// SERVO DA BASE
	base->num = 0;
	base->pulso = 1500;
	base->pulsomin = 500;
	base->pulsomax = 2380;
	base->resang = 180;
	base->angunit = (1.0*base->resang)/(base->pulsomax-base->pulsomin);
	base->angoffset = 12.446809;
	base->ang = base->angoffset;
	base->angmin = deg_to_rad((base->pulsomin-1500)*base->angunit + base->angoffset);
	base->angmax = deg_to_rad((base->pulsomax-1500)*base->angunit + base->angoffset);

	// SERVO DO OMBRO
	ombro->num = 1;
	ombro->pulso = 1500;
	ombro->pulsomin = 1200;
	ombro->pulsomax = 2000;
	ombro->resang = 90;
	ombro->angunit = (1.0*ombro->resang)/(ombro->pulsomax-ombro->pulsomin);
	ombro->angoffset = 90 + 6.625;
	ombro->ang = ombro->angoffset;
	ombro->angmin = deg_to_rad((ombro->pulsomin-1500)*ombro->angunit + ombro->angoffset);
	ombro->angmax = deg_to_rad((ombro->pulsomax-1500)*ombro->angunit + ombro->angoffset);

	// SERVO DO COTOVELO
	cotovelo->num = 2;
	cotovelo->pulso = 1500;
	cotovelo->pulsomin = 1100;
	cotovelo->pulsomax = 2100;
	cotovelo->resang = 90;
	cotovelo->angunit = ((-1.0)*cotovelo->resang)/(cotovelo->pulsomax-cotovelo->pulsomin);
	cotovelo->angoffset = 12.599998 - 90;
	cotovelo->ang = cotovelo->angoffset;
	cotovelo->angmin = deg_to_rad((cotovelo->pulsomax-1500)*cotovelo->angunit + cotovelo->angoffset); //o ang min do cot acontece quando pulso = pulsomax
	cotovelo->angmax = deg_to_rad((cotovelo->pulsomin-1500)*cotovelo->angunit + cotovelo->angoffset); //o ang max do cot acontece quando pulso = pulsomin

	// SERVO DO PUNHO
	punho->num = 3;
	punho->pulso = 1500;
	punho->pulsomin = 500;
	punho->pulsomax = 2500;
	punho->resang = 180;
	punho->angunit = (1.0*punho->resang)/(punho->pulsomax-punho->pulsomin);
	punho->angoffset = 0;
	punho->ang = punho->angoffset;
	punho->angmin = deg_to_rad((punho->pulsomin-1500)*punho->angunit + punho->angoffset);
	punho->angmax = deg_to_rad((punho->pulsomax-1500)*punho->angunit + punho->angoffset);

	// SERVO DA GARRA
	garra->num = 4;
	garra->pulso = 1500;
	garra->pulsomin = 1300;
	garra->pulsomax = 2400;
	garra->resang = 0;
	garra->angunit = 0;
	garra->angoffset = 0;
	garra->ang = garra->angoffset;
}

#endif
