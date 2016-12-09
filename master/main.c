#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <math.h>
#include <string.h>
#include "./bibliotecas/servo.h"
#include "./bibliotecas/driver.h"
#include "./bibliotecas/increasedecrease.h"
#include "./bibliotecas/calculos_angulos.h"
#include "./bibliotecas/cinematica.h"
#include "./bibliotecas/textos.h"
#include "./bibliotecas/eventfunctions.h"
#include "./bibliotecas/graphical.h"
#include "./bibliotecas/calibracao.h"
#include "./bibliotecas/transf_perspectiva.h"
#include "./bibliotecas/AI.h"


#define alt 6.731
#define L1 14.605
#define L2 18.7325
#define L3 10.0


int Inicializar_Portas()
{
	int serial_fd;
	if ((serial_fd = abrir_porta()) == -1){
		printf("A porta nao foi aberta\n");
		return -1;
	}
	printf("Porta aberta\n");
	if (configurar_porta(serial_fd) == -1){
		printf("A porta nao foi configurada\n");
		close(serial_fd); 
		return -1;
	}
	printf("Porta configurada\n");
	return serial_fd;
}

int main()
{
	buffer = malloc(sizeof(char)*150);

	struct CD ponto_teste;
	gsl_matrix *Homografia = gsl_matrix_alloc(3,3);	
	calibra(Homografia);
	ponto_teste = pixel_to_world (400, 200, Homografia);

	struct servo base, ombro, cotovelo, punho, garra;			//define servos
	struct servo *ptrservo[5] = {&base, &ombro, &cotovelo, &punho, &garra};
	define_servos(ptrservo);

	int serial_fd = -1;
	
	char pontos[25][25];
	int numpontos = 0, i = 0;
	
	float CX = coor_x(base.ang,ombro.ang,cotovelo.ang,punho.ang);
	float CY = (-1)*coor_y(base.ang,ombro.ang,cotovelo.ang,punho.ang);
	float CZ = coor_z(base.ang,ombro.ang,cotovelo.ang,punho.ang);
	float *CXYZ[3] = {&CX, &CY, &CZ};

/*
	if ((serial_fd = Inicializar_Portas()) == -1) return 1; // Chama a função que abre e configura as portas
	memset(buffer, 0 , sizeof(char)*150);
	sprintf(buffer, "#0P1500#1P1500#2P1500#3P1500#4P1500T3000\r");
	if(enviar_comando(buffer, serial_fd) == -1){
		printf("Problema no posicionamento inicial\n");
		return 1;
	}
*/


/*
 * start of X-related variables definitions using define_xthings() and mainwin mapping
 */ 
	Display *display;

	int screen_num, coordx_mainwin, coordy_mainwin;  	
	int *intptr[3] = {&screen_num, &coordx_mainwin, &coordy_mainwin};

	Window rootwin, mainwin;
	Window *winptr[2] = {&rootwin, &mainwin};
	
	unsigned long blackpix, whitepix, col_bord_mainwin, col_backg_mainwin;
	unsigned long *ulongptr[4] = {&blackpix, &whitepix, &col_bord_mainwin, &col_backg_mainwin};
	
	unsigned int alt_mainwin, larg_mainwin, largbord_mainwin;
	unsigned int *uintptr[3] = {&alt_mainwin, &larg_mainwin, &largbord_mainwin};

	GC gc_branco, gc_preto, gc_red, gc_team1, gc_team2;
	GC *gcptr[5] = {&gc_branco, &gc_preto, &gc_red, &gc_team1, &gc_team2};
	
	struct Mainwin_var mainwin_var;
	
	if (define_xthings(&display, intptr, winptr, ulongptr, uintptr, gcptr, &mainwin_var) == -1){
		printf("Nao foi possivel abrir o display");
		return -1;
	}
/*
 * end of X-related variables definitions using define_xthings() and mainwin mapping
 */
	XEvent evento;
	KeySym key_symbol;
	int loopvar = 1;

	initial_draw(&mainwin_var, CX, CY);

	while(loopvar){		
		XNextEvent(display, &evento);
		switch(evento.type){
			case Expose:
				drawEixoEscalaGarraPontos(&mainwin_var, CX, CY, pontos, numpontos);
			break;

			case KeyPress:
				key_symbol = XkbKeycodeToKeysym(display, evento.xkey.keycode, 0, 1);
				loopvar = evento_keypress(serial_fd, &mainwin_var, ptrservo, CXYZ, &key_symbol, &numpontos, pontos);
			break;
			
			default:
			break;
		}
	}	

	FILE *fp;
	memset(buffer, 0, sizeof(char)*150);
	printf("Digite o nome do arquivo para salvar as coordenadas:\n");
	if (fgets(buffer, sizeof(char)*150, stdin) == NULL) 
		printf("Erro ao ler nome\n");
	buffer[strlen(buffer)-1]='\0';			
	fp = fopen(strcat(buffer,".txt\0"), "w");		
	for (i = 0; i < numpontos; i++) 
		fprintf(fp, "%s\n", pontos[i]);
	
	/*
	int x, k;
	for (x = 0; x < 3; x++) {
		printf("\n");
		for (k = 0; k < 3; k++) {
		printf("\t%g\t", gsl_matrix_get(Homografia, x, k));
		}
	}
	*/

	printf("Ponto teste -> X: %f Y: %f\n", ponto_teste.X, ponto_teste.Y);
	printf("Chamando IA\n");
	

	XFreeGC(display, *mainwin_var.gc_preto);
	XFreeGC(display, *mainwin_var.gc_branco);
	XFreeGC(display, *mainwin_var.gc_vermelho);
	XFreeGC(display, *mainwin_var.gc_team1);
	XFreeGC(display, *mainwin_var.gc_team2);

	fclose(fp);
	fechar_porta(serial_fd);
	XCloseDisplay(display);
	free(buffer);
	gsl_matrix_free(Homografia);
	return 0;
}

