#ifndef CABECALHO_H
#define CABECALHO_H
#include "cabecalho.h"

struct pixel{
	unsigned char luma,cb, cr;
	unsigned char grad, prox;
};

/*O bloco é uma estrutura que contém o luma de
um pixel e os outros três ao seu redor necessários
para os cálculos do gradiente. */

struct bloco{
	unsigned char ij, ij1, i1j, i1j1;
};

void array_bloco(struct bloco block[altura*largura], struct pixel matriz[altura][largura]);
struct bloco create_block(struct pixel matriz[altura][largura], int k, int p);
void alocate(struct pixel matriz[altura][largura], unsigned char *mmap);
struct pixel edge_detection(struct bloco block);
void generate_grad(struct pixel grad[altura][largura], struct bloco block[altura*largura]);
void generate_prox(struct pixel matriz[altura][largura]);
void dealocate(struct pixel gradiente[altura][largura], unsigned char *mmap);
/*Função de faz os calculos do apocalipse e retorn 255 se o valor estiver acima do limiar, e 0 se estiver abaixo.*/

struct pixel gradient_check(struct bloco block);	
#endif
