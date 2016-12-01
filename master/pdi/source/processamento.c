#include "../headers/cabecalho.h"

int limiar = 0;

int main(int argc, char *argv[]){
	
	/*O primeiro argumento é o nome do próprio programa. O segundo é o nome da imagem.
	O terceiro é o nome do arquivo de saída. Todos são em .yuv*/
 	
	struct pixel matriz[altura][largura];
        struct bloco tijolo[altura*largura];

	if (argc<3){
	fprintf (stderr, "usage: %s <file>\n", argv[0]);
                return 1;
	}
	unsigned char *mapa;

	/*Depois de mapeado o arquivo, o descritor pode
	ser encerrado que o processo não perderá acesso
	ao arquivo.*/

	int fd;
	if((fd = open(argv[1], O_RDWR)) == -1){
		perror("open");
		exit(1);
	}
	
	/* O fstat retorna informações acerca do arquivo. 
	   O struct stat contém informações sobre o arquivo. */

	struct stat buf;
	if(fstat(fd, &buf) == -1){
		perror("fstat");
		exit(1);
	}

	/* Inicialização de um nmap chamado mapa para acessar a área da memória ocupada pelo arquivo de imagem.*/
	
	printf("Valor de fd: %d\n", fd);
	mapa = mmap(0, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(mapa == MAP_FAILED){
		perror("mmap");
		exit(1);
	}
	
	//unsigned char *nome;
	//nome = malloc(30);
	char nome[30]={};
	strcpy(nome, argv[2]);
	threshold(matriz,mapa,tijolo,nome);
	
	//printf("Ponto 1\n");
	//alocate(matriz,mapa);
	//printf("Ponto 2\n");
	//printf("Ponto 3\n");
	//array_bloco(tijolo, matriz);
	//printf("Ponto 4\n");
	//generate_grad(matriz, tijolo);
	//printf("Ponto 5\n");
		
	//unsigned char *prototipo;
	//crote = malloc (buf.st_size);
	//dealocate(matriz, crote); 


	munmap(mapa, buf.st_size);
	close(fd);

	return 0;
}
