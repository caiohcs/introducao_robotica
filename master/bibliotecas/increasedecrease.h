#include "./hack.h"

int change_servo(int serial_fd, struct servo *Servo, int novo_pulso)
{
	Servo->pulso = trava(Servo,novo_pulso);
	char *comando = malloc(sizeof(char)*100);
	memset(comando,0,100);
	sprintf(comando,"#%dP%d\r",Servo->num,Servo->pulso);
	enviar_comando(comando,serial_fd);
	printf("%u\n",Servo->pulso);
	free(comando);
	return 0;
}
