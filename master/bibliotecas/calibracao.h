#ifndef CALIBRACAO
#define CALIBRACAO
#include "../pdi/headers/cabecalho.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>


int limiar=10;

/* Calcula e imprime as coordenadas no referencial I, em pixels */

struct CD *cdcamera()
{
	struct pixel matriz[altura][largura];
        struct bloco tijolo[altura*largura];

	map_yuv(matriz);	//Ver em aquisition.c

        unsigned char *prototipo;        
	prototipo = malloc(altura*largura*2);
	image_processing(matriz, tijolo); //Ver em processing.c
	dealocate(matriz, prototipo);
        return print_ballcoord(matriz); //Ver em escrita.c
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


/* Função que serve para calcular H. É esperado que Homografia
seja uma matrix 3x3 */

void calibra(gsl_matrix *Homografia)
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
		gsl_matrix_set(A, i, 6, (-1)*ccd[f].Y*wcd[f].X);
		gsl_matrix_set(A, i, 7, (-1)*ccd[f].Y*wcd[f].Y);
	
		gsl_matrix_set(A, i+1, 3, wcd[f].X);
		gsl_matrix_set(A, i+1, 4, wcd[f].Y);
		gsl_matrix_set(A, i+1, 5, 1);
		gsl_matrix_set(A, i+1, 6, (-1)*ccd[f].X*wcd[f].X);
		gsl_matrix_set(A, i+1, 7, (-1)*ccd[f].X*wcd[f].Y);
	}
	
	
	for (i = 0; i < 38; i+=2) {
		int f=(i/2);
		gsl_matrix_set(b, i, 0, ccd[f].Y);
		gsl_matrix_set(b, i+1, 0, ccd[f].X);
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
	
	/* Criação da matriz de Homografia a partir do vetor x*/

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (i!=2 && j!=2) {
			gsl_matrix_set(Homografia, i, j, gsl_matrix_get(iAtA, i*3 + j, 0));
			} else gsl_matrix_set(Homografia, 2, 2, 1); 
		}
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
