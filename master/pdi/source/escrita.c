#include "../headers/cabecalho.h"

void threshold (struct pixel matriz[altura][largura], unsigned char *mapa, struct bloco block[altura*largura], unsigned char *arquivo) {
	limiar=10;
	raio=8;
	unsigned char *prototipo;
        prototipo = malloc(altura*largura*2);
	alocate(matriz,mapa);
	array_bloco(block, matriz);
	generate_grad(matriz, block);	
	generate_prox(matriz);
	shrink(matriz, 4);
	swell(matriz, 6);
	find_surround(matriz,raio);
	dealocate(matriz, prototipo);
	arquivo[0]='0' + 0; //Cheat de converter o inteiro para char
	escrita(arquivo, prototipo);
	printf("Valor do limiar: %d\n", limiar);
}


void AI(struct pixel matriz[altura][largura])
{
	    
	/*
	*	Valores médios dos lumas, cbs, crs
	*	de peças amarelas e azuis e posições vazias.
	*	Para calibrar, tire uma foto, determine
	*	o centro das peças e use a função  lumcbcr_medium.
	*
	*/
	struct pixel pixyellow;
	pixyellow.luma = 230;
	pixyellow.cb = 65;
	pixyellow.cr = 144;
	struct pixel pixblue;
	pixblue.luma = 116;
	pixblue.cb = 191;
	pixblue.cr = 80;
	struct pixel pixA;
	pixA.luma = 241;
	pixA.cb = 134;
	pixA.cr = 128;
	struct pixel pixRH;
	pixRH.luma = 253;
	pixRH.cb = 128;
	pixRH.cr = 127;


	struct posicao A[3][3];
	struct posicao R[6];
	struct posicao H[6];
	A[0][0].X = 196;
	A[0][0].Y = 460;
	A[0][1].X = 193;
	A[0][1].Y = 424;
	A[0][2].X = 190;
	A[0][2].Y = 387;

	A[1][0].X = 233;
	A[1][0].Y = 458;
	A[1][1].X = 230;
	A[1][1].Y = 422;
	A[1][2].X = 228;
	A[1][2].Y = 385;

	A[2][0].X = 267;
	A[2][0].Y = 455;
	A[2][1].X = 265;
	A[2][1].Y = 419;
	A[2][2].X = 263;
	A[2][2].Y = 383;
	
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			A[i][j].pix = lumcbcr_medium(matriz, A[i][j].X, A[i][j].Y, 13);
		}
	}

	int erro = 0, errovazio = 0, erroamarelo = 0, erroazul = 0;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			errovazio = pix_erro(A[i][j].pix, pixA);
			erroamarelo = pix_erro(A[i][j].pix, pixyellow);
			erroazul = pix_erro(A[i][j].pix, pixblue);
			erro = min(errovazio, min(erroamarelo, erroazul));		
			if (erro == errovazio) {
				A[i][j].status = 0;
				printf("A %d%d vazio, erro %d\n", i, j, erro); 
			} else if (erro == erroamarelo) {
				A[i][j].status = 1;
				printf("A %d%d amarelo, erro %d\n", i, j, erro); 
			} else if (erro == erroazul) {
				A[i][j].status = 2;
				printf("A %d%d azul, erro %d\n", i, j, erro); 
			}
				
		}
	}
}




int pix_erro(struct pixel original, struct pixel compare){
	int erro = 0;
	erro = abs(original.luma - compare.luma) + 
		abs(original.cb - compare.cb) +
		abs(original.cr - compare.cr);
	return erro;
}



void escrita (unsigned char *arquivo, unsigned char* matriz_temp) {
	int jpgfile;
        if((jpgfile = open(arquivo, O_WRONLY | O_CREAT, 0660)) < 0){
            perror("open");
            exit(1);

        }

        write(jpgfile, matriz_temp, altura*largura*2);
        close(jpgfile);
}
