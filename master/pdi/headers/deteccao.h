#ifndef DETECCAO
#define DETECCAO
#include "cabecalho.h"


void find_round(struct pixel **matriz);
void detect_circle(struct pixel **matriz);
void calc_circle(struct pixel **matriz, int X, int Y);
void swell_circle(struct pixel **matriz, int nprox);
void shrink_circle(struct pixel **matriz, int nprox);
void detect_proxcircle(struct pixel **matriz);
void generate_proxcircle(struct pixel **matriz);

#endif
