#ifndef SERVO_H
#define SERVO_H
struct servo
{
  	int num;
	int pulso;
	float ang;
	int pulsomin;
	int pulsomax;
	int angmax;
	float angunit;
	float angoffset;
};

struct servo base;
struct servo ombro;
struct servo cotovelo;
struct servo punho;
struct servo garra;

void define_servos()
{
	// SERVO DA BASE
//	struct servo base;
	base.num = 0;
	base.pulso = 1500;
	base.pulsomin = 500;
	base.pulsomax = 2380;
	base.angmax = 180;
	base.angunit = (1.0*base.angmax)/(base.pulsomax-base.pulsomin);
	base.angoffset = 12.446809;
	base.ang = base.angoffset;

	// SERVO DO OMBRO
//	struct servo ombro;
	ombro.num = 1;
	ombro.pulso = 1500;
	ombro.pulsomin = 1200;
	ombro.pulsomax = 2000;
	ombro.angmax = 90;
	ombro.angunit = (1.0*ombro.angmax)/(ombro.pulsomax-ombro.pulsomin);
	ombro.angoffset = 90 + 6.625;
	ombro.ang = ombro.angoffset;

	// SERVO DO COTOVELO
//	struct servo cotovelo;
	cotovelo.num = 2;
	cotovelo.pulso = 1500;
	cotovelo.pulsomin = 1100;
	cotovelo.pulsomax = 2100;
	cotovelo.angmax = 90;
	cotovelo.angunit = ((-1.0)*cotovelo.angmax)/(cotovelo.pulsomax-cotovelo.pulsomin);
	cotovelo.angoffset = 12.599998 - 90;
	cotovelo.ang = cotovelo.angoffset;

	// SERVO DO PUNHO
//	struct servo punho;
	punho.num = 3;
	punho.pulso = 1500;
	punho.pulsomin = 500;
	punho.pulsomax = 2500;
	punho.angmax = 180;
	punho.angunit = (1.0*punho.angmax)/(punho.pulsomax-punho.pulsomin);
	punho.angoffset = 0;
	punho.ang = punho.angoffset;

	// SERVO DA GARRA
//	struct servo garra;
	garra.num = 4;
	garra.pulso = 1500;
	garra.pulsomin = 1300;
	garra.pulsomax = 2400;
	garra.angmax = 0;
	garra.angunit = 0;
	garra.angoffset = 0;
	garra.ang = garra.angoffset;
}

#endif
