#include "../headers/cabecalho.h"


int percorre_tudo(struct coordenadas pixcircle[300], int size, int nregs, int x, int y)
{
	for (int i = 0; i < size; i++)
	{
		if (pixcircle[i].regiao == -1 &&
				(pixcircle[i].X == x || pixcircle[i].X == x+1 || pixcircle[i].X == x-1) && 
				(pixcircle[i].Y == y || pixcircle[i].Y == y+1 || pixcircle[i].Y == y-1)) {
			pixcircle[i].regiao = nregs;
			percorre_tudo(pixcircle, size, nregs, pixcircle[i].X, pixcircle[i].Y);
		} else {
		}
	}
	return 0;
}

void detect_regiao(struct pixel matriz[altura][largura])
{
	struct coordenadas pixcircle[300];
	int npixcircle = 0;
       
	for (int i = 110; i < 350; i++) {
       		for (int j = 340; j < 510; j++) {
			if (matriz[i][j].circle == 1) {
				pixcircle[npixcircle].X = i;
				pixcircle[npixcircle].Y = j;
				pixcircle[npixcircle].regiao = -1;
				npixcircle++;
			}
		}
	}

	int nregs = 0;
	for (int i = 0; i < npixcircle; i++)
	{
		if (pixcircle[i].regiao == -1) {
			nregs++;
			pixcircle[i].regiao = nregs;
			percorre_tudo(pixcircle, npixcircle, nregs, pixcircle[i].X, pixcircle[i].Y);
		}
	}

	int x = 0, y = 0, n = 0;
	struct coordenadas *centros = malloc(sizeof(struct coordenadas)*nregs);

	for (int j = 0; j < nregs; j++){
		x = 0;
		y = 0;
		n = 0;
		for (int i = 0; i < npixcircle; i++) {
			if (pixcircle[i].regiao == j+1) {
				x += pixcircle[i].X;
				y += pixcircle[i].Y;
				n++;
			}
		}
		centros[j].X = x/n;
		centros[j].Y = y/n;
	}


	for (int j = 0; j < nregs; j++){
		printf("Regiao: %d Centro X : %d Centro Y: %d\n", j+1, centros[j].X, centros[j].Y);
	}
}

