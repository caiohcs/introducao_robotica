#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <string.h>

#ifndef TEXTOS_H
#define TEXTOS_H
void enviar_comandoX(Display *disp);

int rgb_to_xrgb(int cor) {return cor*(65535/255);}

XFontStruct* fonte;

XFontStruct *carregar_fonte(Display *disp)
{

	int i = 0;
	int num_fontes_carregadas;
	int max_fontes = 100;
	char **nomefontes = XListFonts(disp, "*", max_fontes, &num_fontes_carregadas);

	fonte = NULL;
	
 	/*Esse laço busca carregar a primeira fonte encontrada em nomefontes na variável fonte. Note que o índice "i" começa em 11 para pular as fontes que causam problemas de visualização. */

	while((fonte==NULL)  &&  (i < num_fontes_carregadas)){
		fonte = XLoadQueryFont(disp,nomefontes[i]);
		i=i+1;
	}

	if(fonte==NULL){
		printf("Não foi possível carregar uma fonte\n");
	}
	return fonte;
}

void desenha_texto(Display *disp, int tela, Window *janela, char *texto, int verm, int verd, int azul, unsigned int x, unsigned int y)
{
	GC gc = XCreateGC(disp, *janela, 0, NULL);
	fonte = carregar_fonte(disp);
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

	XDrawString(disp, *janela, gc, x, y, texto, strlen(texto)); 
	free (fonte);
}

#endif
