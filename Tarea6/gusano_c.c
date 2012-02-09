#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "wish.h"

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


#include <sys/wait.h>

#include "gusano_c.h"
#include "jsocket.h"


#define READ 0
#define WRITE 1
#define BUFF_SIZE 1024

pid_t pid_wish, pid_tee;
int paused=0;

int main(int argc, char const *argv[])
{
	
	char buff[BUFF_SIZE], debug;
	const char *programa, *host;
	int port, s, file, readed;
	
	if (argc == 4)
	{
		debug = 0;
		programa = argv[1];
		host = argv[2];
		port = atoi(argv[3]);
	}
	else if (argc == 5 && strcmp(argv[1], "-d")==0)
	{
		debug = 1;
		programa = argv[2];
		host = argv[3];
		port = atoi(argv[4]);
	}
	else
	{
		fprintf(stderr,"Uso: %s [-d] programa host port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if ((s = j_socket()) == -1)
	{
		perror("Error de Socket");
		exit(1);
	}
	
	if (j_connect(s, host, port) == -1)
	{
		perror("Error de Conexion");
		exit(1);
	}
	
	if(!(file = open(programa,O_RDONLY,0)))
	{
		perror("Error del Archivo");
		exit(1);
	}
	
	while ((readed = read(file, buff, BUFF_SIZE))>0)
		write(s,buff,readed);
	
	buff[0]=EOF;
	write(s,buff,1);
	
	startWish(s,debug);
	close(s);
	waitWish();

	exit(0);
}


void handler(int sig)
{
	static int pausas = 0;
	if (sig == SIGINT)
	{
		paused = 1;
		pausas++;
		if (pausas&1)
		{
			pause();
		}
	}
	else if (sig == SIGCHLD || sig == SIGPIPE)
		exit (1);
}


pid_t tee(int in, int out1, int out2)
{
	struct sigaction sig;
	pid_t tpid;

	if((tpid=fork()) == 0)
	{
		int cnt;
		char buf[255];

		sig.sa_handler = SIG_IGN;
		sigemptyset (&sig.sa_mask);
		sig.sa_flags = 0;

		sigaction(SIGINT,&sig,NULL);
		/* soy tee */
		while((cnt=read(in, buf, 255)) > 0)
		{
			write(out1, buf, cnt);
			write(out2, buf, cnt);
		}
		exit(0);
	}

	return tpid;
}

void wish(int in)
{
	struct sigaction sig;
	pid_t wpid;
	
	if ((wpid = fork()) == 0)
	{

		sig.sa_handler = SIG_IGN;
		sigemptyset (&sig.sa_mask);
		sig.sa_flags = 0;

		sigaction(SIGINT,&sig,NULL);

		if (in != STDIN_FILENO)
		{
			close(STDIN_FILENO);
			dup(in); close(in);
		}
		
		execlp("wish","wish",NULL);
		exit(EXIT_FAILURE);
	}
	else if (wpid < 0)
	{
		/* Error */
		exit(EXIT_FAILURE);
	}
}

void startWish(int s, char d)
{
	struct sigaction sig;
	pid_t tpid;
	int fds[2];
	
	pipe(fds);

	if((tpid=fork()) == 0)
	{
		int cnt;
		char buf[255];
		
		wish(fds[READ]);
		close(fds[READ]);

		sig.sa_handler = handler;
		sigemptyset (&sig.sa_mask);
		sig.sa_flags = SA_NODEFER;

		sigaction(SIGINT,&sig,NULL);
		sigaction(SIGCHLD,&sig,NULL);
		sigaction(SIGPIPE,&sig,NULL);
		
		while((cnt=read(s, buf, 255)) > 0||paused)
		{
			if(paused)
			{
				paused = 0;
				if (!cnt)
					continue;
			}
			
			if (d)
				write(0, buf, cnt);
				
			write(fds[WRITE], buf, cnt);
		}
		exit(0);
	}
	
	sig.sa_handler = SIG_IGN;
	sigemptyset (&sig.sa_mask);
	sig.sa_flags = 0;

	sigaction(SIGINT,&sig,NULL);
}

void waitWish()
{
	waitpid(pid_wish,0,0);
	waitpid(pid_tee,0,0);
}