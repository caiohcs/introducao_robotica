#include "cabecalho.h"


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
