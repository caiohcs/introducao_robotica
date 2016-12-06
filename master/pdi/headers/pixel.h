#ifndef CABECALHO_H
#define CABECALHO_H
#include "cabecalho.h"

struct pixel{
	unsigned char luma,cb, cr;
	unsigned char grad, prox, circle, proxcircle, team, prox_team;
};


/*
 * Status = 0 -> posicao sem peça
 * Status = 1 -> ocupado por peça amarela
 * Status = 2 -> ocupado por peça azul
 *
 */

struct posicao{
	int X, Y, status;
	struct pixel pix;
};

/*O bloco é uma estrutura que contém o luma de
um pixel e os outros três ao seu redor necessários
para os cálculos do gradiente. */

struct bloco{
	unsigned char ij_luma, ij1_luma, i1j_luma, i1j1_luma;
	unsigned char ij_cb, ij1_cb, i1j_cb, i1j1_cb;
	unsigned char ij_cr, ij1_cr, i1j_cr, i1j1_cr;
};

void array_bloco(struct bloco *block, struct pixel **matriz);
struct bloco create_block(struct pixel **matriz, int k, int p);
void alocate(struct pixel **matriz, unsigned char *mmap);
struct pixel edge_detection(struct bloco block);
void generate_grad(struct pixel **grad, struct bloco *block);
void generate_prox(struct pixel **matriz);
void AI(struct pixel **grad);
void dealocate(struct pixel **gradiente, unsigned char *mmap);
void swell(struct pixel **matriz, int nprox);
void shrink(struct pixel **matriz, int nprox);
int pix_erro(struct pixel original, struct pixel compare); 
struct pixel lumcbcr_medium(struct pixel **matriz, int X, int Y, int R);
/*Função de faz os calculos do apocalipse e retorn 255 se o valor estiver acima do limiar, e 0 se estiver abaixo.*/

struct pixel gradient_check(struct bloco block);	
#endif
