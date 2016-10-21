#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include "./bibliotecas/servo.h"
#include "./bibliotecas/hack.h"
#include "./bibliotecas/increasedecrease.h"
#include "./bibliotecas/calculos_angulos.h"
#include "./bibliotecas/cinematica.h"
#include "./bibliotecas/textos.h"
#include "./bibliotecas/GC.h"
#include <math.h>
#include <string.h>

#define HOME_POS "#0P1500#1P1500#2P1500#3P1500#4P1500T2000\r"

#define alt 6.731
#define L1 14.605
#define L2 18.7325
#define L3 10.0

/*Converte X e Y, reais (em centímetros), em um número de pixels na tela*/

int coord_real_X_to_pixels(float X, unsigned int larg_main_win){return (X + (larg_main_win/2));}

int coord_real_Y_to_pixels(float Y, unsigned int alt_main_win){return ((alt_main_win/2)-Y);}

void desenha_escala(Display *disp, int tela, Window janela, unsigned int altjanela, unsigned int largjanela)
{
	int i,j;
	char texto[100];
	for (i = 5; i <= 40; i=i+5){
		j = coord_real_Y_to_pixels((-7.0)*i,altjanela);
		sprintf(texto,"%d",i);
		desenha_texto(disp, tela, janela, texto, 0, 0, 0,coord_real_X_to_pixels(0,largjanela)-8,j);
	}
	for (i = 40; i >= -40; i=i-5){
		if(i!=0){
			j = coord_real_X_to_pixels((7.0)*i,largjanela);
			sprintf(texto,"%d",i);
			desenha_texto(disp, tela, janela, texto, 0, 0, 0,j, coord_real_Y_to_pixels(0,altjanela)+8);
		}
	}
}

int Inicializar_Portas()		//Abre e configura as portas
{
	int serial_fd;

	printf("Programa iniciado\n");

	if ( (serial_fd = abrir_porta() ) == -1)
	{
		printf("A porta nao foi aberta\n");
		return -1;
	}

	printf("Porta aberta\n");

	if ( configurar_porta(serial_fd) == -1)
	{
		printf("A porta nao foi configurada\n");
		close(serial_fd); 
		return -1;
	}

	printf("Porta configurada\n");

	return serial_fd;
}

void enviar_comandoX(Display *disp)	// dá um flush e espera um pouco
{
	XSync(disp,False);
	usleep(10000);
}


/*Função para desenhar os eixos horizontal e vertical, respectivamente*/
void desenhar_eixos(Display *display, Window main_win, unsigned int larg_main_win, unsigned int alt_main_win, GC gc_main)
{
	XDrawLine(display, main_win, gc_main, 0, alt_main_win/2, larg_main_win, alt_main_win/2);
	XDrawLine(display, main_win, gc_main, larg_main_win/2, 0, larg_main_win/2, alt_main_win);
}

int main()
{
	int serial_fd = -1;
	define_servos();
	calc_all_ang();

	if ( (serial_fd=Inicializar_Portas()) == -1) return 1; // Chama a função que abre e configura as portas
	

	char *posinicial = malloc(sizeof(char)*100);
	sprintf(posinicial,"%s",HOME_POS);
	if(enviar_comando(posinicial,serial_fd)==-1)
	{
		printf("Problema no posicionamento inicial\n");
		return 1;
	}
	memset(posinicial, 0, 100);
	free (posinicial);
	

	char pontos[25][25];
	int numpontos = 0, i = 0;

	float Xuser=0,Yuser=0,Zuser=0,PHIuser=0;	//valores fornecidos pelo usuário para fazer a cinemática inversa
	
	float cxpontos=0,cypontos=0;	//valores pra desenhar os pontos ao apertar espaço



	float CX = coor_x(base.ang,ombro.ang,cotovelo.ang,punho.ang);	// coordenadas X e Y da ponta da garra
	float CY = (-1)*coor_y(base.ang,ombro.ang,cotovelo.ang,punho.ang);
	float CZ = coor_z(base.ang,ombro.ang,cotovelo.ang,punho.ang);

	Display *display;				// Abre o display
	if ( (display = XOpenDisplay(NULL)) == NULL)
	{
		printf("O display nao foi aberto\n");
		return 1;
	}
	int screen_num = DefaultScreen(display);        // Pega a ID da screen
	Window root_win = RootWindow(display,screen_num); // Abre a janela root
	
	/* black e white armazenam as cores preto e branco da janela*/
	unsigned long black = BlackPixel(display,screen_num), white = WhitePixel(display,screen_num);

	/*Definição da altura e da largura da janela, assim como a espessura da borda*/
	unsigned int alt_main_win= 600, larg_main_win = 1000, larg_bord_main_win = 1;

	/*Define a cor da borda e do fundo da janela principal*/
	unsigned long col_bord_main_win = black, backg_main_win = white;	

	/*Coordenadas da extremidade superior esquerda da janela principal*/
	int coord_x_main_win = 0, coord_y_main_win = 0;	
	
	/*Criação e configuração da janela principal do programa*/
	Window main_win;					
	main_win = XCreateSimpleWindow(display,root_win,
					coord_x_main_win, coord_y_main_win,
					larg_main_win, alt_main_win,
					larg_bord_main_win, col_bord_main_win,
					backg_main_win);
	desenha_escala(display, screen_num, main_win, alt_main_win, larg_main_win);

	XMapWindow(display,main_win);		//desenha a janela principal
	enviar_comandoX(display);
	
	XEvent evento;
	KeySym key_symbol;
	XSelectInput(display,main_win,ExposureMask | KeyPressMask | KeyReleaseMask);	//recebe eventos do tipo exposure, keypress e keyrelease
	int loopvar = 1;
	XGCValues values;
	unsigned long valuemask = 0;

	/*Configuração dos GCs para pintar a apagar na janela do programa */
	GC gc_branco, gc_preto, gc_red;
	gc_preto = gc_pintar(display, main_win, valuemask, &values);
	gc_branco = gc_apagar(display, main_win, valuemask, &values);
	gc_red = gc_vermelho(display, main_win, valuemask, &values);

	XDrawArc(display,main_win,gc_preto,coord_real_X_to_pixels(CY*7,larg_main_win)-5,coord_real_Y_to_pixels((-7)*CX,alt_main_win)-5,10,10,0,360*64);
	desenhar_eixos(display, main_win, larg_main_win, alt_main_win, gc_preto);
	desenha_escala(display, screen_num, main_win, alt_main_win, larg_main_win);
	enviar_comandoX(display);

	char comando[20];
	float *P;
	while(loopvar)		//loop principal do programa que reage a eventos
	{
		XNextEvent(display, &evento);	//coloca os eventos recebidos pelo X na variavel evento
		switch(evento.type)
		{
			case Expose:
				desenhar_eixos(display, main_win, larg_main_win, alt_main_win, gc_preto);
				desenha_escala(display, screen_num, main_win, alt_main_win, larg_main_win);
				XDrawArc(display,main_win,gc_preto, coord_real_X_to_pixels(CY*7,larg_main_win)-5,coord_real_Y_to_pixels((-7)*CX,alt_main_win)-5,10,10,0,360*64);
				
				for(i = 0; i < numpontos; i++)
				{	
					sscanf(pontos[i],"%f %f", &cxpontos, &cypontos);			
					XFillArc(display,main_win,gc_red, coord_real_X_to_pixels(cypontos*7,larg_main_win)-4,coord_real_Y_to_pixels((-7)*cxpontos,alt_main_win)-4,8,8,0,360*64);
				}
				enviar_comandoX(display);
			break;

			case KeyPress:
				key_symbol = XkbKeycodeToKeysym (display, evento.xkey.keycode, 0, 1);
				
				XDrawArc(display,main_win,gc_branco,coord_real_X_to_pixels(CY*7,larg_main_win)-5,
				coord_real_Y_to_pixels((-7)*CX,alt_main_win)-5,10,10,0,360*64);		// apaga uma esfera de raio 5 onde na ultima posição da garra
				
				switch(key_symbol)
				{
					case XK_D:					//aumenta e diminui o angulo da base
						change_servo(serial_fd,&base,base.pulso+10);
						base.ang = calc_ang_deg(&base);
					break;
					case XK_L:
						change_servo(serial_fd,&base,base.pulso-10);
						base.ang = calc_ang_deg(&base);
					break;

					
					case XK_S:						//aumenta e diminui o angulo do ombro
						change_servo(serial_fd,&ombro,ombro.pulso+10);
						ombro.ang = calc_ang_deg(&ombro);
					break;
					case XK_K:
						change_servo(serial_fd,&ombro,ombro.pulso-10);
						ombro.ang = calc_ang_deg(&ombro);
					break;

					
					case XK_A:						//aumenta e diminui o angulo do cotovelo
						change_servo(serial_fd,&cotovelo,cotovelo.pulso+10);
						cotovelo.ang = calc_ang_deg(&cotovelo);
					break;
					case XK_J:
						change_servo(serial_fd,&cotovelo,cotovelo.pulso-10);
						cotovelo.ang = calc_ang_deg(&cotovelo);
					break;

					 
					case XK_W:						//aumenta e diminui o angulo do punho
						change_servo(serial_fd,&punho,punho.pulso+10);
						punho.ang = calc_ang_deg(&punho);
					break;
					case XK_I:
						change_servo(serial_fd,&punho,punho.pulso-10);
						punho.ang = calc_ang_deg(&punho);
					break;


					case XK_Page_Up:					//abre e fecha a garra
						change_servo(serial_fd,&garra,garra.pulso+10);
					break;
					case XK_Page_Down:
						change_servo(serial_fd,&garra,garra.pulso-10);
					break;


					case XK_space:
						sprintf(pontos[numpontos],"%.2f %.2f %.2f",CX,CY,CZ);
						numpontos=numpontos+1;
					break;


					case XK_C:
						printf("Digite o comando:\n");
						if (fgets(comando,sizeof(comando),stdin)!=NULL)
							printf("\n%s\n",comando);
						if( (strcmp(comando,"coordenadas\n")==0) )
						{
							printf("Digite a coordenada X:\n");
							if (fgets(comando,sizeof(comando),stdin)!=NULL)
								Xuser = atof(comando);
						
							printf("Digite a coordenada Y:\n");
							if (fgets(comando,sizeof(comando),stdin)!=NULL)
								Yuser = atof(comando);
						
							printf("Digite a coordenada Z:\n");
							if (fgets(comando,sizeof(comando),stdin)!=NULL)
								Zuser = atof(comando);


							printf("Digite o angulo PHI:\n");
							if (fgets(comando,sizeof(comando),stdin)!=NULL)
								PHIuser = deg_to_rad(atof(comando));
							

							printf("%f %f %f %f\n",Xuser,Yuser,Zuser,PHIuser);
							P = ptr_angles(Xuser,Yuser,Zuser,PHIuser);
							printf("Angulos: %f %f %f %f\n", P[0], P[1], P[2], P[3]);
						
							/*
							change_servo(serial_fd,&base,calc_ang_pul(rad_to_deg(P[0]),ang_max_base,BAS_SERVO));
							change_servo(serial_fd,SHL_SERVO,&ang_omb,calc_ang_pul(rad_to_deg(P[1]),ang_max_omb,SHL_SERVO));
							change_servo(serial_fd,ELB_SERVO,&ang_cot,calc_ang_pul(rad_to_deg(P[2]),ang_max_cot,ELB_SERVO));
							change_servo(serial_fd,WRI_SERVO,&ang_pun,calc_ang_pul(rad_to_deg(P[3]),ang_max_pun,WRI_SERVO));

							ang_base_deg = calc_ang_deg(ang_base,ang_max_base,BAS_SERVO);
							ang_omb_deg = calc_ang_deg(ang_omb,ang_max_omb,SHL_SERVO);
							ang_cot_deg = calc_ang_deg(ang_cot,ang_max_cot,ELB_SERVO);
							ang_pun_deg = calc_ang_deg(ang_pun,ang_max_pun,WRI_SERVO);
							*/

							free(P);
						}

					break;


					case XK_Escape:
						loopvar = 0;
					break;
					
						
					default:				//Ignora outras teclas
					break;
				
				}

				CX = coor_x(base.ang,ombro.ang,cotovelo.ang,punho.ang);	// coordenadas X e Y da ponta da garra
				CY = (-1)*coor_y(base.ang,ombro.ang,cotovelo.ang,punho.ang);
				CZ = coor_z(base.ang,ombro.ang,cotovelo.ang,punho.ang);
				printf("\n\nANG_BAS %f\nANG_OMB %f\nANG_COT %f\nANG_PUN %f\n",base.ang,ombro.ang,cotovelo.ang,punho.ang);
				printf("\nCOORD X: %f\n",CX);
				printf("\nCOORD Y: %f\n",CY);
				printf("\nCOORD Z: %f\n",CZ);
				printf("\nCOORD PX: %d\n",coord_real_X_to_pixels(CY*7,larg_main_win));
				printf("\nCOORD PY:%d\n",coord_real_Y_to_pixels(CX*(-7),alt_main_win));

				XDrawArc(display,main_win,gc_preto, coord_real_X_to_pixels(CY*7,larg_main_win)-5,coord_real_Y_to_pixels((-7)*CX,alt_main_win)-5,10,10,0,360*64);
				
				desenha_escala(display, screen_num, main_win, alt_main_win, larg_main_win);
				desenhar_eixos(display, main_win, larg_main_win, alt_main_win, gc_preto);
				
				for(i = 0; i < numpontos; i++)
				{	
					sscanf(pontos[i],"%f %f", &cxpontos, &cypontos);			
					XFillArc(display,main_win,gc_red, coord_real_X_to_pixels(cypontos*7,larg_main_win)-4,coord_real_Y_to_pixels((-7)*cxpontos,alt_main_win)-4,8,8,0,360*64);
				}


				enviar_comandoX(display);
			break;

			default:

			break;
		}
}	

	FILE *fp;
	fp = fopen("coordenadas.txt","w");
	for (i = 0; i < numpontos; i++)
	{
		printf("%s\n",pontos[i]);
		fprintf(fp,"%s\n",pontos[i]);
	}
	fclose(fp);
	
	fechar_porta(serial_fd);
	XCloseDisplay(display);
	return 0;
}

