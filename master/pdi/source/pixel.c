#include "../headers/cabecalho.h"


/*Verifica se um pixel gerador do bloco block tem 
valor de gradiente acima do limiar estabelecido.*/

struct pixel gradient_check(struct bloco block){
 	struct pixel pixel_temp;	
	pixel_temp.cb=128;
	pixel_temp.cr=128;
	float grad_luma;
	grad_luma = (float)(pow((block.i1j1-block.ij),2) + pow((block.ij1-block.i1j),2))/2.0*delta_x;
	
	if (grad_luma>=limiar) {
	   pixel_temp.luma=255;
	   pixel_temp.grad=1;
	}  else {
	   pixel_temp.luma=0;
           pixel_temp.grad=0;
	   }
	return pixel_temp;
}

/*Ler gradiente_check*/

struct pixel edge_detection(struct bloco block){
	struct pixel gradiente;		
	gradiente=gradient_check(block);	
	return gradiente;
}

void generate_grad(struct pixel grad[altura][largura], struct bloco block[altura*largura]){
	for (int i=0;i<altura;i++){
	    for (int j=0; j<largura; j++){
	        grad[i][j] = edge_detection(block[i*largura + j]);
		
            }
	
	}

}


/*
 *	Para cada pixel i,j; olha os oito pixels ao redor (i-1 j-1, i-1 j, etc)
 *	e diz quantos fazem parte da borda (grad == 1). Não devemos olhar o x=0,y=0.
 */

void generate_prox(struct pixel matriz[altura][largura]){
	for (int i = 0;i < altura; i++) {
	   for (int j = 0; j < largura; j++) {
	      matriz[i][j].prox = 0;
		for (int x = -1; x <= 1; x++) {
		   for (int y = -1; y <= 1; y++) {
		      if ((i+x<0 || j+y <0) || (i+x >= altura || j+y >= largura)) continue;
			if (x != 0 || y != 0) {
			  if (matriz[i+x][j+y].grad == 1) {
			  matriz[i][j].prox++;
			}
		      }
	           }
		}
	   }
	}

}


struct pixel lumcbcr_medium(struct pixel matriz[altura][largura], int X, int Y, int R){
	struct pixel pixeltmp;
	int luma = 0;
	int cb = 0;
	int cr = 0;
	int npixels = 0;
	for (int i=0; i < altura;i++) {
		for (int j=0; j < largura; j++) {
			if (pow(i-X,2) + pow(j-Y,2) <= pow(R,2)) {
				luma += matriz[i][j].luma;
				cb += matriz[i][j].cb;
				cr += matriz[i][j].cr;
				npixels++;
			}
		}
	}
	pixeltmp.cb = cb/npixels;
	pixeltmp.cr = cr/npixels;
	pixeltmp.luma = luma/npixels;
	return pixeltmp;
}



/*
 * Operador de dilatação. Fecha buracos.
 * Se mais do que nprox forem parte do grad, faça
 * esse pixel parte do grad.
 */
void swell(struct pixel matriz[altura][largura], int nprox) {
	for (int i=0;i < altura;i++) {
	  for (int j=0;j < largura; j++) {
	    if(matriz[i][j].prox>=nprox) {
	      matriz[i][j].grad=1;
	      matriz[i][j].luma=255;
	    }
	  }
	}
}


/*
 * Operador de erosão. Fecha buracos.
 * Se menos do que nprox forem parte do grad, faça
 * esse pixel não ser parte do grad.
 */
void shrink(struct pixel matriz[altura][largura], int nprox) {
	for (int i=0;i < altura;i++) {
	  for (int j=0;j < largura; j++) {
	    if(matriz[i][j].prox<=nprox) {
	      matriz[i][j].grad=0;
	      matriz[i][j].luma=0;
	    }
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


