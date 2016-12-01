#ifndef FUNCOES
#define FUNCOES
#include "./graphical.h"

void cinversa(int serial_fd, struct servo *ptrservo[5], float Xuser, float Yuser, float Zuser, float PHIuser)
{

	struct servo *base = ptrservo[0],
                *ombro = ptrservo[1],
                *cotovelo = ptrservo[2],
                *punho = ptrservo[3];
	PHIuser = deg_to_rad(PHIuser);
	printf("%f %f %f %f\n", Xuser, Yuser, Zuser, PHIuser);
	float *P;
	P = ptr_angles(ptrservo, Xuser, (-1)*Yuser, Zuser, PHIuser);
	printf("Angulos: %f %f %f %f\n", P[0], P[1], P[2], P[3]);

	if(P[0] != 1000){
		printf("posicao valida\n");
		
		base->ang = rad_to_deg(P[0]);
		ombro->ang = rad_to_deg(P[1]);
		cotovelo->ang = rad_to_deg(P[2]);
		punho->ang = rad_to_deg(P[3]);

		base->pulso = trava(base, calc_pul(base));
		ombro->pulso = trava(ombro, calc_pul(ombro));
		cotovelo->pulso = trava(cotovelo, calc_pul(cotovelo));
		punho->pulso = trava(punho, calc_pul(punho));
		
		memset(buffer, 0, sizeof(char)*150);
		sprintf(buffer ,"#%dP%d#%dP%d#%dP%d#%dP%dT4000\r",
		base->num, base->pulso,
		ombro->num, ombro->pulso,
		cotovelo->num, cotovelo->pulso,
		punho->num, punho->pulso);

		enviar_comando(buffer, serial_fd);
	} else {
		printf("Posição invalida!\n");
	}

	printf("Angulos: %f %f %f %f\n", base->ang, ombro->ang, cotovelo->ang, punho->ang);
	printf("Pulsos: %d %d %d %d\n", base->pulso, ombro->pulso, cotovelo->pulso, punho->pulso);
	free(P);
}

void learquivo(struct Mainwin_var *mainwin, int numpontos, char pontos[25][25], struct servo *ptrservo[5], int serial_fd)
{
	
	float Xuser = 0, Yuser = 0, Zuser = 0, PHIuser = 0;
	struct servo *base = ptrservo[0],
                *ombro = ptrservo[1],
                *cotovelo = ptrservo[2],
                *punho = ptrservo[3],
                *garra = ptrservo[4];
	FILE *fp;
	static char movebuffer[100];
	memset(movebuffer, 0, sizeof(char)*100);
	printf("Digite o nome do arquivo que contém os comandos:\n");
	if (fgets(buffer, sizeof(buffer)*150, stdin) == NULL)			
		printf("Erro ao ler nome\n");
	buffer[strlen(buffer)-1] = '\0';
	fp = fopen(buffer, "r");		
	while (fgets(buffer, sizeof(char)*150, fp)) {	
		switch (buffer[2]) { 
			case 'P':
				sscanf(buffer, "%*d)%s", buffer);
				printf("%s\n", buffer);
				if (strcmp(buffer,"PEGA;")==0){		
					printf("Detectei a palavra PEGA\n");
					change_servo(serial_fd, garra, 2400);
				}

			break;

			case 'S':
				sscanf(buffer, "%*d)%s", buffer);
				if (strcmp(buffer,"SOLTA;")==0){
					printf("Detectei a palavra SOLTA\n");
					change_servo(serial_fd, garra, 1300);
				}
			break;

			case 'R':
				sscanf(buffer, "%*d)%s", buffer);
				if (strcmp(buffer, "REPOUSO;")==0){
					printf("Detectei a palavra REPOUSO\n");
					memset(buffer, 0, sizeof(char)*150);

					base->pulso = trava(base, 1500);
					ombro->pulso = trava(ombro, 1500);
					cotovelo->pulso = trava(cotovelo, 1500);
					punho->pulso = trava(punho, 1500);

					base->ang = base->angoffset;
					ombro->ang = ombro->angoffset;
					cotovelo->ang = cotovelo->angoffset;
					punho->ang = punho->angoffset;

					sprintf(buffer, "#%dP%d#%dP%d#%dP%d#%dP%dT4000\r",
					base->num, base->pulso,
					ombro->num, ombro->pulso,
					cotovelo->num, cotovelo->pulso,
					punho->num, punho->pulso);

					enviar_comando(buffer, serial_fd);
				}
			break;

			case 'M':	
				sscanf(buffer, "%*d)%s(%*d,%*d,%*d,%*d);", movebuffer);
				printf("%s\n", movebuffer);
				if (strncmp(movebuffer, "MOVE", 4) == 0){
					printf("Detectei a palavra MOVE\n");
					sscanf(buffer, "%*d)MOVE(%f,%f,%f,%f);", &Xuser, &Yuser, &Zuser, &PHIuser);	
					apagagarra(mainwin, Xuser, Yuser);
					enviar_comandoX(mainwin->display);
					cinversa(serial_fd, ptrservo, Xuser, Yuser, Zuser, PHIuser);	
					drawEixoEscalaGarraPontos(mainwin, Xuser, Yuser, pontos, numpontos);
					enviar_comandoX(mainwin->display);
					usleep(4000000);
				}
			break;
		} 
		usleep(1500000);
	} 
	fclose(fp);
}

int evento_keypress(int serial_fd, struct Mainwin_var *mainwin, struct servo *ptrservo[5], float *CXYZ[3], KeySym *key_symbol, int *numpontos, char pontos[25][25])
{
	float *CX = CXYZ[0],
		*CY = CXYZ[1],
		*CZ = CXYZ[2];
	float Xuser = 0, Yuser = 0, Zuser = 0, PHIuser = 0;
	struct servo *base = ptrservo[0],
                *ombro = ptrservo[1],
                *cotovelo = ptrservo[2],
                *punho = ptrservo[3],
                *garra = ptrservo[4];

	apagagarra(mainwin, *CX, *CY);

	switch(*key_symbol)
	{
		case XK_D:                                             //aumenta e diminui o angulo da base
			change_servo(serial_fd, base, base->pulso+10);
			base->ang = calc_angdeg(base);
		break;
		case XK_L:
			change_servo(serial_fd, base, base->pulso-10);
			base->ang = calc_angdeg(base);
		break;


		case XK_S:                                              //aumenta e diminui o angulo do ombro
			change_servo(serial_fd, ombro, ombro->pulso+10);
			ombro->ang = calc_angdeg(ombro);
		break;
		case XK_K:
			change_servo(serial_fd, ombro, ombro->pulso-10);
			ombro->ang = calc_angdeg(ombro);
		break;


		case XK_A:                                              //aumenta e diminui o angulo do cotovelo
			change_servo(serial_fd, cotovelo, cotovelo->pulso+10);
			cotovelo->ang = calc_angdeg(cotovelo);
		break;
		case XK_J:
			change_servo(serial_fd, cotovelo, cotovelo->pulso-10);
			cotovelo->ang = calc_angdeg(cotovelo);
		break;


		case XK_W:                                              //aumenta e diminui o angulo do punho
			change_servo(serial_fd, punho, punho->pulso+10);
			punho->ang = calc_angdeg(punho);
		break;
		case XK_I:
			change_servo(serial_fd, punho, punho->pulso-10);
			punho->ang = calc_angdeg(punho);
		break;


		case XK_Page_Up:                                        //abre e fecha a garra
			change_servo(serial_fd, garra, garra->pulso+10);
		break;
		case XK_Page_Down:
			change_servo(serial_fd, garra, garra->pulso-10);
		break;


		case XK_space:						//desenha um ponto vermelho
			sprintf(pontos[*numpontos],"%.2f %.2f %.2f", *CX, *CY, *CZ);
			*numpontos = *numpontos+1;
		break;


		case XK_C:
			printf("Digite o comando:\n");
			if (fgets(buffer, sizeof(char)*150, stdin)!=NULL)
				printf("%s\n", buffer);
			if (strcmp(buffer, "coordenadas\n") == 0) {
				printf("Digite a coordenada X, Y, Z e o angulo PHI, em ordem:\n");
				if (fgets(buffer, sizeof(char)*150, stdin) != NULL)
					Xuser = atof(buffer);
				if (fgets(buffer, sizeof(char)*150, stdin) != NULL)
					Yuser = atof(buffer);
				if (fgets(buffer, sizeof(char)*150, stdin) != NULL)
					Zuser = atof(buffer);
				if (fgets(buffer,sizeof(buffer),stdin)!=NULL)
					PHIuser = atof(buffer);
				
				cinversa(serial_fd, ptrservo, Xuser, Yuser, Zuser, PHIuser);
			} else if (strcmp(buffer, "arquivo\n") == 0) {
				learquivo(mainwin, *numpontos, pontos, ptrservo, serial_fd);
			}       
		break;


		case XK_Escape:				//sai do programa
			return 0;
		break;


		default:                                //Ignora outras teclas
		break;
	}

	*CX = coor_x(base->ang, ombro->ang, cotovelo->ang, punho->ang);
	*CY = (-1)*coor_y(base->ang, ombro->ang, cotovelo->ang, punho->ang);
	*CZ = coor_z(base->ang, ombro->ang, cotovelo->ang, punho->ang);

	drawEixoEscalaGarraPontos(mainwin, *CX, *CY, pontos, *numpontos);

	enviar_comandoX(mainwin->display);
	
	printf("\n\nANG_BAS %f\nANG_OMB %f\nANG_COT %f\nANG_PUN %f\n",base->ang, ombro->ang, cotovelo->ang, punho->ang);
	printf("\nCOORD X: %f\n", *CX);
	printf("\nCOORD Y: %f\n", *CY);
	printf("\nCOORD Z: %f\n", *CZ);
	printf("\nCOORD PX: %d\n", coordX_pixels((*CY)*7, mainwin->larg));
	printf("\nCOORD PY:%d\n", coordY_pixels((*CX)*(-7), mainwin->altw));
	return 1;
}

#endif
