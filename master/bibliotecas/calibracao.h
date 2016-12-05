#ifndef CALIBRACAO
#define CALIBRACAO
#include "../pdi/headers/cabecalho.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>


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

void calibra()
{
	int i, j;
	struct CD *ccd = cdcamera();
	struct CD *wcd = cdworld();
	gsl_matrix *A = gsl_matrix_alloc(38, 8);
	gsl_matrix *x = gsl_matrix_alloc(8, 1);
	gsl_matrix *tmpAtA = gsl_matrix_alloc(8, 8);
	gsl_matrix *tmp = gsl_matrix_alloc(8, 38);
	gsl_matrix *At = gsl_matrix_alloc(8, 38);
	gsl_matrix *AtA = gsl_matrix_alloc(8, 8);
	gsl_matrix *iAtA = gsl_matrix_alloc(8, 8);
	gsl_matrix *b = gsl_matrix_alloc(38, 1);
	gsl_permutation *p = gsl_permutation_alloc(8);
	int *signal = malloc(8*sizeof(int));

	gsl_matrix_set_zero(A);
	
	for (i = 0; i < 38; i+=2) {
		int f=(i/2);
		gsl_matrix_set(A, i, 0, wcd[f].X);
		gsl_matrix_set(A, i, 1, wcd[f].Y);
		gsl_matrix_set(A, i, 2, 1);
		gsl_matrix_set(A, i, 6, (-1)*ccd[f].X*wcd[f].X);
		gsl_matrix_set(A, i, 7, (-1)*ccd[f].X*wcd[f].Y);
	
		gsl_matrix_set(A, i+1, 3, wcd[f].X);
		gsl_matrix_set(A, i+1, 4, wcd[f].Y);
		gsl_matrix_set(A, i+1, 5, 1);
		gsl_matrix_set(A, i+1, 6, (-1)*ccd[f].Y*wcd[f].X);
		gsl_matrix_set(A, i+1, 7, (-1)*ccd[f].Y*wcd[f].Y);
	}
	
	
	for (i = 0; i < 38; i+=2) {
		int f=(i/2);
		gsl_matrix_set(b, i, 0, ccd[f].X);
		gsl_matrix_set(b, i+1, 0, ccd[f].Y);
	}

	/*
	for (i = 0; i < 38; i++) {
		printf("\nNúmero da linha %d\t",i);
		for (j = 0; j < 8; j++) {
			printf("%g\t", gsl_matrix_get(A, i, j));
		}
	}

	for (i = 0; i < 18; i++) {
		printf("\nValor de U%d: %g\t Valor de V%d: %g\t", i, gsl_matrix_get(b, i*2, 0), i, gsl_matrix_get(b, i*2+1, 0));
	}
	*/

	gsl_matrix_transpose_memcpy(At, A);
	gsl_matrix_memcpy(tmp, At);
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
			1.0, At, A, 0.0, tmpAtA);
	
	gsl_linalg_LU_decomp(tmpAtA, p, signal);
	gsl_linalg_LU_invert(tmpAtA, p, iAtA);
	
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
			1.0, iAtA, At, 0.0, tmp);
	
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
			1.0, tmp, b, 0.0, x);
	
	for (i = 0; i < 8; i++) {
		printf("Valor de h %d\t %g\n", i+1, gsl_matrix_get(iAtA, i, 0));
	}


	gsl_permutation_free(p);
	gsl_matrix_free(A);
	gsl_matrix_free(x);
	gsl_matrix_free(tmp);
	gsl_matrix_free(AtA);
	gsl_matrix_free(tmpAtA);
	gsl_matrix_free(iAtA);
	gsl_matrix_free(At);
	gsl_matrix_free(b);
	free(ccd);
	free(signal);;
	free(wcd);
}

#endif
