#include <math.h>

#ifndef L1

	#define alt 6.731
	#define L1 14.605
	#define L2 18.7325
	#define L3 10.0

#endif


/*
 *
 *	CINEMÁTICA DIRETA
 *
 *
 */
float coor_x(float tet1, float tet2, float tet3, float tet4)            // usa a matrix de cinematica direta pra calcular X da ponta da garra em centimetros
{
        return ( ( L3*cos(deg_to_rad(tet1))*cos(deg_to_rad(tet2+tet3+tet4)) ) + ( L2*cos(deg_to_rad(tet1))*cos(deg_to_rad(tet2+tet3)) ) + ( L1*cos(deg_to_rad(tet1))*cos(deg_to_rad(tet2)) ) );
}

float coor_y(float tet1, float tet2, float tet3, float tet4)    // usa a matrix de cinematica inversa pra calcular Y da ponta da garra em centimetros
{
        return ( ( L3*sin(deg_to_rad(tet1))*cos(deg_to_rad(tet2+tet3+tet4)) ) + ( L2*sin(deg_to_rad(tet1))*cos(deg_to_rad(tet2+tet3)) ) + ( L1*sin(deg_to_rad(tet1))*cos(deg_to_rad(tet2)) ) );
}

float coor_z(float tet1, float tet2, float tet3, float tet4)    // usa a matrix de cinematica inversa pra calcular Y da ponta da garra em centimetros
{
        return ( ( L3*sin(deg_to_rad(tet2+tet3+tet4)) ) + ( L2*sin(deg_to_rad(tet2+tet3)) ) + ( L1*sin(deg_to_rad(tet2)) ) + alt );
}


/*
 *
 *	CINEMÁTICA INVERSA
 *
 *
 *	Recebe Xuser,Yuser,Zuser,PHIuser e retorna um ponteiro float *P
 *	P[0] = theta1
 *	P[1] = theta2
 *	P[2] = theta3
 *	P[3] = theta4
 *
 */
float *ptr_angles(float Xuser,float Yuser,float Zuser,float PHIuser)	// calcula os angulos usando x,y,z e inclinacao da garra
{
	float *P=malloc(sizeof(float)*4);
	float A,B,C;
	P[0] = atan2(Yuser,Xuser);
	if (P[0] < base.angmin || P[0] > base.angmax || isnan(P[0])){
		printf("Ang base invalido: %f\n", P[0]);
		P[0] = 1000;
		return P;
	}
	A = Xuser/cos(P[0]) - L3*cos(PHIuser);
	B = Zuser - L3*sin(PHIuser) - alt;

	//Em rad, a funcao acos retorna valores entre 0 e pi, mas o angulo do cotovelo é limitado a um intervalo negativo, então multiplica-se acos por -1
	P[2] = (-1)*acos( (pow(A,2) + pow(B,2) - pow(L1,2) - pow(L2,2))/(2*L1*L2) ); 	
	if (P[2] < cotovelo.angmin || P[2] > cotovelo.angmax || isnan(P[2])){
		printf("Ang cot invalido: %f\n", P[2]);
		P[0] = 1000;
		return P;
	}
	C = ( (pow(A,2) + pow(B,2) + pow(L1,2) - pow(L2,2))/(2*L1) );
	
	P[1] = asin(C/sqrt(pow(A,2)+pow(B,2))) - atan2(A,B);
	if (P[1] < ombro.angmin || P[1] > ombro.angmax || isnan(P[1])){
		printf("Ang omb invalido: %f\n", P[1]);
		P[1] = M_PI - asin(C/sqrt(pow(A,2)+pow(B,2))) - atan2(A,B);
		if (P[1] < ombro.angmin || P[1] > ombro.angmax || isnan(P[1])){
			printf("Ang omb invalido: %f\n", P[1]);
			P[0] = 1000;
			return P;
		}
	}
	P[3] = PHIuser - P[2] - P[1];
	if (P[3] < punho.angmin || P[3] > punho.angmax || isnan(P[3])){
		printf("Ang punho invalido: %f\n", P[3]);
		P[0] = 1000;
		return P;
	}
	return P;
}
