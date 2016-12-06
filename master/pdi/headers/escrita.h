#ifndef ESCRITA
#define ESCRITA
#include "cabecalho.h"

void threshold (struct pixel **matriz, unsigned char *mapa, struct bloco *block, char *arquivo);
void escrita (char *arquivo, unsigned char* matriz_temp);
void AI (struct pixel **matriz);
void generate_teams (struct pixel **matriz, struct pixel team1, struct pixel team2);
struct CD * print_ballcoord(struct pixel **matriz);


#endif
