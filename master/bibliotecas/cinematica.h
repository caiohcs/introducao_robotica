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

	/* Em rad, a funcao acos retorna valores entre 0 e pi, mas o angulo do cotovelo é limitado a um intervalo negativo, então multiplica-se acos por -1
	 * Ou seja, não há ambiguidade nos valores do angulo do cotovelo, já que os únicos valores possíveis eram um no intervalo (0, pi) e outro que
	 * é o negativo desse, ou seja, no intervalo (0, -pi).
	 */
	P[2] = (-1)*acos( (pow(A,2) + pow(B,2) - pow(L1,2) - pow(L2,2))/(2*L1*L2) ); 	
	if (P[2] < cotovelo.angmin || P[2] > cotovelo.angmax || isnan(P[2])){
		printf("Ang cot invalido: %f\n", P[2]);
		P[0] = 1000;
		return P;
	}
	C = ( (pow(A,2) + pow(B,2) + pow(L1,2) - pow(L2,2))/(2*L1) );
	
	/* 
	 * Essa é uma resposta, que não sei se é possível, para o angulo do ombro. A função asin retorna valores entre (-pi/2,pi/2), enquanto que a atan2 retorna entre
	 * (-pi,pi). Para todas as posições que testei, essa resposta não funcionou (repare na mensagem ang omb invalido ao testar uma posicao), deu angulo inválido.
	 * O intervalo total então é (-3pi/2,3pi/2).
	 */
	P[1] = asin(C/sqrt(pow(A,2)+pow(B,2))) - atan2(A,B);
	if (P[1] < ombro.angmin || P[1] > ombro.angmax || isnan(P[1])){
		printf("Ang omb invalido: %f\n", P[1]);
		/*
		 * Já essa outra resposta pro ombro, pegando o angulo secundário do sin (fazendo pi - asin), funcionou para algumas posições.
		 * Nesse caso, o intervalo de pi - asin é (pi/2,3pi/2) e o do atan2 continua sendo (-pi,pi).
		 * Assim, o intervalo total é (-pi/2,5pi/2)
		 * A limitação física do ombro é (ombro.angmin e ombro.angmax) que dá aproximadamente (1,09;2,66) em radianos.
		 * Creio que analisando o intervalo total das duas respostas e a da limitação física do ombro seja possível provar que essa é a única possível
		 */
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
