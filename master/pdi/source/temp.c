#include "../headers/cabecalho.h"

void threshold (struct pixel matriz[altura][largura], unsigned char *mapa, struct bloco block[altura*largura], unsigned char *arquivo) {
	limiar=10;
	unsigned char *prototipo;
        prototipo = malloc(altura*largura*2);
	alocate(matriz,mapa);
	
	
	array_bloco(block, matriz);
	generate_grad(matriz, block);	
	generate_prox(matriz);
	shrink(matriz, 4);
	generate_prox(matriz);
	swell(matriz, 6);
	
	detect_circle(matriz);
	generate_proxcircle(matriz);
	shrink_circle(matriz, 5);
	generate_proxcircle(matriz);
	        
	struct coordenadas_size aux = detect_regiao(matriz);
	struct coordenadas *bolinhas = aux.coord;
	aux.coord = NULL;
	int nbolinhas = aux.size;
	struct coordenadas *hashtag = generate_hashtag(bolinhas, nbolinhas);

	for (int j = 0; j < nbolinhas; j++){
		printf("Bolinha: %d Centro X : %d Centro Y: %d\n", j+1, bolinhas[j].X, bolinhas[j].Y);
	}
	for (int i = 0; i < 9; i++) printf("%d) X: %d Y: %d\n", i, hashtag[i].X, hashtag[i].Y);
	
	free(bolinhas);
	free(hashtag);
	dealocate(matriz, prototipo);
	arquivo[0]='0' + 0; //Cheat de converter o inteiro para char
	escrita(arquivo, prototipo);
	printf("Valor do limiar: %d\n", limiar);
}


