#include <X11/Xlib.h>
#include "./textos.h"

#ifndef graphicaldefs
#define graphicaldefs

struct Mainwin_var
{
	Display *display;
	Window *mainwin;
	GC *gc_preto, *gc_branco, *gc_vermelho, *gc_team1, *gc_team2;
	unsigned int altw, larg;
	int screen_num;
};

void enviar_comandoX(Display *disp)
{
	XSync(disp,False);
	usleep(10000);
}

int coordX_pixels(float X, unsigned int larg_mainwin){return (X + (larg_mainwin/2));}
int coordY_pixels(float Y, unsigned int alt_mainwin){return ((alt_mainwin/2)-Y);}

void desenhar_eixos(Display *display, Window *mainwin, unsigned int larg_mainwin, unsigned int alt_mainwin, GC *gc_main)
{
        XDrawLine(display, *mainwin, *gc_main, 0, alt_mainwin/2, larg_mainwin, alt_mainwin/2);
        XDrawLine(display, *mainwin, *gc_main, larg_mainwin/2, 0, larg_mainwin/2, alt_mainwin);
}

void desenha_escala(Display *disp, int tela, Window *janela, unsigned int altjanela, unsigned int largjanela)
{
	int i,j;
	char texto[100];
	for (i = 5; i <= 40; i=i+5){
		j = coordY_pixels((-7.0)*i,altjanela);
		sprintf(texto,"%d",i);
		desenha_texto(disp, tela, janela, texto, 0, 0, 0, coordX_pixels(0, largjanela)-8, j);
	}
	for (i = 40; i >= -40; i=i-5){
		if(i!=0){
			j = coordX_pixels((7.0)*i,largjanela);
			sprintf(texto,"%d",i);
			desenha_texto(disp, tela, janela, texto, 0, 0, 0, j, coordY_pixels(0, altjanela)+8);
		}
	}
}

void apagagarra(struct Mainwin_var *mainwin, float CX, float CY)
{
	XDrawArc(mainwin->display, *(mainwin->mainwin), *(mainwin->gc_branco),
        	coordX_pixels((CY)*7, mainwin->larg)-5,
                coordY_pixels((-7)*(CX), mainwin->altw)-5,
                10, 10 , 0, 360*64);
}


void drawEixoEscalaGarraPontos(struct Mainwin_var *main_win, float CX, float CY, char pontos[25][25], int numpontos)
{
        int i;
        float X, Y;
        desenhar_eixos(main_win->display, main_win->mainwin, main_win->larg, main_win->altw, main_win->gc_preto);
        desenha_escala(main_win->display, main_win->screen_num, main_win->mainwin, main_win->altw, main_win->larg);
        XDrawArc(main_win->display, *(main_win->mainwin), *(main_win->gc_preto), 
                coordX_pixels(CY*7, main_win->larg)-5, 
                coordY_pixels((-7)*CX, main_win->altw)-5, 
                10, 10, 0, 360*64);

        for(i = 0; i < numpontos; i++)
        {       
                sscanf(pontos[i],"%f %f", &X, &Y);                        
                XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_vermelho), 
                        coordX_pixels(Y*7, main_win->larg)-4, 
                        coordY_pixels((-7)*X, main_win->altw)-4,
                        8, 8, 0, 360*64);
        }
}


int define_xthings(Display **displayptr, int *intptr[3], Window *winptr[2], unsigned long *ulongptr[4],
		unsigned int *uintptr[3], GC *gcptr[5], struct Mainwin_var *mainwin_var)
{
        Display **display = displayptr;

        int *screen_num = intptr[0], 
		*coordx_mainwin = intptr[1], 
		*coordy_mainwin = intptr[2];

        Window *rootwin = winptr[0],
		*mainwin = winptr[1];

        unsigned long *blackpix = ulongptr[0], 
		*whitepix = ulongptr[1], 
		*col_bord_mainwin = ulongptr[2], 
		*col_backg_mainwin = ulongptr[3];
        
        unsigned int *alt_mainwin = uintptr[0], 
		*larg_mainwin = uintptr[1], 
		*largbord_mainwin = uintptr[2];

        GC *gc_branco = gcptr[0], 
		*gc_preto = gcptr[1], 
		*gc_vermelho = gcptr[2],
		*gc_time1 = gcptr[3],
		*gc_time2 = gcptr[4];


        if ((*display = XOpenDisplay(NULL)) == NULL){
                printf("O display nao foi aberto\n");
        	return -1;
	}

        *screen_num = DefaultScreen(*display); 
       
	*rootwin = RootWindow(*display, *screen_num); 
        
        *blackpix = BlackPixel(*display, *screen_num);
	
	*whitepix = WhitePixel(*display, *screen_num);
        
	*alt_mainwin= 600; 
	*larg_mainwin = 1000; 
	*largbord_mainwin = 1;
	*col_bord_mainwin = *blackpix;
	*col_backg_mainwin = *whitepix;	
	*coordx_mainwin = 0;
	*coordy_mainwin = 0;

	*mainwin = XCreateSimpleWindow(*display, *rootwin,
					*coordx_mainwin, *coordy_mainwin,
					*larg_mainwin, *alt_mainwin,
					*largbord_mainwin, *col_bord_mainwin,
					*col_backg_mainwin);
	XMapWindow(*display, *mainwin);

	XGCValues values;
        unsigned long valuesmask= 0;

	*gc_preto = XCreateGC(*display, *mainwin, valuesmask, &values);
	XSetForeground(*display, *gc_preto, *blackpix);
	XSetBackground(*display, *gc_preto, *blackpix);

	*gc_branco = XCreateGC(*display, *mainwin, valuesmask, &values);
	XSetForeground(*display, *gc_branco, *whitepix);
	XSetBackground(*display, *gc_branco, *whitepix);
	
	*gc_vermelho = XCreateGC(*display, *mainwin, valuesmask, &values);
	Colormap colormaptela = XDefaultColormap(*display, *screen_num);
	XColor cor;
	cor.red = rgb_to_xrgb(255);
	cor.green = rgb_to_xrgb(0);
	cor.blue = rgb_to_xrgb(0);
	XAllocColor(*display, colormaptela, &cor);
	XSetBackground(*display, *gc_vermelho, cor.pixel);
	XSetForeground(*display, *gc_vermelho, cor.pixel);


	*gc_time1 = XCreateGC(*display, *mainwin, valuesmask, &values);
	colormaptela = XDefaultColormap(*display, *screen_num);
	cor.red = rgb_to_xrgb(176);
	cor.green = rgb_to_xrgb(216);
	cor.blue = rgb_to_xrgb(145);
	XAllocColor(*display, colormaptela, &cor);
	XSetBackground(*display, *gc_time1, cor.pixel);
	XSetForeground(*display, *gc_time1, cor.pixel);

	*gc_time2 = XCreateGC(*display, *mainwin, valuesmask, &values);
	colormaptela = XDefaultColormap(*display, *screen_num);
	cor.red = rgb_to_xrgb(204);
	cor.green = rgb_to_xrgb(126);
	cor.blue = rgb_to_xrgb(139);
	XAllocColor(*display, colormaptela, &cor);
	XSetBackground(*display, *gc_time2, cor.pixel);
	XSetForeground(*display, *gc_time2, cor.pixel);

	mainwin_var->display = *display;
	mainwin_var->mainwin = mainwin;
	mainwin_var->gc_preto = gc_preto;
	mainwin_var->gc_branco = gc_branco;
	mainwin_var->gc_vermelho = gc_vermelho;
	mainwin_var->gc_team1 = gc_time1;
	mainwin_var->gc_team2 = gc_time2;
	mainwin_var->altw = *alt_mainwin;
	mainwin_var->larg = *larg_mainwin;
	mainwin_var->screen_num = *screen_num;
	return 0;
}

void initial_draw(struct Mainwin_var *main_var, float CX, float CY)
{
	XSelectInput(main_var->display, *(main_var->mainwin), ExposureMask | KeyPressMask | KeyReleaseMask); // Escolhe que tipos de eventos receber de mainwin
        
	XDrawArc(main_var->display, *(main_var->mainwin), *(main_var->gc_preto), 
		coordX_pixels(CY*7, main_var->larg)-5, 
		coordY_pixels((-7)*CX, main_var->altw)-5,
		10, 10 , 0, 360*64);	// Desenha a posicao inicial da garra
        
	desenhar_eixos(main_var->display, main_var->mainwin, main_var->larg, main_var->altw, main_var->gc_preto);
        desenha_escala(main_var->display, main_var->screen_num, main_var->mainwin, main_var->altw, main_var->larg);
        enviar_comandoX(main_var->display);
}

#endif
