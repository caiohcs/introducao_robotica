#ifndef REGIAO
#define REGIAO
#include "../headers/cabecalho.h"

struct coordenadas{
	int X, Y, regiao;
};

void detect_regiao(struct pixel matriz[altura][largura]);
#endif
