#ifndef REGIAO
#define REGIAO
#include "../headers/cabecalho.h"

struct coordenadas{
	int X, Y, regiao;
};

void detect_regiao(struct pixel matriz[altura][largura]);
void detect_regiaoteam(struct pixel matriz[altura][largura]);
int percorre_tudo(struct coordenadas pixteam[300], int size, int nregs, int x, int y);
int percorre_tudoteam(struct coordenadas pixteam[2000], int size, int nregs, int x, int y);
#endif
