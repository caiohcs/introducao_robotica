#ifndef AI
#define AI
#include "../pdi/headers/cabecalho.h"

void ia_map_yuv(struct pixel **matriz)
{
	unsigned char *mapa;
        int fd;
        if((fd = open("tabuleiro_com_pecas.yuv", O_RDWR)) == -1){
                perror("open");
                exit(1);
        }

        struct stat buf;
        if(fstat(fd, &buf) == -1){
                perror("fstat");
                exit(1);
        }

        mapa = mmap(0, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(mapa == MAP_FAILED){
                perror("mmap");
                exit(1);
        }

        alocate(matriz,mapa);
        close(fd);
        munmap(mapa, buf.st_size);
}

void ia()
{
	struct pixel pixteam1; //verde
        pixteam1.luma = 182;
        pixteam1.cb = 104;
        pixteam1.cr = 115;
        struct pixel pixteam2; //vermelho
        pixteam2.luma = 144;
        pixteam2.cb = 122;
        pixteam2.cr = 159;
	unsigned char *prototipo = malloc(largura*altura*2);

	struct pixel **matriz = malloc(altura*sizeof(struct pixel*));
	struct coordenadas **centros_teams = malloc(sizeof(struct coordenadas*)*2);
	int i;
	for (i = 0; i < altura; i++)
		matriz[i] = malloc(largura*sizeof(struct pixel));
	
	ia_map_yuv(matriz);
        generate_teams(matriz, pixteam1, pixteam2);
	generate_prox_teams(matriz);
	shrink_teams(matriz, 5);	// caso detecte mais peças do que deveria, ajustar os valores dos filtros swell e shrink, nao esquecer de generate_prox_teams
	//swell_teams(matriz, 4);
	centros_teams = detect_regiaoteam(matriz);
	dealocate(matriz, prototipo);
	escrita("teams.jpg", prototipo);
	free(prototipo);
	
	struct CD team1[5];
        struct CD team2[5];
        struct CD *centros_posicoes_vazias;
        struct CD centros_hashtag[9];
        struct CD centros_bolinhas_team1[5];

	centros_posicoes_vazias = cdcamera();

	for (i = 0; i < 5; i++) {
		team1[i].X = centros_teams[0][i].X;
		team1[i].Y = centros_teams[0][i].Y;
		team2[i].X = centros_teams[1][i].X;
		team2[i].Y = centros_teams[1][i].Y;
	}

	for (i = 0; i < 9; i++) {
		centros_hashtag[i].X = centros_posicoes_vazias[i].X;
		centros_hashtag[i].Y = centros_posicoes_vazias[i].Y;
	}

	for (i = 0; i < 5; i++) {
		centros_bolinhas_team1[i].X = centros_posicoes_vazias[i+9].X;
		centros_bolinhas_team1[i].Y = centros_posicoes_vazias[i+9].Y;
	}

	free(centros_teams[0]);
	free(centros_teams[1]);
	free(centros_teams);

	printf("\n\nTeam1:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, team1[i].X, team1[i].Y);
	}
	printf("\n\nTeam2:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, team2[i].X, team2[i].Y);
	}
	printf("\n\nCentros hashtag:\n");
	for (i = 0; i < 9; i++) {
		printf("%d %f %f\n", i, centros_hashtag[i].X, centros_hashtag[i].Y);
	}
	printf("\n\nCentros bolinhas team1:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, centros_bolinhas_team1[i].X, centros_bolinhas_team1[i].Y);
	}

	





	for (i = 0; i < altura; i++)
		free(matriz[i]);
	free(matriz);
	free(centros_posicoes_vazias);
}





#endif 


