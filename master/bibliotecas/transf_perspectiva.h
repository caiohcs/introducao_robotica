#ifndef TRANSF_PERSPECTIVA
#define TRANSF_PERSPECTIVA
#include "../pdi/headers/cabecalho.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>



/* O valor de u e v que essa função recebe é dado por Y e X de um struct CD */

struct CD pixel_to_world(int u, int v, gsl_matrix *Homografia) {
	struct CD coord_world;
	/* Variáveis para desobfuscação */
	double a = (gsl_matrix_get(Homografia, 2, 0) * v - gsl_matrix_get(Homografia, 1, 0));
	double b = (gsl_matrix_get(Homografia, 2, 0) * u - gsl_matrix_get(Homografia, 0, 0));	
	double c = (u-gsl_matrix_get(Homografia, 0, 2));
	double d = (gsl_matrix_get(Homografia, 1, 2) - v);
	double e = (gsl_matrix_get(Homografia, 0, 1) - gsl_matrix_get(Homografia, 2, 1) * u);
	double f = (gsl_matrix_get(Homografia, 2, 1) * v - gsl_matrix_get(Homografia, 1, 1));

	double g = (gsl_matrix_get(Homografia, 0, 1) - gsl_matrix_get(Homografia, 2, 1) * u);
	
	coord_world.Y = (c * a + d * b)/(a * e + b * f);
	coord_world.X = (coord_world.Y * g + gsl_matrix_get(Homografia, 0, 2) - u)/b;

	return coord_world;
}	

#endif
