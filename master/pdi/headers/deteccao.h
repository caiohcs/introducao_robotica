#ifndef DETECCAO
#define DETECCAO
#include "cabecalho.h"


void find_round(struct pixel matriz[altura][largura]);
void detect_circle(struct pixel matriz[altura][largura]);
void calc_circle(struct pixel matriz[altura][largura], int X, int Y);



#endif
