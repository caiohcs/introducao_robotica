#ifndef AI
#define AI
#include "../pdi/headers/cabecalho.h"

void ia_map_yuv(struct pixel **matriz)
{
	unsigned char *mapa;
        int fd;
        if((fd = open("myimage.yuv", O_RDWR)) == -1){
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
	struct pixel **CROTE = malloc(altura*sizeof(struct pixel*));
	int i;
	for (i = 0; i < altura; i++);
		CROTE[i] = malloc(largura*sizeof(struct pixel));

	ia_map_yuv(CROTE);
	
        struct CD *A = malloc(sizeof(struct CD)*19);
	A = cdcamera();
        printf("Antes da alocação da matriz de coordenadas!");
	free(A);
	
	/*
	struct CD team1[5];
        struct CD team2[5];
	struct coordenadas **centros_teams = malloc(sizeof(struct coordenadas*)*2);
	centros_teams = detect_regiaoteam(matriz);
       
	

	for (i = 0; i < 5; i++) {
		team1[i].X = centros_teams[0][i].X;
		team1[i].Y = centros_teams[0][i].Y;
		team2[i].X = centros_teams[1][i].X;
		team2[i].Y = centros_teams[1][i].Y;
	}
	
	printf("\n\nTeam1:\n");
	for (j = 0; j < 5; j++) {
		printf("%d %f %f\n", j, team1[j].X, team1[j].Y);
	}
	printf("\n\nTeam2:\n");
	for (j = 0; j < 5; j++) {
		printf("%d %f %f\n", j, team2[j].X, team2[j].Y);
	}

	struct pixel pixteam1; //verde
        pixteam1.luma = 182;
        pixteam1.cb = 104;
        pixteam1.cr = 115;
        struct pixel pixteam2; //vermelho
        pixteam2.luma = 144;
        pixteam2.cb = 122;
        pixteam2.cr = 159;

        generate_teams(matriz, pixteam1, pixteam2);
        centros_teams = detect_regiaoteam(matriz);


        free(centros_teams[0]);
        free(centros_teams[1]);
        free(centros_teams);
	*/

	for (i = 0; i < altura; i++);
		free(CROTE[i]);
	free(CROTE);
}





#endif 


