#include "../headers/cabecalho.h"


void detect_circle(struct pixel matriz[altura][largura]){
        int i, j;
	for (i = altmin; i < altmax; i++) {
                for (j = largmin; j < largmax; j++) {
			matriz[i][j].circle = 0;
			calc_circle(matriz, i, j);
                }
        }
}


/*Essa função "calc_circle" pinta o píxel se ele
fizer parte das bolinhas de posição inicial*/

void calc_circle(struct pixel matriz[altura][largura], int X, int Y){
        int i, j;
	int nbranco = 0;
        int npreto = 0;
        for (i = X-9; i < X+9; i++) {
                for (j = Y-12; j < Y+12; j++) {
                        if (pow(i-X, 2) + pow(j-Y, 2) <= pow(8, 2)) {
                                if (matriz[i][j].grad == 1) nbranco++;
                                if (matriz[i][j].grad == 0) npreto++;
                        }
                }
        }

        if (abs(nbranco-50) < 12 && abs(npreto-150) < 15 && matriz[X][Y].prox >= 8) {
                matriz[X][Y].luma = 116;
                matriz[X][Y].cb = 191;
                matriz[X][Y].cr = 80;
		matriz[X][Y].circle = 1;
	}
}


/* Função que diz quantos píxeis bolinha estão ao redor
 do pixel analisado.*/

void generate_proxcircle(struct pixel matriz[altura][largura]){
        int i, j, x, y;
	for (i = altmin; i < altmax; i++) {
           for (j = largmin; j < largmax; j++) {
              matriz[i][j].proxcircle = 0;
                for (x = -1; x <= 1; x++) {
                   for (y = -1; y <= 1; y++) {
                        if (x != 0 || y != 0) {
			  if (matriz[i+x][j+y].circle == 1) {
                            matriz[i][j].proxcircle++;
			}
                      }
                   }
                }
           }
        }
}

void swell_circle(struct pixel matriz[altura][largura], int nprox) {
        int i, j;
	for (i = altmin; i < altmax; i++) {
           for (j = largmin; j < largmax; j++) {
            if(matriz[i][j].proxcircle>=nprox) {
              	matriz[i][j].circle=1;
                matriz[i][j].luma = 116;
                matriz[i][j].cb = 191;
                matriz[i][j].cr = 80;
            }
          }
        }
}

void shrink_circle(struct pixel matriz[altura][largura], int nprox) {
        int i, j;
	for (i = altmin; i < altmax; i++) {
           for (j = largmin; j < largmax; j++) {
            if(matriz[i][j].proxcircle<=nprox) {
              matriz[i][j].circle=0;
              matriz[i][j].luma=0;
              matriz[i][j].cb=128;
              matriz[i][j].cr=128;
	    }
          }
        }
}

void find_round(struct pixel matriz[altura][largura]) {
	int check=0;
	int count=1;
	int i, j, x, y;
	for (i=0;i<altura;i++) {
	  for (j=0;j<largura;j++) {
	     if (matriz[i][j].prox==8) {
	       check=(int)matriz[i][j].prox;
	       while (count <=2) {
	            if (count == 1) {
		      for (y=-2;y<=2;y++) {
		       	 if ((i-2<0 || j+y <0) || (i+2 >= altura || j+y >= largura)) continue;
		         if (matriz[i-2][j+y].grad==1) check++;
			 if (matriz[i+2][j+y].grad==1) check++;
		      }
		    }else {
		       for (x=-2;x<=2;x++) {
		       	  if ((i+x<0 || j-2 <0) || (i+x >= altura || j+2 >= largura)) continue;
		          if (matriz[i+x][j-2].grad==1) check++;
			  if (matriz[i+x][j+2].grad==1) check++;    
		       } 
		     }
	        count++;	
		} 
	     }
	   }
	 }
}
