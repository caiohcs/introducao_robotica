#ifndef PIXEL_H
#define PIXEL_H
#include <stdio.h>
#include <stdlib.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h> // for close
#include <sys/stat.h>
#include "pixel.h" 
#include <math.h>
#include "escrita.h"
#include "deteccao.h"
#include "regiao.h"
#endif

#ifndef PARAMETROS
#define PARAMETROS

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define altura 480
#define largura 640
#define delta_x 1
#define delta_y 1
int extern limiar;
#endif
