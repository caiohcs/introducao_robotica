#include <X11/Xlib.h>
#include "textos.h"

#ifndef GC_H
#define GC_H

GC gc_pintar (Display *disp, Window win, unsigned long valuesmask, XGCValues *values)
{
 GC gc_preto = XCreateGC(disp, win, valuesmask, values);
 XSetForeground(disp,gc_preto,XBlackPixel(disp, DefaultScreen(disp)));
 XSetBackground(disp,gc_preto,XBlackPixel(disp, DefaultScreen(disp)));
 return gc_preto;
}

GC gc_apagar (Display *disp, Window win, unsigned long valuesmask, XGCValues *values)
{
 GC gc_branco = XCreateGC(disp, win, valuesmask, values);
 XSetForeground(disp,gc_branco,XWhitePixel(disp, DefaultScreen(disp)));
 XSetBackground(disp,gc_branco,XWhitePixel(disp, DefaultScreen(disp)));
 return gc_branco;
}

GC gc_vermelho (Display *disp, Window win, unsigned long valuesmask, XGCValues *values)
{
   	GC gc_vermelho = XCreateGC(disp, win, valuesmask, values);
        Colormap colormaptela = XDefaultColormap(disp, XDefaultScreen(disp));
        XColor cor;
        cor.red = rgb_to_xrgb(255);
        cor.green = rgb_to_xrgb(0);
        cor.blue = rgb_to_xrgb(0);
        XAllocColor(disp, colormaptela, &cor);
        XSetBackground(disp, gc_vermelho, cor.pixel);
        XSetForeground(disp, gc_vermelho, cor.pixel);

	return gc_vermelho;
}
#endif
