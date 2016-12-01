#include "../headers/cabecalho.h"


/*Verifica se um pixel gerador do bloco block tem 
valor de gradiente acima do limiar estabelecido.*/

unsigned char gradient_check(struct bloco block){
	float grad_luma;
	grad_luma = (float)(pow((block.i1j1-block.ij),2) + pow((block.ij1-block.i1j),2))/2.0*delta_x;
	
	if (grad_luma>=limiar) return 255;
	
	return 0;
}

/*Ler gradiente_check*/

struct pixel edge_detection(struct bloco block){
	struct pixel gradiente;		
	if ((gradiente.luma=gradient_check(block))==255){
		gradiente.grad = 1;
	}
	gradiente.cb=128;
	gradiente.cr=128;   	
	
	return gradiente;
}

void generate_grad(struct pixel grad[altura][largura], struct bloco block[altura*largura]){
	for (int i=0;i<altura;i++){
	    for (int j=0; j<largura; j++){
	        grad[i][j] = edge_detection(block[i*largura + j]);
		
            }
	
	}

}


struct bloco create_block(struct pixel matriz[altura][largura], int k, int p){
	struct bloco block;
	
	block.ij=matriz[k][p].luma;
	block.ij1=matriz[k][p+1].luma;
	block.i1j=matriz[k+1][p].luma;
	block.i1j1=matriz[k+1][p+1].luma;

	return block;
}


/*Criação de um array de blocos para poder gerar a matriz de pixels gradiente
visto que a função de detecção apenas aceita um bloco por vez */


void array_bloco(struct bloco block[altura*largura], struct pixel matriz[altura][largura]){
	for (int i=0; i<altura;i++){
	    for(int j=0;j<largura;j++){
	       block[i*largura+j]=create_block(matriz, i,j);
            }
	}
}

void alocate(struct pixel matriz[altura][largura], unsigned char *mmap){
	for (int i=0;i<altura;i=i+1){
		for (int j=0;j<largura;j=j+1){
		    matriz[i][j].luma = mmap[largura*2*i + 2*j];
			if (j%2 == 0){
		            matriz[i][j].cb = mmap[(largura*2)*i + (2*j +1)];
		            matriz[i][j].cr = mmap[(largura*2)*i + (2*j +3)];
		    } else{ 
			matriz[i][j].cb = mmap[(largura*2)*i + (2*j-1)];
			matriz[i][j].cr = mmap[(largura*2)*i + (2*j+1)];
 		      }
		
		}
	}
}


void dealocate(struct pixel gradiente[altura][largura], unsigned char *mmap){
	for (int i=0;i<altura;i=i+1) {
             for (int j=0;j<largura;j=j+1) {
                     mmap[largura*2*i + 2*j] = gradiente[i][j].luma;
                        if (j%2 == 0) {
                            mmap[largura*2*i + (2*j +1)] = gradiente[i][j].cb;
                            mmap[largura*2*i + (2*j +3)] = gradiente[i][j].cr;                    			
			} else{
                          mmap[largura*2*i + (2*j-1)] = gradiente[i][j].cb;                        		    
			  mmap[largura*2*i + (2*j+1)] = gradiente[i][j].cr; 
                          }  
              }
         }
}


