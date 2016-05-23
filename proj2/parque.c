#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>

int n_lugares;
int t_abertura;
int t;
int ocupados;


void* tcontroller(void *arg);/*
{

	int closed = 0;
	
	int fd=open("FIFON",O_RDONLY);

	while(read(...) != 0)
	{
	
		if(vehicle.id == 0)
			closed = 1;
	
	}
	
	close(fd);

	return NULL;

}*/


int main(int argc, char *argv[])
{
	if (argc != 3) 
	{
		printf("Usage: gerador <N_LUGARES> <T_ABERTURA>\n");
		return 1;
	}
	return 0;

	n_lugares = atoi(argv[1]);
	t_abertura = atoi(argv[2]);
	t = 0;
	ocupados = 0;

	mkfifo("FIFON",0600);

	pthread_t tidN;
	
	pthread_create(&tidN,NULL,tcontroller,"N");

	//int fdN=open("FIFON",O_WRONLY);
	sleep(t_abertura);
/*
	//vehicle = vehicle.stop(...0...);
	write(fdN,&vehicle,sizeof("structviatura?"));
	close(fdN);
	*/

	pthread_t entradaN, entradaW, entradaE, entradaS;
	char w = 'W', n = 'N', e = 'E', s = 'S';

	pthread_create(&entradaN, NULL, tcontroller, &n);
	pthread_create(&entradaW, NULL, tcontroller, &w);
	pthread_create(&entradaE, NULL, tcontroller, &e);
	pthread_create(&entradaS, NULL, tcontroller, &s);

	return 0;

}





