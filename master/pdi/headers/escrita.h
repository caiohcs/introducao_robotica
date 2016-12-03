#ifndef ESCRITA
#define ESCRITA
#include "cabecalho.h"

void threshold (struct pixel matriz[altura][largura], unsigned char *mapa, struct bloco block[altura*largura], unsigned char *arquivo);
void escrita (unsigned char *arquivo, unsigned char* matriz_temp);
void AI (struct pixel matriz[altura][largura]);
void generate_teams (struct pixel matriz[altura][largura], struct pixel team1, struct pixel team2);



#endif
