#include "../headers/cabecalho.h"

void threshold (struct pixel matriz[altura][largura], unsigned char *mapa, struct bloco block[altura*largura], unsigned char *arquivo) {
	limiar=10;
	unsigned char *prototipo;
        prototipo = malloc(altura*largura*2);
	alocate(matriz,mapa);
	
	/*
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
	
	detect_regiao(matriz);
	*/

	AI(matriz);
	
	dealocate(matriz, prototipo);
	arquivo[0]='0' + 0; //Cheat de converter o inteiro para char
	escrita(arquivo, prototipo);
	printf("Valor do limiar: %d\n", limiar);
}


void AI(struct pixel matriz[altura][largura])
{
	    
	struct pixel pixteam1; //verde
	pixteam1.luma = 182;
	pixteam1.cb = 104;
	pixteam1.cr = 115;
	struct pixel pixteam2; //vermelho
	pixteam2.luma = 144;
	pixteam2.cb = 122;
	pixteam2.cr = 159;

	generate_teams(matriz, pixteam1, pixteam2);
	detect_regiaoteam(matriz);
}

void generate_teams(struct pixel matriz[altura][largura], struct pixel team1, struct pixel team2)
{
	struct pixel tmp;

	for (int i = altmin; i < altmax; i++) {
		for (int j = largmin; j < largmax; j++) {
			tmp = lumcbcr_medium(matriz, i, j, 8);
			matriz[i][j].team = 0;
			if (pix_erro(tmp, team1) < 20) matriz[i][j].team = 1;
			if (pix_erro(tmp, team2) < 20) matriz[i][j].team = 2;
		}
	}

	for (int i = altmin; i < altmax; i++) {
		for (int j = largmin; j < largmax; j++) {
			matriz[i][j].luma = 0;
			matriz[i][j].cb = 128;
			matriz[i][j].cr = 128;
			if (matriz[i][j].team == 1) {
				matriz[i][j].luma = team1.luma;
				matriz[i][j].cb = team1.cb;
				matriz[i][j].cr = team1.cr;
			} else if (matriz[i][j].team == 2) {
				matriz[i][j].luma = team2.luma;
				matriz[i][j].cb = team2.cb;
				matriz[i][j].cr = team2.cr;
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
