#include "../headers/cabecalho.h"

void paint_surround(struct pixel matriz[altura][largura]){
	int gray_count=0;
	raio=3;
	for (int i=110;i<350;i++) {
	    for (int j=340;j<510;j++) { 
            if (matriz[i][j].prox==8) {
	       for (int x=i-raio;x<=i+raio;x++) {
		  for (int y=j-raio;y<=j+raio;y++) {
 			if ((x<0 || y <0) || (x >= altura || y >= largura)) continue;
		          if ((pow(i-x,2) + pow(j-y,2)) <= pow(raio,2)) {
			  if (matriz[x][y].luma==128 && matriz[x][y].grad==1) gray_count++;  
		  	  } 
		  }	
	       }   
	  	 if (gray_count>=10) {
		 /*for (int x=i-raio;x<=i+raio;x++) {
	       	   for (int y=j-raio;y<=j+raio;y++) { 
		      if ((x<0 || y <0) || (x >= altura || y >= largura)) continue;
		       if ((pow(i-x,2) + pow(j-y,2)) <= pow(raio,2)) {
		  	matriz[x][y].luma=128;	
		       } 
		   } 
                 }*/matriz[i][j].luma=128;	
	         } else matriz[i][j].luma=255;
	    } 
	   gray_count=0;
	 }  
        }

	/*for (int i=0; i<altura; i++) {
	   for (int j=0;j<largura; j++) {
		if (matriz[i][j].luma!=128) matriz[i][j].luma=0;  
	   }
	}*/
}


void find_surround(struct pixel matriz[altura][largura],int raio) {
	int black_count=0;
	int white_count=0;
	for (int i=110;i<350;i++) {
	    for (int j=340;j<510;j++) { 
            if (matriz[i][j].prox==8) {
	       for (int x=i-raio;x<=i+raio;x++) {
		  for (int y=j-raio;y<=j+raio;y++) {
 			if ((x<0 || y <0) || (x >= altura || y >= largura)) continue;
		          if ((pow(i-x,2) + pow(j-y,2)) <= pow(raio,2)) {
			  if (matriz[x][y].grad==1) white_count++;
			  if (matriz[x][y].grad==0) black_count++;    
		  } 
		}	
	     } 
	   }  
	  /*12 e 15 são os valores da metade do intervalo de aceitação do branco e preto*/
	  if ( abs(white_count-50) < 12 && abs(black_count - 150) < 15 ) matriz[i][j].luma=128;
	  black_count=0;
	  white_count=0;
	  }
	 }
	paint_surround(matriz);
}

