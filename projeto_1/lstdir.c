#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <inttypes.h>

#define BUFF_SIZE 5000
pid_t sons[100];
int n_sons = 0;

/*
	transforma o argumento do tipo time_t numa string 
	retorna uma string com a data no formato YYYY.MM.DD-HH:MM:SS
*/
char* getdate(char* s, time_t time)
{
	strftime(s, 36, "%Y.%m.%d-%H:%M:%S", localtime(&time));
	return s;
}

/*
	le conteudo de um diretorio e guarda a informação de dos ficheiros encontrados nesse diretorio
	e subdiretorios no ficheiro com fd = argv[1]
*/
int main(int argc, char *argv[]) 
{	
	int fd = atoi(argv[1]);
	
	char current[PATH_MAX];
	char entry_info[BUFF_SIZE];
	getcwd(current, PATH_MAX);
	struct stat e_status;
	struct dirent *entry;
	DIR * dir;
	dir = opendir(current);

	// numero errado de parâmetros
	if (argc != 2) {                         
		printf("Usage: %s  <file descriptor (int)>", argv[0]);
		return 1;
	}
	
	while((entry = readdir(dir)) != NULL)
	{
		lstat(entry->d_name, &e_status);
		{
			//se for um ficheiro regular
			if(S_ISREG(e_status.st_mode))
			{
				char date[36];
				
				//escrever para ficheiro com fd = argv[1] a informação dos ficheiros
				sprintf(entry_info, "%40s %10ld %04o %20s %ju %s/%s\n", entry->d_name, e_status.st_size, e_status.st_mode &0777, getdate(date, e_status.st_mtime),  (uintmax_t)e_status.st_ino, current, entry->d_name);
				write(fd, entry_info, strlen(entry_info));
				
			}
			//se for um diretorio
			if(S_ISDIR(e_status.st_mode))
			{
				if((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
				{
					pid_t pid;
					pid = fork();
					if(pid == 0)
					{
						//muda a current working directory para o diretorio e executa de novo o lstdir
						chdir(entry->d_name);
						execl(argv[0], argv[0], argv[1], NULL);
						perror(argv[0]);
						return 1;
					}
					sons[n_sons++] = pid;
				}

			}
		}
		
	}
	closedir(dir);
	
	int k;
	for(k= 0; k < n_sons; k++)
		waitpid(sons[k], NULL, 0);
	
	
	return 0;
}