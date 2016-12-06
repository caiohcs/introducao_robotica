#ifndef REGIAO
#define REGIAO
#include "../headers/cabecalho.h"

struct coordenadas{
	int X, Y, regiao;
};

/* Usa-se essa estrutura para poder retornar na função detect_regiao tanto a região quanto o tamanho */

struct coordenadas_size{                
	struct coordenadas *coord;
        int size;
};



struct coordenadas_size detect_regiao(struct pixel **matriz);
void detect_regiaoteam(struct pixel **matriz);
int percorre_tudo(struct coordenadas pixteam[300], int size, int nregs, int x, int y);
int percorre_tudoteam(struct coordenadas pixteam[2000], int size, int nregs, int x, int y);
struct coordenadas *generate_hashtag(struct coordenadas *bolinhas, int size);
#endif
