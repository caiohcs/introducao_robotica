#include <stdio.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

int main(){
	double matriz[]={ 1, 0.6, 0.0,
			 0.0, 1.5, 1.0,
			 0.0, 1.0, 1.0 };
	double inversa[9];
	int * signal;
	int i, j;
	signal = malloc (sizeof(int)*3);
	
	gsl_matrix_view m = gsl_matrix_view_array (matriz, 3, 3);
	gsl_matrix_view inv = gsl_matrix_view_array (inversa, 3, 3);
	gsl_permutation *p = gsl_permutation_alloc(3);

	gsl_linalg_LU_decomp (&m.matrix, p, signal);
	gsl_linalg_LU_invert (&m.matrix, p, &inv.matrix);

	printf("The inverse is\n");
        for (i = 0; i < 3; ++i) {
         	for (j = 0; j < 3; ++j) {
        		printf(j==2?"%6.3f\n":"%6.3f ",gsl_matrix_get(&inv.matrix,i,j));
        
		}
	}
	
	free(signal);	
	gsl_permutation_free (p);
    	return 0;
}
