#include "../headers/cabecalho.h"

void threshold (struct pixel matriz[altura][largura], unsigned char *mapa, struct bloco block[altura*largura], unsigned char *arquivo) {
	limiar=10;
	unsigned char *prototipo;
        prototipo = malloc (altura*largura*2);

	for (int i=0;i<10;i++) {
	    alocate(matriz,mapa);
	    array_bloco(block, matriz);
	    generate_grad(matriz, block);	
	    generate_prox(matriz);
	    dealocate(matriz, prototipo);
	    printf("nome do arquivo: %s\n",arquivo);
	    arquivo[0]='0' + i; //Cheat de converter o inteiro para char
	    printf("Nome do arquivo modificado: %s\n", arquivo);
	    escrita(arquivo, prototipo);
	    printf("Valor do limiar: %d\n", limiar);
	    limiar=limiar+10;
	}

}



void escrita (unsigned char *arquivo, unsigned char* matriz_temp) {
	int jpgfile;
        if((jpgfile = open(arquivo, O_WRONLY | O_CREAT, 0660)) < 0){
            perror("open");
            exit(1);

        }

        write(jpgfile, matriz_temp, altura*largura*2);
        close(jpgfile);
}
