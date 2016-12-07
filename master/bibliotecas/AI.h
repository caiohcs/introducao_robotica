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
	struct coordenadas **centros_teams = malloc(sizeof(struct coordenadas*)*2);
	int i, j;
	for (i = 0; i < altura; i++)
		matriz[i] = malloc(largura*sizeof(struct pixel));
	
	ia_map_yuv(matriz);
        generate_teams(matriz, pixteam1, pixteam2);
	generate_prox_teams(matriz);
	shrink_teams(matriz, 5); 
	/*
	 * caso detecte mais ou menos peças do que deveria, ajustar os valores dos filtros swell e shrink, nao esquecer de dar generate_prox_teams
	 */
	//swell_teams(matriz, 4);
	centros_teams = detect_regiaoteam(matriz);
	dealocate(matriz, prototipo);
	escrita("teams.jpg", prototipo);
	free(prototipo);
	
	struct CD team1[5];
        struct CD team2[5];
        struct CD *centros_posicoes_vazias_pixels;
        struct CD *centros_posicoes_vazias_centimetros;
        struct CD centros_hashtag_pixels[9];
        struct CD centros_hashtag_centimetros[9];
	int hashtag_status[9];	// status = 0 -> posicao vazia, status = 1 -> posicao ocupada por team1, status = 2 -> posicao ocupada por team2
	int bolinhas_team1_status[5];
        struct CD centros_bolinhas_team1_pixels[5];
        struct CD centros_bolinhas_team1_centimetros[5];
	int bolinhas_team2_status[5];
        struct CD centros_bolinhas_team2_pixels[5];
        struct CD centros_bolinhas_team2_centimetros[5];

	centros_posicoes_vazias_pixels = cdcamera();
	centros_posicoes_vazias_centimetros = cdworld();

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

	for (i = 0; i < 5; i++) {	// Esse laço vai ser o robô procurando uma peça disponível em bolinhas time 1 para pegar e jogar em algum canto
		if (bolinhas_team1_status[i] == 1) {
			usleep(2000000);
			cinversa(serial_fd, ptrservo, centros_bolinhas_team1_centimetros[i].X, centros_bolinhas_team1_centimetros[i].Y, 9, -70);
			break;
		}
	}

	//	Parei aqui e fui dormir.

	for (i = 0; i < altura; i++)
		free(matriz[i]);
	free(matriz);
}




#endif 


