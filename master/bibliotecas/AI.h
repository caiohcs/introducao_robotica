#ifndef AI
#define AI
#include "../pdi/headers/cabecalho.h"
#include "./tirafoto.c"
#include "./eventfunctions.h"

void ia_map_yuv(struct pixel **matriz)
{
	unsigned char *mapa;
        int fd;
        if((fd = open("tabuleiro_com_pecas.yuv", O_RDWR)) == -1){
                perror("open");
                exit(1);
        }

        struct stat buf;
        if(fstat(fd, &buf) == -1){
                perror("fstat");
                exit(1);
        }

        mapa = mmap(0, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(mapa == MAP_FAILED){
                perror("mmap");
                exit(1);
        }

        alocate(matriz,mapa);
        close(fd);
        munmap(mapa, buf.st_size);
}


int distancia_cds(struct CD cd1, struct CD cd2) 
{
	int distancia = round(pow(pow(cd1.X - cd2.X, 2) + pow(cd1.Y - cd2.Y, 2), 0.5)); // d = sqrt((x-x')^2 + (y-y')^2)
	//printf("%d\n", distancia);
	return distancia;
}

struct CD verifica_vertices(struct CD posicao_atual, struct CD posicoes_auxiliares[16], struct CD alvo, struct servo *ptrservo[5], int serial_fd) {
	int i;
	int menor_distancia = 100000;
	struct CD posicao_proxima;
	/*O for é doidão assim porque ele verifica apenas os vertices das posições auxiliares (3, 7, 11, 15);*/
	/*
		--- --- --- --- ---
	      | 15 | 0 | 1 | 2 | 3 |		Posições auxiliares
		--- ---	--- --- ---
	      | 14 |	       | 4 |
		---		---
	      | 13 |	       | 5 |
		---		---
	      | 12 |	       | 6 |
		--- --- --- ---	---
	      | 11 | 10| 9 | 8 | 7 |
		--- --- --- --- ---	
		
	*/


	for (i = 3; i <16; i+=4) {
		if ((distancia_cds(posicao_atual, posicoes_auxiliares[i]) + distancia_cds(alvo, posicoes_auxiliares[i])) < menor_distancia) {
		menor_distancia = distancia_cds(posicao_atual, posicoes_auxiliares[i]);
		posicao_proxima = posicoes_auxiliares[i];
		} 
	}	
	
	if (posicao_proxima.X == posicao_atual.X && posicao_proxima.Y == posicao_atual.Y) return posicao_proxima;
	
	usleep(2000000);
	cinversa(serial_fd, ptrservo, posicao_proxima.X, posicao_proxima.Y, 9, -70);
		
	return verifica_vertices(posicao_proxima, posicoes_auxiliares, alvo, ptrservo, serial_fd);
 

}



void decisao_trajetoria_jogada(struct Mainwin_var *main_win, struct CD posicao_atual_peca, struct CD centros_hashtag_centimetros[9], int hashtag_status[9], struct servo *ptrservo[5], int serial_fd) {

	int i,j;
	struct CD *posicoes_auxiliares_centimetros;
	struct CD *hashtags_alvos;
	struct CD alvo;
	struct CD posicao_temporaria;
	int menor_distancia_atual = 100000;
	int num_hashtag_disp = 0;	//Número de hashtags que podem receber uma peça
	posicoes_auxiliares_centimetros = cdworld_auxiliares(); // onde está o free?
	
	/*Posicao_temporaria ira armazenar a posicao auxiliar mais proxima do ponto de origem da peça*/

	for (i =0; i < 16; i++) {
		if (distancia_cds(posicao_atual_peca, posicoes_auxiliares_centimetros[i]) < menor_distancia_atual) {
			menor_distancia_atual = distancia_cds(posicao_atual_peca, posicoes_auxiliares_centimetros[i]);
			posicao_temporaria = posicoes_auxiliares_centimetros[i];
			printf("A menor distancia atual é a do hashtag %d\n", i);
		}	
	}
	
	posicao_atual_peca = posicao_temporaria;
	

	/*Manda a peça para a posição auxiliar mais próxima da sua posição inicial */

	usleep(3000000);
	cinversa(serial_fd, ptrservo, posicao_atual_peca.X, posicao_atual_peca.Y, 9, -70);
	printf("\t !!!!!!!!!!!Primeiro deslocamento depois de pegar a peça!!!!!!!!!!!\n");
	XDrawArc(main_win->display, *(main_win->mainwin), *(main_win->gc_preto),
					coordX_pixels(posicao_atual_peca.Y*7, main_win->larg)-5,
					coordY_pixels((-7)*posicao_atual_peca.X, main_win->altw)-5,
					10, 10, 0, 360*64);
	enviar_comandoX(main_win->display);

	/*Verificação de quantos são os hashtags que podem receber peças */

	for (i = 0; i < 9; i++) {
		if (hashtag_status[i] == 0) {
			num_hashtag_disp++;
		}
	}
		
	hashtags_alvos = malloc (sizeof(struct CD)*num_hashtag_disp);
	
	/* Atribui a hashtags_alvos as posições dos hastags livres */
	j=0;
	for (i = 0; i < 9; i++) {
		if (hashtag_status[i] == 0) {
			hashtags_alvos[j]=centros_hashtag_centimetros[i];
			j++;
		}
	}
	
	/*Decide qual dos hashtags livres será o escolhido para a jogada */

	menor_distancia_atual = 100000;
	for (i = 0; i < num_hashtag_disp; i++) {
				if (distancia_cds(posicao_atual_peca, hashtags_alvos[i]) < menor_distancia_atual ) {
					menor_distancia_atual = distancia_cds(posicao_atual_peca, hashtags_alvos[i]);
					alvo = hashtags_alvos[i];
				} 
	}
	
	/*Desenhar o ponto ALVO na tela*/
	
	XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_preto),
                                        coordX_pixels(alvo.Y*7, main_win->larg)-5,
                                        coordY_pixels((-7)*alvo.X, main_win->altw)-5,
                                        10, 10, 0, 360*64);
        enviar_comandoX(main_win->display);











	/*Verifica qual dos vertices está mais próximo da peça e do alvo e manda a peça para la até
	que a nova posição seja igual à atual */

	posicao_atual_peca = verifica_vertices(posicao_atual_peca, posicoes_auxiliares_centimetros, alvo, ptrservo, serial_fd);
	
	/*Agora é feita a verificação de qual é a posição auxiliar mais proxima do alvo, e o braço é
	mandado para la */
	menor_distancia_atual = 100000;
	for ( i =0; i < 16 ; i++) {
		if (distancia_cds(alvo, posicoes_auxiliares_centimetros[i]) < menor_distancia_atual ) {
			posicao_atual_peca = posicoes_auxiliares_centimetros[i];
			menor_distancia_atual = distancia_cds(posicao_atual_peca, posicoes_auxiliares_centimetros[i]);
		}
	}

	usleep(2000000);
        cinversa(serial_fd, ptrservo, posicao_atual_peca.X, posicao_atual_peca.Y, 9, -70);
	XDrawArc(main_win->display, *(main_win->mainwin), *(main_win->gc_preto),
					coordX_pixels(posicao_atual_peca.Y*7, main_win->larg)-5,
					coordY_pixels((-7)*posicao_atual_peca.X, main_win->altw)-5,
					10, 10, 0, 360*64);
	enviar_comandoX(main_win->display);
	usleep(2000000);
	XDrawArc(main_win->display, *(main_win->mainwin), *(main_win->gc_preto),
					coordX_pixels(posicao_atual_peca.Y*7, main_win->larg)-5,
					coordY_pixels((-7)*posicao_atual_peca.X, main_win->altw)-5,
					10, 10, 0, 360*64);
        cinversa(serial_fd, ptrservo, alvo.X, alvo.Y, 9, -70);
	enviar_comandoX(main_win->display);
	
	free(posicoes_auxiliares_centimetros);
	free(hashtags_alvos);
}


void ia(struct Mainwin_var *main_win, struct servo *ptrservo[5], int serial_fd)
{
	struct servo *base = ptrservo[0],
	*ombro = ptrservo[1],
	*cotovelo = ptrservo[2],
	*punho = ptrservo[3],
	*garra = ptrservo[4];
	
	/*
	 *	Decidi que pra facilitar, o time verde é o 1 e vai ser sempre o que o braço vai usar pra jogar.
	 *	Então o time vermelho (2) é o que o usuário vai usar pra jogar contra o braço.
	 *	Também decidi que o time verde sempre começa do lado direito, enquanto o vermelho sempre começa do lado esquerdo.
	 *	Assim, chamei as bolinhas do lado direito de bolinhas time 1 e as bolinhas do lado esquerdo de bolinhas do time 2.
	 */


	//foto_tabuleiro_com_pecas();	essa funcao tira a foto do tabuleiro para detectar as peças e fazer as jogadas
	
	struct pixel pixteam1; //verde
        pixteam1.luma = 182;
        pixteam1.cb = 104;
        pixteam1.cr = 115;
        struct pixel pixteam2; //vermelho
        pixteam2.luma = 144;
        pixteam2.cb = 122;
        pixteam2.cr = 159;
	unsigned char *prototipo = malloc(largura*altura*2);

	struct pixel **matriz = malloc(altura*sizeof(struct pixel*));
	int i, j;
	for (i = 0; i < altura; i++)
		matriz[i] = malloc(largura*sizeof(struct pixel));
	
	ia_map_yuv(matriz);
        generate_teams(matriz, pixteam1, pixteam2);	//Classifica em times
	generate_prox_teams(matriz);	//funciona com a de baixo para limpar "falsos" times.
	shrink_teams(matriz, 5); 	
	/*
	 * caso detecte mais ou menos peças do que deveria, ajustar os valores dos filtros swell e shrink, nao esquecer de dar generate_prox_teams
	 */
	//swell_teams(matriz, 4);
	struct coordenadas **centros_teams = detect_regiaoteam(matriz); //Obtém o centro de cada peça dos dois times;
	dealocate(matriz, prototipo);
	escrita("teams.jpg", prototipo); //tirou o warning do make
	free(prototipo);
	
	struct CD team1[5];
        struct CD team2[5];
        struct CD *centros_posicoes_vazias_pixels;
        struct CD *centros_posicoes_vazias_centimetros;
        struct CD centros_hashtag_pixels[9];
        struct CD centros_hashtag_centimetros[9];
	int hashtag_status[9];	// status = 0 -> posicao vazia, status = 1 -> posicao ocupada por team1, status = 2 -> posicao ocupada por team2
	int bolinhas_team1_status[5];
        struct CD centros_bolinhas_team1_pixels[5];	//Essa variável contém as posições inicias das bolinhas do time 1
        struct CD centros_bolinhas_team1_centimetros[5];
	int bolinhas_team2_status[5];
        struct CD centros_bolinhas_team2_pixels[5];
        struct CD centros_bolinhas_team2_centimetros[5];

	centros_posicoes_vazias_pixels = cdcamera();
	centros_posicoes_vazias_centimetros = cdworld();

	/*Variavel usada no calculo da trajetoria */
	struct CD posicao_atual_peca;

	/*Pode usar isso aqui pra saber o tamanho de centros_teams[0] e [1], para não precisar usar 5*/
	int center_counting_1, center_couunting_2;
	center_counting_1 = sizeof(centros_teams[0])/sizeof(centros_teams[0][0]);
	center_counting_1 = sizeof(centros_teams[1])/sizeof(centros_teams[1][0]);
	



	for (i = 0; i < 5; i++) {
		team1[i].X = centros_teams[0][i].X;
		team1[i].Y = centros_teams[0][i].Y;
		team2[i].X = centros_teams[1][i].X;
		team2[i].Y = centros_teams[1][i].Y;
	}
	free(centros_teams[0]);	// centros_teams não é mais necessário, pois já passou seus valores para team1 e team2
	free(centros_teams[1]);
	free(centros_teams);

	for (i = 0; i < 9; i++) {
		centros_hashtag_pixels[i].X = centros_posicoes_vazias_pixels[i].X;
		centros_hashtag_pixels[i].Y = centros_posicoes_vazias_pixels[i].Y;
		centros_hashtag_centimetros[i].X = centros_posicoes_vazias_centimetros[i].X;
		centros_hashtag_centimetros[i].Y = centros_posicoes_vazias_centimetros[i].Y;
	}

	for (i = 0; i < 5; i++) {
		centros_bolinhas_team1_pixels[i].X = centros_posicoes_vazias_pixels[i+9].X;
		centros_bolinhas_team1_pixels[i].Y = centros_posicoes_vazias_pixels[i+9].Y;
		centros_bolinhas_team1_centimetros[i].X = centros_posicoes_vazias_centimetros[i+9].X;
		centros_bolinhas_team1_centimetros[i].Y = centros_posicoes_vazias_centimetros[i+9].Y;
	}
	
	for (i = 0; i < 5; i++) {
		centros_bolinhas_team2_pixels[i].X = centros_posicoes_vazias_pixels[i+9+5].X;
		centros_bolinhas_team2_pixels[i].Y = centros_posicoes_vazias_pixels[i+9+5].Y;
		centros_bolinhas_team2_centimetros[i].X = centros_posicoes_vazias_centimetros[i+9+5].X;
		centros_bolinhas_team2_centimetros[i].Y = centros_posicoes_vazias_centimetros[i+9+5].Y;
	}
	free(centros_posicoes_vazias_pixels); /* centros_posicoes_vazias_pixels não é mais necessário, 
						pois já passou seus valores para centros_hashtag_pixels e centros_bolinhas_team1_pixels*/
	free(centros_posicoes_vazias_centimetros); /* centros_posicoes_vazias_centimetros não é mais necessário, 
						pois já passou seus valores para centros_hashtag_centimetros e centros_bolinhas_team1_centimetros*/
	
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 9; j++) {
				hashtag_status[j] = 0;
		}
	}
	
	/*Verifica se as peças estão perto o suficiente para ocuparem os hashtags*/
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 9; j++) {
			if (distancia_cds(team1[i], centros_hashtag_pixels[j]) < 15) {
				hashtag_status[j] = 1;
			} else if (distancia_cds(team2[i], centros_hashtag_pixels[j]) < 15) {
				hashtag_status[j] = 2;
			}
		}
	}

	for (i = 0; i < 5; i++) {
		bolinhas_team1_status[i] = 0;
		bolinhas_team2_status[i] = 0;
	}

	/*Diz para bolinhas_team_1 e 2 se uma peça de algum dos times está em uma das posições iniciais */
	
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (distancia_cds(team1[i], centros_bolinhas_team1_pixels[j]) < 15) {
				bolinhas_team1_status[j] = 1;
			} else if (distancia_cds(team2[i], centros_bolinhas_team1_pixels[j]) < 15) {
				bolinhas_team1_status[j] = 2;
			}
			if (distancia_cds(team1[i], centros_bolinhas_team2_pixels[j]) < 15) {
				bolinhas_team2_status[j] = 1;
			} else if (distancia_cds(team2[i], centros_bolinhas_team2_pixels[j]) < 15) {
				bolinhas_team2_status[j] = 2;
			}
		}
	}

	for (i = 0; i < 9; i++)
		printf("Hashtag %d: %d\n", i+1, hashtag_status[i]);

	for (i = 0; i < 5; i++)
		printf("Bolinhas t1 %d: %d\n", i+1, bolinhas_team1_status[i]);
	
	for (i = 0; i < 5; i++)
		printf("Bolinhas t2 %d: %d\n", i+1, bolinhas_team2_status[i]);

/*
	//debug pixels
	printf("\n\nTeam1:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, team1[i].X, team1[i].Y);
	}
	printf("\n\nTeam2:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, team2[i].X, team2[i].Y);
	}
	printf("\n\nCentros hashtag:\n");
	for (i = 0; i < 9; i++) {
		printf("%d %f %f\n", i, centros_hashtag_pixels[i].X, centros_hashtag_pixels[i].Y);
	}
	printf("\n\nCentros bolinhas team1:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, centros_bolinhas_team1_pixels[i].X, centros_bolinhas_team1_pixels[i].Y);
	}

	printf("\n\nCentros bolinhas team2:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, centros_bolinhas_team2_pixels[i].X, centros_bolinhas_team2_pixels[i].Y);
	}

	//debug centimetros
	printf("\n\nCentros hashtag:\n");
	for (i = 0; i < 9; i++) {
		printf("%d %f %f\n", i, centros_hashtag_centimetros[i].X, centros_hashtag_centimetros[i].Y);
	}
	printf("\n\nCentros bolinhas team1:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, centros_bolinhas_team1_centimetros[i].X, centros_bolinhas_team1_centimetros[i].Y);
	}

	printf("\n\nCentros bolinhas team2:\n");
	for (i = 0; i < 5; i++) {
		printf("%d %f %f\n", i, centros_bolinhas_team2_centimetros[i].X, centros_bolinhas_team2_centimetros[i].Y);
	}
*/

	for (i = 0; i < 9; i++) {
		if (hashtag_status[i] == 1) {
			XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_team1),
							coordX_pixels(centros_hashtag_centimetros[i].Y*7, main_win->larg)-5,
							coordY_pixels((-7)*centros_hashtag_centimetros[i].X, main_win->altw)-5,
							10, 10, 0, 360*64);
		} else if (hashtag_status[i] == 2) {
			XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_team2),
							coordX_pixels(centros_hashtag_centimetros[i].Y*7, main_win->larg)-5,
							coordY_pixels((-7)*centros_hashtag_centimetros[i].X, main_win->altw)-5,
							10, 10, 0, 360*64);
		}
	}

	for (i = 0; i < 5; i++) {
		if (bolinhas_team1_status[i] == 1) {
			XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_team1),
							coordX_pixels(centros_bolinhas_team1_centimetros[i].Y*7, main_win->larg)-5,
							coordY_pixels((-7)*centros_bolinhas_team1_centimetros[i].X, main_win->altw)-5,
							10, 10, 0, 360*64);
		} else if (bolinhas_team1_status[i] == 2) {
			XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_team2),
							coordX_pixels(centros_bolinhas_team1_centimetros[i].Y*7, main_win->larg)-5,
							coordY_pixels((-7)*centros_bolinhas_team1_centimetros[i].X, main_win->altw)-5,
							10, 10, 0, 360*64);
		}
	}

	for (i = 0; i < 5; i++) {
		if (bolinhas_team2_status[i] == 1) {
			XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_team1),
							coordX_pixels(centros_bolinhas_team2_centimetros[i].Y*7, main_win->larg)-5,
							coordY_pixels((-7)*centros_bolinhas_team2_centimetros[i].X, main_win->altw)-5,
							10, 10, 0, 360*64);
		} else if (bolinhas_team2_status[i] == 2) {
			XFillArc(main_win->display, *(main_win->mainwin), *(main_win->gc_team2),
							coordX_pixels(centros_bolinhas_team2_centimetros[i].Y*7, main_win->larg)-5,
							coordY_pixels((-7)*centros_bolinhas_team2_centimetros[i].X, main_win->altw)-5,
							10, 10, 0, 360*64);
		}
	}
	enviar_comandoX(main_win->display);
		
	/*Tranformarei isso em função */
	
	for (i = 0; i < 5; i++) {	// Esse laço vai ser o robô procurando uma peça disponível em bolinhas time 1 para pegar e jogar em algum canto
		if (bolinhas_team1_status[i] == 1) {
			usleep(3000000);
			cinversa(serial_fd, ptrservo, centros_bolinhas_team1_centimetros[i].X, centros_bolinhas_team1_centimetros[i].Y, 9, -70);			
			change_servo(serial_fd, garra, 2000);	//Comando para fechar a garra;	
			posicao_atual_peca=centros_bolinhas_team1_centimetros[i];
			break;
		}
	}
	
	XDrawArc(main_win->display, *(main_win->mainwin), *(main_win->gc_preto),
					coordX_pixels(posicao_atual_peca.Y*7, main_win->larg)-5,
					coordY_pixels((-7)*posicao_atual_peca.X, main_win->altw)-5,
					10, 10, 0, 360*64);

	enviar_comandoX(main_win->display);

	decisao_trajetoria_jogada(main_win, posicao_atual_peca, centros_hashtag_centimetros, hashtag_status, ptrservo, serial_fd);
	
	/*Essas variaveis são usadas no calculo da trajetória simples */
	/*Struct CD auxiliar para guardar a distância minima
	entre a posição atual da peça e uma das posições auxiliares*/
	
//	Parei aqui e fui dormir.

	for (i = 0; i < altura; i++)
		free(matriz[i]);
	free(matriz);
}




#endif 


