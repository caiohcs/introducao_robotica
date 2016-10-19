#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "./servo.h"
#include <unistd.h>

#ifndef HACK_H
#define HACK_H

int enviar_comando(char *P, int fd)
{
	if ( (write(fd,P,strlen(P))==-1) ){ // lembre-se: comandos válidos são da forma: "#<canal>P<posição><cr>", cr = \r
		printf("Erro ao escrever");
		return -1;
	}
	return 0;
}

int abrir_porta()
{
	int fd;
	if ( (fd = open("/dev/ttyS0",O_RDWR) ) == -1)
	{
		printf("Erro ao abrir portas\n");
		return -1;
	}
	return fd; 
}

void fechar_porta(int fd)
{
	if ( (close(fd) == -1) )
		printf("Erro ao fechar portas\n");
}

int configurar_porta(int fd)
{
        struct termios config; 
        tcgetattr(fd, &config); 
        cfsetispeed(&config,B115200); 
        cfsetospeed(&config,B115200); 
        config.c_iflag &= ~(IXON | IXANY | IXOFF); 
        config.c_cc[5] = VINTR; 
        config.c_cc[6] = VINTR; 
        config.c_cflag &= ~(CRTSCTS | CS8 | CSTOPB | PARENB); 
        config.c_cflag |= (CS8 | CREAD | CLOCAL); 
        tcflush(fd,TCIFLUSH); 
	if ( (tcsetattr(fd, TCSANOW, &config)==-1) ){
		printf("Erro ao configurar\n");
		return -1;
	}
	return 0;
}

unsigned int trava(struct servo *Servo, unsigned int pos)
{
	if(pos > Servo->pulsomax)
		return Servo->pulsomax;
	if(pos < Servo->pulsomin)
		return Servo->pulsomin;

	return pos;

}

#endif
