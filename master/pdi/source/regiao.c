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
       
	for (int i = altmin; i < altmax; i++) {
       		for (int j = largmin; j < largmax; j++) {
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
	struct coordenadas *hashtag = malloc(sizeof(struct coordenadas)*9);

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

	/*
	 *	Calcula o centro do hashtag (hashtag 4)
	 *	0|1|2
	 *	3|4|5
	 *	6|7|8
	 */
	float y2, y1, x2, x1, a1, a2, b1, b2, teta, teta1, d;
	x1 = centros[0].X;
	y1 = centros[0].Y;
	x2 = centros[9].X;
	y2 = centros[9].Y;
	a1 = (y2 - y1)/(x2 - x1);
	b1 = (y2*x1 - y1*x2)/(x1 - x2);

	x1 = centros[4].X;
	y1 = centros[4].Y;
	x2 = centros[5].X;
	y2 = centros[5].Y;
	a2 = (y2 - y1)/(x2 - x1);
	b2 = (y2*x1 - y1*x2)/(x1 - x2);

	hashtag[4].X = (int) round((b2 - b1)/(a1 - a2));
	hashtag[4].Y = (int) round((b1*a2 - b2*a1)/(a2 - a1));
	
	/*
	 *	Calcula o 1 e 7 do hashtag
	 *
	 */
	x1 = centros[2].X;
	y1 = centros[2].Y;
	x2 = centros[7].X;
	y2 = centros[7].Y;
	
	a1 = (y2 - y1)/(x2 - x1);
	teta = atan(a1); 
	teta1 = teta;
	d = 0.2101*pow(pow(x2 - x1, 2) + pow(y2 - y1, 2), 0.5); // d = sqrt(x²+y²)
	
	hashtag[1].X = round(hashtag[4].X - fabs(d*cos(teta)));
	hashtag[1].Y = round(hashtag[4].Y + fabs(d*sin(teta)));

	hashtag[7].X = round(hashtag[4].X + fabs(d*cos(teta)));
	hashtag[7].Y = round(hashtag[4].Y - fabs(d*sin(teta)));
	
	/*
	 * Calcula o 3 e o 5 do hashtag
	 * 
	 */
	x1 = centros[0].X;
	y1 = centros[0].Y;
	x2 = centros[4].X;
	y2 = centros[4].Y;

	a1 = (y2 - y1)/(x2 - x1);
	teta = atan(a1);
	hashtag[5].X = round(hashtag[4].X + fabs(d*cos(teta)));
	hashtag[5].Y = round(hashtag[4].Y + fabs(d*sin(teta)));

	hashtag[3].X = round(hashtag[4].X - fabs(d*cos(teta)));
	hashtag[3].Y = round(hashtag[4].Y - fabs(d*sin(teta)));

	/*
	 * Calcula o 0 e 6 e 2 e 8 do hashtag
	 *
	 */

	hashtag[0].X = round(hashtag[3].X - fabs(d*cos(teta1)));
	hashtag[0].Y = round(hashtag[3].Y + fabs(d*sin(teta1)));

	hashtag[6].X = round(hashtag[3].X + fabs(d*cos(teta1)));
	hashtag[6].Y = round(hashtag[3].Y - fabs(d*sin(teta1)));
	
	hashtag[2].X = round(hashtag[5].X - fabs(d*cos(teta1)));
	hashtag[2].Y = round(hashtag[5].Y + fabs(d*sin(teta1)));

	hashtag[8].X = round(hashtag[5].X + fabs(d*cos(teta1)));
	hashtag[8].Y = round(hashtag[5].Y - fabs(d*sin(teta1)));

	for (int i = 0; i < 9; i++) printf("%d) X: %d Y: %d\n", i, hashtag[i].X, hashtag[i].Y); 
	

	for (int i = 0; i < 9; i++) {
		matriz[hashtag[i].X][hashtag[i].Y].luma = 255;
		matriz[hashtag[i].X][hashtag[i].Y].cb = 127;
		matriz[hashtag[i].X][hashtag[i].Y].cr = 127;
	}

	free(centros);
	free(hashtag);
}




int percorre_tudoteam(struct coordenadas pixteam[2000], int size, int nregs, int x, int y)
{
	for (int i = 0; i < size; i++)
	{
		if (pixteam[i].regiao == -1 &&
				(pixteam[i].X == x || pixteam[i].X == x+1 || pixteam[i].X == x-1) && 
				(pixteam[i].Y == y || pixteam[i].Y == y+1 || pixteam[i].Y == y-1)) {
			pixteam[i].regiao = nregs;
			percorre_tudoteam(pixteam, size, nregs, pixteam[i].X, pixteam[i].Y);
		} else {
		}
	}
	return 0;
}

void detect_regiaoteam(struct pixel matriz[altura][largura])
{
	struct coordenadas pixteam1[2000];
	struct coordenadas pixteam2[2000];
	int npixteam1 = 0, npixteam2 = 0;
       
	for (int i = altmin; i < altmax; i++) {
       		for (int j = largmin; j < largmax; j++) {
			if (matriz[i][j].team == 1) {
				pixteam1[npixteam1].X = i;
				pixteam1[npixteam1].Y = j;
				pixteam1[npixteam1].regiao = -1;
				npixteam1++;
			} else if (matriz[i][j].team == 2) { 
				pixteam2[npixteam2].X = i;
				pixteam2[npixteam2].Y = j;
				pixteam2[npixteam2].regiao = -1;
				npixteam2++;
			}
		}
	}

	printf("team1: %d team2: %d\n", npixteam1, npixteam2);
	int nregsteam1 = 0, nregsteam2 = 0;
	for (int i = 0; i < npixteam1; i++)
	{
		if (pixteam1[i].regiao == -1) {
			nregsteam1++;
			pixteam1[i].regiao = nregsteam1;
			percorre_tudoteam(pixteam1, npixteam1, nregsteam1, pixteam1[i].X, pixteam1[i].Y);
		}
	}
	for (int i = 0; i < npixteam2; i++)
	{
		if (pixteam2[i].regiao == -1) {
			nregsteam2++;
			pixteam2[i].regiao = nregsteam2;
			percorre_tudoteam(pixteam2, npixteam2, nregsteam2, pixteam2[i].X, pixteam2[i].Y);
		}
	}

	int x = 0, y = 0, n = 0;
	struct coordenadas *centros1 = malloc(sizeof(struct coordenadas)*nregsteam1);
	struct coordenadas *centros2 = malloc(sizeof(struct coordenadas)*nregsteam2);
	for (int j = 0; j < nregsteam1; j++){
		x = 0;
		y = 0;
		n = 0;
		for (int i = 0; i < npixteam1; i++) {
			if (pixteam1[i].regiao == j+1) {
				x += pixteam1[i].X;
				y += pixteam1[i].Y;
				n++;
			}
		}
		centros1[j].X = x/n;
		centros1[j].Y = y/n;
	}

	for (int j = 0; j < nregsteam2; j++){
		x = 0;
		y = 0;
		n = 0;
		for (int i = 0; i < npixteam2; i++) {
			if (pixteam2[i].regiao == j+1) {
				x += pixteam2[i].X;
				y += pixteam2[i].Y;
				n++;
			}
		}
		centros2[j].X = x/n;
		centros2[j].Y = y/n;
	}
	printf("Team1:\n");
	for (int j = 0; j < nregsteam1; j++){
		printf("Regiao: %d Centro X : %d Centro Y: %d\n", j+1, centros1[j].X, centros1[j].Y);
	}
	printf("Team2:\n");
	for (int j = 0; j < nregsteam2; j++){
		printf("Regiao: %d Centro X : %d Centro Y: %d\n", j+1, centros2[j].X, centros2[j].Y);
	}



	free(centros1);
	free(centros2);
}

