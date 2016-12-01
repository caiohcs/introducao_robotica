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
#endif

#ifndef PARAMETROS
#define PARAMETROS

#define altura 480
#define largura 640
#define delta_x 1
#define delta_y 1
int extern limiar;
#endif
