#include <X11/Xlib.h>

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

#endif
