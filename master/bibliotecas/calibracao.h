#ifndef CALIBRACAO
#define CALIBRACAO
#include "../pdi/headers/cabecalho.h"


int limiar=10;

struct CD{
	float X, Y;
};


struct CD *cdcamera()
{
	struct CD *cds = malloc(sizeof(struct CD)*19);
        struct pixel matriz[altura][largura];
        struct bloco tijolo[altura*largura];

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

        unsigned char *prototipo;
        prototipo = malloc(altura*largura*2);
        alocate(matriz,mapa);

        array_bloco(tijolo, matriz);
        generate_grad(matriz, tijolo);
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
	int i;

	for (i = 0; i < 3; i++) {
		cds[0 + 3*i].X = hashtag[6 + i].X;
		cds[0 + 3*i].Y = hashtag[6 + i].Y;
		
		cds[1 + 3*i].X = hashtag[3 + i].X;
		cds[1 + 3*i].Y = hashtag[3 + i].Y;

		cds[2 + 3*i].X = hashtag[0 + i].X;
		cds[2 + 3*i].Y = hashtag[0 + i].Y;
	}

	for (i = 0; i < 10; i++) {
		cds[9 + i].X = bolinhas[i].X;
		cds[9 + i].Y = bolinhas[i].Y;
	}

	printf("Camera\n");
	for (i = 0; i < 19; i++) {
		printf("%d %f %f\n", i, cds[i].X, cds[i].Y);
	}

        free(bolinhas);
        free(hashtag);
        dealocate(matriz, prototipo);
        // nao esquecer de desalocar
	munmap(mapa, buf.st_size);
        close(fd);
        return cds;
}

struct CD *cdworld()
{      
	struct CD *cds = malloc(sizeof(struct CD)*19);
        FILE *fp;
        static char movebuffer[100];
        memset(movebuffer, 0, sizeof(char)*100);
        buffer[strlen(buffer)-1] = '\0';        
        fp = fopen("calibrar", "r");                
        int i = 0;
	while (fgets(buffer, sizeof(char)*150, fp)) {
                switch (buffer[2]) {  
                        case 'M':       
                                sscanf(buffer, "%*d)%s(%*d,%*d,%*d,%*d);", movebuffer);
                                if (strncmp(movebuffer, "MOVE", 4) == 0){
                                        sscanf(buffer, "%*d)MOVE(%f,%f,%*f,%*f);", &cds[i].X, &cds[i].Y);
					i++;
                                }
                        break;
			default:
			break;
                } 
        }

	printf("Mundo\n");
	for (i = 0; i < 19; i++) {
		printf("%d %f %f\n", i, cds[i].X, cds[i].Y);
	}

	//Não esquecer de desalocar
	return cds; 
}


#endif
