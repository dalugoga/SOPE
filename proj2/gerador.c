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
#include <math.h>

int *nr;
double nts;
int id = 1;
double u_relogio;
double t_global;

typedef struct
{
	char gate;
	int id;
	int t_park;
	char fifo[10];
	double t_gen;
}Vehicle;

//
Vehicle* create_vehicle(int vehicle_id)
{

	Vehicle *v = (Vehicle*) malloc(sizeof(Vehicle));
	int r = rand() % 3;

	switch(r)
	{
		case 0:
			v->gate = 'N';
			break;
		case 1:
			v->gate = 'W';
			break;
		case 2:
			v->gate = 'E';
			break;
		case 3:
			v->gate = 'S';
			break;
	}

	v->t_park = (rand() % 10 +1)* u_relogio;
	v->id = vehicle_id;
	v->t_gen= t_global;
	sprintf(v->fifo, "fifo%d", vehicle_id);
	
	return v;
}

//
void* tracker (void *arg)
{
	pthread_detach(pthread_self());

	//t1 = t;
	int vehicle_id = *(int *) arg;
	free(arg);
	//create fifo

	Vehicle* v = create_vehicle(vehicle_id);

	printf("%c\n%i\n%i\n%s\n%g\n\n", v->gate, v->id, v->t_park, v->fifo, v->t_gen);
	
	int fd;
	switch(v->gate)
	{
		case 'N':
			fd = open("FIFON", O_WRONLY | O_NONBLOCK);
		case 'W':
			fd = open("FIFOW", O_WRONLY | O_NONBLOCK);
		case 'E':
			fd = open("FIFOE", O_WRONLY | O_NONBLOCK);
		case 'S':
			fd = open("FIFOS", O_WRONLY | O_NONBLOCK);
	}
	
	if(fd != 1)
	{
		close(fd);
	}

	write(fd, &v, sizeof(Vehicle));

	//esperar resposta
	close(fd);
	return NULL;
}

//
void mysleep(int tics)
{
	usleep((tics/nts)*pow(10, 6));
}

//argv[1] = t_geracao; argv[2] = u_relogio
int main(int argc, char *argv[]) 
{	

	if (argc != 3) {
		printf("Usage: gerador <T_GERACAO> <U_RELOGIO>\n");
		return 1;
	}


	//initializing variables
	nts = sysconf(_SC_CLK_TCK);
	int t_geracao = atoi(argv[1]) * nts;
	u_relogio = atoi(argv[2]);
	t_global = 0;

	//geracao
	srand(time(NULL));

	int v = 0;

	while(t_global < t_geracao)
	{
		v = rand() % 10;

		if(v < 5)
		{
			
		}
		else if (v < 8)
		{
			mysleep(u_relogio);
			t_global += u_relogio;
		}
		else if (v < 10)
		{
			mysleep(2 * u_relogio);
			t_global += 2 * u_relogio;
		}
		
		pthread_t pt_vehicle;

		nr = (int *) malloc (sizeof(int));
		*nr = id;
		id++;

		pthread_create(&pt_vehicle, NULL, tracker, nr);
		
		
	}

	return 0;
}







