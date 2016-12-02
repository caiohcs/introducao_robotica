#include "../headers/cabecalho.h"


void detect_circle(struct pixel matriz[altura][largura]){
        for (int i = 110; i < 350; i++) {
                for (int j = 340; j < 510; j++) {
			calc_circle(matriz, i, j);
                }
        }
}


void calc_circle(struct pixel matriz[altura][largura], int X, int Y){
        int nbranco = 0;
        int npreto = 0;
        for (int i = X-12; i < X+12; i++) {
                for (int j = Y-12; j < Y+12; j++) {
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
	}
}



void find_round(struct pixel matriz[altura][largura]) {
	int check=0;
	int count=1;
	for (int i=0;i<altura;i++) {
	  for (int j=0;j<largura;j++) {
	     if (matriz[i][j].prox==8) {
	       check=(int)matriz[i][j].prox;
	       while (count <=2) {
	            if (count == 1) {
		      for (int y=-2;y<=2;y++) {
		       	 if ((i-2<0 || j+y <0) || (i+2 >= altura || j+y >= largura)) continue;
		         if (matriz[i-2][j+y].grad==1) check++;
			 if (matriz[i+2][j+y].grad==1) check++;
		      }
		    }else {
		       for (int x=-2;x<=2;x++) {
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
