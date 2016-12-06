#ifndef ESCRITA
#define ESCRITA
#include "cabecalho.h"

void threshold (struct pixel matriz[altura][largura], unsigned char *mapa, struct bloco block[altura*largura], char *arquivo);
void escrita (char *arquivo, unsigned char* matriz_temp);
void AI (struct pixel matriz[altura][largura]);
void generate_teams (struct pixel matriz[altura][largura], struct pixel team1, struct pixel team2);
struct CD * print_ballcoord(struct pixel matriz[altura][largura]);


#endif
