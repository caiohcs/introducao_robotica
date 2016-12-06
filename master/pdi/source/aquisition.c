#include "../headers/cabecalho.h"


void map_yuv(struct pixel **matriz) {

	unsigned char *mapa;	
	int fd;
        if((fd = open("myimage.yuv", O_RDWR)) == -1){
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
