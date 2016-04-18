#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define FLNAME "/tmp/files.txt"
#define SFLNAME "/tmp/sortedfiles.txt"
#define HLNAME "hlinks.txt"
#define FILEMODE 0600 //owner can read and write
#define BUFFER_SIZE 1000
#define STRING_SIZE 5000

/*
compares the content of f1 and f2
returns 1 if content is the same
returns 0 if content is different
*/

typedef struct Fstruct 
{
	char name[BUFFER_SIZE];
	char date[BUFFER_SIZE];
	char perm[BUFFER_SIZE];
	char path[BUFFER_SIZE];
	char inod[BUFFER_SIZE];
	char size[BUFFER_SIZE];
}Fstruct;

/*
	compara char a char o conteudo do ficheiro fp1 com o conteudo do ficheiro fp2
	return 0 se o conteudo for igual
	return 1 se o conteudo for diferente
*/
int compare_files(char fp1[], char fp2[])
{
	FILE *f1, *f2;
	int ch1, ch2;
	
	f1 = fopen(fp1, "r");
	if(f1 == NULL){
		printf("compare_files de rmdup nao conseguiu abrir o ficheiro %s\n", fp1);
		return 1;
	}
	
    f2 = fopen(fp2, "r");
	if(f2 == NULL){
		printf("compare_files de rmdup nao conseguiu abrir o ficheiro %s\n", fp2);
		return 1;
	}
	
	ch1 = getc(f1);
	ch2 = getc(f2);
	
	
	while((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2))
	{
		ch1 = getc(f1);
		ch2 = getc(f2);
	}
	
	 if(fclose(f1) != 0)
	 {
		printf("compare_files de rmdup nao conseguiu fechar o ficheiro %s\n", fp1);
		return 1;
	 }
	
     if(fclose(f2) != 0)
	 {
		printf("compare_files de rmdup nao conseguiu fechar o ficheiro %s\n", fp2);
		return 1;
	 }
	
	if(ch1 == ch2)
		return 0;
	else
		return 1;
	
}

/*
	cria hardlinks para os ficheiros duplicados presentes no array de Fstruct files
	return 0 se completar a função corretamente
	return 1 se ocorrer um erro
*/
int hard_link_files(Fstruct* files, int n_files)
{
	int hfd;
	int i, k;
	char hard_link_info[STRING_SIZE];
	
	hfd = open(HLNAME, O_RDWR | O_TRUNC | O_CREAT, FILEMODE);
	if(hfd == -1){
		printf("hard_link_files de rmdup não conseguiu abrir \"%s\"\n", HLNAME);
		return 1;
	}
	
	for(i = 0; i < n_files; i++)
	{
		
		k = i+1;
		while(strcmp(files[i].name, files[k].name) == 0)
		{
			if(strcmp(files[i].perm, files[k].perm) == 0)
			{
				if(compare_files(files[i].path, files[k].path) == 0)
				{
					if(unlink(files[k].path) == -1){
						printf("unlink falhou na função  hard_link_files de rmdup\n");
						return 1;
					}
					if(link(files[i].path, files[k].path) == -1){
						printf("link falhou na função  hard_link_files de rmdup\n");
						return 1;
					}
					
					sprintf(hard_link_info, "created hardlink from %40s to %40s\n", files[k].path, files[i].path);
					write(hfd, hard_link_info, strlen(hard_link_info));
					
				}
					
			}
			k++;
		}
		i = k -1;
	}
	
	
	
	if(close(hfd) == -1)
	{
		printf("hard_link_files de rmdup não conseguiu fechar \"%s\"\n", HLNAME);
		return 1;
	}
		
	return 0;
}

/*
	le a informação do ficheiro /tmp/files e coloca o num array de structs do tipo Fstruct.
	depois chama a função hard_link_files
	return 0 se completar a função corretamente
	return 1 se ocorrer um erro
*/
int read_files(void)
{
	FILE *f = fopen(SFLNAME,"r");
	char ch;
	int lines = 0;
	
	if(f == NULL){
		printf("read_files de rmdup não conseguiu abrir \"/tmp/sortedfiles.txt\"\n");
		return 1;
	}
	
	while(!feof(f))
	{
		ch = fgetc(f);
		if(ch == '\n')
		lines++;
	}
	
	printf("%d", lines);
	fseek(f, 0, SEEK_SET);
	
	Fstruct* files = malloc((lines -1) * sizeof(*files));
	
	int i;
	for(i = 0; i < lines; i++)
	{
	
		
		fscanf(f, "%s %s %s %s %s %s", files[i].name, files[i].size, 
			   files[i].perm, files[i].date, files[i].inod, files[i].path);
		
	}
	
	
	
	if(fclose(f) != 0)
	{
		printf("read_files de rmdup não conseguiu fechar \"/tmp/sortedfiles.txt\"\n");
		return 1;
	}
	
	if(hard_link_files(files, lines) == 1){
		printf("hard_link_files retornou 1 na função read_files\n");
		return 1;
	}
	return 0;
}




int main(int argc, char *argv[]) 
{	
    char currdir[PATH_MAX];
	pid_t pid;
	int fd, sfd;
	
	// numero errado de parâmetros
	if (argc != 2) {                         
		printf("Usage: %s  <\"directory\">\n", argv[0]);
		return 1;
	}
	
	//abrir ficheiros para colocar a informação dos ficheiros
	fd = open(FLNAME, O_RDWR | O_TRUNC | O_CREAT, FILEMODE);
	if(fd == -1 ){
		printf("main de rmdup não conseguiu abrir /tmp/files.txt");
		return 1;
	}
	sfd = open(SFLNAME, O_RDWR | O_TRUNC | O_CREAT, FILEMODE);
	if(sfd == -1 ){
		printf("main de rmdup não conseguiu abrir \"/tmp/sortedfiles.txt\"\n");
		return 1;
	}
	
	//encontrar path para o programa lstdir
	getcwd(currdir, PATH_MAX);
	strcat(currdir, "/bin/lstdir");
	
	if(chdir(argv[1]) == -1)
	{
		char absdir[PATH_MAX];
		strcat(absdir, currdir);
		strcat(absdir, "/");
		strcat(absdir, argv[1]);
		
		if(chdir(absdir) == -1){
			printf("chdir falhou na função main de rmdup\n");
			return 1;
		}
	}
	
	//executar o programa list dir num processo filho
	pid = fork();
	if (pid == 0) {
	  char stringfd[10];
		
	  sprintf(stringfd, "%d", fd);
	  execl(currdir, currdir, stringfd, NULL);
	}
	waitpid(pid, NULL, 0);
	//colocar o offset do ficheiro a zero
	lseek(fd, 0, SEEK_SET);
	
	//fazer sort do ficheiro /tmp/files para o ficheiro /tmp/sortedfiles
	pid = fork();
	if (pid == 0) {
		dup2(fd, STDIN_FILENO);
		dup2(sfd, STDOUT_FILENO);
	    execlp("sort", "sort", NULL);
		
		if(close(fd) == -1)
		{
			printf("main de rmdup não conseguiu fechar /tmp/files.txt");
			return 1;
		}
		
		if(close(sfd) == -1)
		{
			printf("main de rmdup não conseguiu fechar \"/tmp/sortedfiles.txt\"\n");
			return 1;
		}
	}
	waitpid(pid, NULL, 0);
	
	//ler e criar os hard_links para os ficheiros duplicados
	if(read_files() == 1){
		printf("readfiles retornou 1 no main de rmdup\n");
		return 1;
	}
	
	return 0;
}