#include "../headers/cabecalho.h"

void image_processing(struct pixel matriz[altura][largura], struct bloco block[altura*largura]) {

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

}
