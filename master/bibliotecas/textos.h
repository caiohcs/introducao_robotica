#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <string.h>
void enviar_comandoX(Display *disp);

int rgb_to_xrgb(int cor) {return cor*(65535/255);}

XFontStruct *carregar_fonte(Display *disp)
{

	int i = 0;
	int num_fontes_carregadas;
	int max_fontes = 100;
	char **nomefontes = XListFonts(disp, "*", max_fontes, &num_fontes_carregadas);

	XFontStruct* fonte = malloc(sizeof(XFontStruct));	// lembrar de desalocar
	fonte = NULL;
	
 
	while((fonte==NULL) && (i < num_fontes_carregadas)){
		fonte = XLoadQueryFont(disp,nomefontes[i]);
		i=i+1;
	}

	
//	fonte = XLoadQueryFont(disp,"-misc-fixed-bold-r-semicondensed--13-120-75-75-c-60-iso8859-15");

	if(fonte==NULL){
		printf("Não foi possível carregar uma fonte\n");
	}
	return fonte;
}

void desenha_texto(Display *disp, int tela, Window janela, char *texto, int verm, int verd, int azul, unsigned int x, unsigned int y)
{
	GC gc = XCreateGC(disp, janela, 0, NULL);
	XFontStruct *fonte = carregar_fonte(disp);
	XSetFont(disp, gc, fonte->fid);
	int largtexto = XTextWidth(fonte, texto, strlen(texto));
	int alttexto = fonte->ascent + fonte->descent;
	x = x - largtexto/2;
	y = y + alttexto/2;

	Colormap colormaptela = XDefaultColormap(disp, tela);
	XColor cor;
	cor.red = rgb_to_xrgb(verm);
	cor.green = rgb_to_xrgb(verd);
	cor.blue = rgb_to_xrgb(azul);
	XAllocColor(disp, colormaptela, &cor);
	XSetBackground(disp, gc, cor.pixel);
	XSetForeground(disp, gc, cor.pixel);

	XDrawString(disp, janela, gc, x, y, texto, strlen(texto)); 
	free (fonte);
}

