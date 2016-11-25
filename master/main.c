#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include "./bibliotecas/servo.h"
#include "./bibliotecas/driver.h"
#include "./bibliotecas/increasedecrease.h"
#include "./bibliotecas/calculos_angulos.h"
#include "./bibliotecas/cinematica.h"
#include "./bibliotecas/textos.h"
#include "./bibliotecas/GC.h"
#include <math.h>
#include <string.h>

#define HOME_POS "#0P1500#1P1500#2P1500#3P1500#4P1500T2000\r"
#define LINESIZE 30

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

	if ( (serial_fd = abrir_porta() ) == -1){
		printf("A porta nao foi aberta\n");
		return -1;
	}

	printf("Porta aberta\n");

	if ( configurar_porta(serial_fd) == -1){
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
	
	// calc_all_ang();
	// Essa função não faz nada!
	
/*
	if ( (serial_fd=Inicializar_Portas()) == -1) return 1; // Chama a função que abre e configura as portas
	

	char *posinicial = malloc(sizeof(char)*100);
	sprintf(posinicial,"%s",HOME_POS);
	if(enviar_comando(posinicial,serial_fd)==-1){
		printf("Problema no posicionamento inicial\n");
		return 1;
	}
	memset(posinicial, 0, 100);
	free (posinicial);
*/	

	char pontos[25][25];
	int numpontos = 0, i = 0;

	float Xuser=0,Yuser=0,Zuser=0,PHIuser=0;	//valores fornecidos pelo usuário para fazer a cinemática inversa
	
	float cxpontos=0,cypontos=0;	//valores pra desenhar os pontos ao apertar espaço



	float CX = coor_x(base.ang,ombro.ang,cotovelo.ang,punho.ang);	// coordenadas X e Y da ponta da garra
	float CY = (-1)*coor_y(base.ang,ombro.ang,cotovelo.ang,punho.ang);
	float CZ = coor_z(base.ang,ombro.ang,cotovelo.ang,punho.ang);

	Display *display;				// Abre o display
	if ((display = XOpenDisplay(NULL)) == NULL){
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

	char buffer[20];
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
						if (fgets(buffer,sizeof(buffer),stdin)!=NULL)
							printf("\n%s\n",buffer);
						if( (strcmp(buffer,"coordenadas\n")==0) )
						{
							printf("Digite a coordenada X:\n");
							if (fgets(buffer,sizeof(buffer),stdin)!=NULL)
								Xuser = atof(buffer);
						
							printf("Digite a coordenada Y:\n");
							if (fgets(buffer,sizeof(buffer),stdin)!=NULL)
								Yuser = atof(buffer);
						
							printf("Digite a coordenada Z:\n");
							if (fgets(buffer,sizeof(buffer),stdin)!=NULL)
								Zuser = atof(buffer);


							printf("Digite o angulo PHI:\n");
							if (fgets(buffer,sizeof(buffer),stdin)!=NULL)
								PHIuser = deg_to_rad(atof(buffer));
							OBFUSCACAO:

							printf("%f %f %f %f\n",Xuser,Yuser,Zuser,PHIuser);
							P = ptr_angles(Xuser,(-1)*Yuser,Zuser,PHIuser);
							printf("Angulos sem offset: %f %f %f %f\n", P[0], P[1], P[2], P[3]);
							
							if(P[0] != 1000){
								base.ang = rad_to_deg(P[0]);
								ombro.ang = rad_to_deg(P[1]);
								cotovelo.ang = rad_to_deg(P[2]);
								punho.ang = rad_to_deg(P[3]);


								char *comando = malloc(sizeof(char)*150);
								memset(comando,0,sizeof(char)*150);

								base.pulso = trava(&base, calc_ang_pul(&base));
								ombro.pulso = trava(&ombro, calc_ang_pul(&ombro));
								cotovelo.pulso = trava(&cotovelo, calc_ang_pul(&cotovelo));
								punho.pulso = trava(&punho, calc_ang_pul(&punho));

								sprintf(comando,"#%dP%d#%dP%d#%dP%d#%dP%dT4000\r",
								base.num, base.pulso,
								ombro.num, ombro.pulso,
								cotovelo.num, cotovelo.pulso,
								punho.num, punho.pulso);

								//printf("%s\n",comando);
								
								enviar_comando(comando,serial_fd);
								free(comando);



								printf("posicao valida\n");
							} else {
								printf("Posição invalida!\n");
							}

							printf("Servos: %f %f %f %f\n", base.ang, ombro.ang, cotovelo.ang, punho.ang);
							printf("Servos: %d %d %d %d\n", base.pulso, ombro.pulso, cotovelo.pulso, punho.pulso);

							free(P);
						} else if( (strcmp(buffer,"arquivo\n")==0) ) {
							FILE *fp;
        						char comando_arquivo[LINESIZE], str[12], nome_arquivo[100];
        						printf("Digite o nome do arquivo que contém os comandos:\n");
						        if (fgets(nome_arquivo,sizeof(nome_arquivo),stdin)==NULL)
					                printf("Erro ao ler nome\n");
						        nome_arquivo[strlen(nome_arquivo)-1]='\0';      //tira a newline do fgets
						        fp = fopen(nome_arquivo,"r");  //cria um arquivo com o nome inserido+.txt
						        while (fgets (comando_arquivo,LINESIZE,fp) ){
							printf("%c",comando_arquivo[2]);
							/* Testa a primeira letra de cada linha para facilitar a comparação. Em seguida verifica se a palavra realmente é um comando, e não SABÃO, por exemplo. */

							switch(comando_arquivo[2]){
							
							case 'P':
								sscanf(comando_arquivo,"%*d)%s", str);
								printf("%s\n",str);
								if (strcmp(str,"PEGA;")==0){
								printf("Detectei a palavra PEGA\n");
								change_servo(serial_fd,&garra,2400);
								}
			
							break;
							
							case 'S':
								sscanf(comando_arquivo,"%*d)%s", str);							
								if (strcmp(str,"SOLTA;")==0){
								change_servo(serial_fd,&garra,1300);
								}
							break;

							case 'R':
								sscanf(comando_arquivo,"%*d)%s", str);
                                                                if (strcmp(str,"REPOUSO;")==0){
									/* Envia posição de repouso para todos os servos */
						       
									char *comando = malloc(sizeof(char)*150);
									memset(comando,0,sizeof(char)*150);

									base.pulso = trava(&base, 1500);
									ombro.pulso = trava(&ombro, 1500);
									cotovelo.pulso = trava(&cotovelo, 1500);
									punho.pulso = trava(&punho, 1500);

									base.ang = base.angoffset;
									ombro.ang = ombro.angoffset;
									cotovelo.ang = cotovelo.angoffset;
									punho.ang = punho.angoffset;

									sprintf(comando,"#%dP%d#%dP%d#%dP%d#%dP%dT4000\r",
									base.num, base.pulso,
									ombro.num, ombro.pulso,
									cotovelo.num, cotovelo.pulso,
									punho.num, punho.pulso);

									//printf("%s\n",comando);

									enviar_comando(comando,serial_fd);
									free(comando);
								}
					              	break;	
							
							case 'M':
								sscanf(comando_arquivo,"%*d)%s(%*d,%*d,%*d,%*d);", str);												printf("%s\n",str);
								if (strncmp(str,"MOVE",4)==0){
									printf("Detectei a palavra MOVE\n");
									sscanf(comando_arquivo,"%*d)MOVE(%f,%f,%f,%f);", &Xuser, &Yuser, &Zuser, &PHIuser);
									PHIuser = deg_to_rad(PHIuser);
									goto OBFUSCACAO;							
								}
							break;
							} //fecha o swith local
							usleep(1500000);
						} //fecha o while
						       
							 fclose(fp);
							
					}	 //fecha o else if

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

	printf("base: %f %f\n", base.angmin, base.angmax);
	printf("ombro: %f %f\n", ombro.angmin, ombro.angmax);
	printf("cotovelo: %f %f\n", cotovelo.angmin, cotovelo.angmax);
	printf("punho: %f %f\n", punho.angmin, punho.angmax);

	FILE *fp;
	char nome[100];
	printf("Digite o nome do arquivo para salvar as coordenadas:\n");
	if (fgets(nome,sizeof(nome),stdin)==NULL)
		printf("Erro ao ler nome\n");
	nome[strlen(nome)-1]='\0';	//tira a newline do fgets
	fp = fopen(strcat(nome,".txt\0"),"w");	//cria um arquivo com o nome inserido+.txt
	for (i = 0; i < numpontos; i++){	//escreve todos os pontos
		printf("%s\n",pontos[i]);	
		fprintf(fp,"%s\n",pontos[i]);
	}
	fclose(fp);
	
	fechar_porta(serial_fd);
	XCloseDisplay(display);
	return 0;
}

