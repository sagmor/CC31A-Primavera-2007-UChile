#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>

#include "listas.h"
#include "eval.h"
#include "wish.h"

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

#include "gusano_s.h"
#include "jsocket.h"

	#define COLORES 12
	char const *colors[] = {
		"red", "blue", "yellow", "black", "chocolate", "white", "cyan", 
		"orange", "brown", "magenta", "violet", "green"
	};

int tablero[MAXX/GORDO][MAXY/GORDO][2];
POINT galleta;
int galleta_id=0, wpid=0, tpid=0, worms_num, message_id=0;
GUSANO **worms;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char paused = 2;
char **programs;
pthread_cond_t pause_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t tablero_cond[MAXX/GORDO][MAXY/GORDO];
pthread_t server;

int main(int argc, char const *argv[])
{
	int i,j;
	int dir = 1; /* 0: N, 1: E, 2: S, 3: W */ 

	if(argc == 2)
	{
		worms_num = atoi(argv[1]);
		pthread_create(&server, NULL, startServer, NULL);
	} 
	else if (argc == 3 && strcmp(argv[1], "-d")==0)
	{
		worms_num = atoi(argv[2]);
		/* Run in Debug mode*/
		pthread_create(&server, NULL, startServer, (void *)&argv[1][1]);
	} 
	else 
	{
		fprintf(stderr,"Uso: %s [-d] ngusanos\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));

	for(i=0;i<MAXX/GORDO;i++)
	{
		for(j=0;j<MAXY/GORDO;j++)
		{
			tablero[i][j][0] = tablero[i][j][1] = 0;
			pthread_cond_init(&tablero_cond[i][j],NULL);
		}
	}
	
	pthread_mutex_lock(&mutex);
	
	while (paused > 1)
		pthread_cond_wait(&pause_cond,&mutex);
	
	pthread_mutex_unlock(&mutex);

	init_w(MAXX, MAXY,"Gusano - Tarea 5");

	worms = (GUSANO **)malloc(sizeof(GUSANO*)*worms_num);

	for (i=0; i < worms_num; i++)
	{
		worms[i] = init_g(MAXX/2-2*i*GORDO, MAXY/2+2*i*GORDO, dir, 4, colors[i%COLORES],i+1);
		usleep(100000);

		pthread_create(&worms[i]->thread, NULL, &wormThread, worms[i]);
	}
	sleep(1);
	if(!galleta_id) cookie(MAXX, MAXY);
	paused = 0;
	pthread_cond_broadcast(&pause_cond);

	for (i=0; i < worms_num; i++)
	{
		pthread_join(worms[i]->thread, NULL);
		free(worms[i]);
	}

	free(worms);
	message_w("Todos Muertos", MAXX/2, MAXY/2);
	close_w();
	stopServer();

	exit(EXIT_SUCCESS);
}

void *wormThread(void *vg)
{
	FILE *prog=NULL;
	char s[200];
	int dir = 1; /* 0: N, 1: E, 2: S, 3: W */ 
	GUSANO *g = (GUSANO *)vg;
	
	for(;;)
	{
		usleep(100000);
		
		pthread_mutex_lock(&mutex);
		while(paused)
		{
			pthread_cond_wait(&pause_cond,&mutex);
		}
		
		prog=fopen(programs[g->id-1],"r");

		if(prog == NULL)
		{
			fprintf(stderr,"Error al Abrir el archivo \"%s\"\n", programs[g->id-1]);
			free_g(g);
			pthread_mutex_unlock(&mutex);
			return NULL;
		}

		dir = eval(g,prog);
		fclose(prog);
		
		g->dir = dir;

		if( !move_g(g) )
		{
			sprintf(s, "Boing!!, Gusano %d obtuvo %d puntos", g->id, size_l(g->cuerpo));
			if(message_id) delete_w(message_id);
			message_id = message_w(s, 100, 100);
			free_g(g);
			pthread_mutex_unlock(&mutex);
			return NULL;
		}
			
		pthread_mutex_unlock(&mutex);	
	}

	return NULL;
}

void handler(int sig)
{
	static int pausas = 0;
	int i;
	if (sig == SIGINT)
	{
		pausas++;
		if (pausas&1)
		{
			/* fprintf(stderr,"Pausa\n"); */
			paused=1;
			pause();
		} else {
			paused=0;
			pthread_cond_broadcast(&pause_cond);
		}
	} else if (sig == SIGPIPE || sig == SIGCHLD) {
		paused=1;
		
		if(tpid) kill(tpid,SIGKILL);

		for (i=0; i < worms_num && worms[i]; i++)
		{
			POINT *p;
			
			if (worms[i] == NULL)
				return;

			for(p=extract_l(worms[i]->cuerpo); p != NULL; p=extract_l(worms[i]->cuerpo))
			{
				free(p);
			}

			free(worms[i]->cuerpo);
			/*free(worms[i]->color);*/
			
			pthread_cancel(worms[i]->thread);
			
			free(worms[i]);
		}

		free(worms);
	
	
		exit(EXIT_FAILURE);
	}
}

int mrandom(int i, int j)
{
	long r = rand();

	return (r % (j-i)) + i;
}

GUSANO *init_g(int x, int y, int dir, int largo, const char *color, int gid)
{
	GUSANO *g;
	POINT *p;
	int i;

	g = (GUSANO *)malloc(sizeof(GUSANO));
	g->dir = dir;
	g->growth = 0;
	g->cuerpo = init_l();
	g->color = color;
		/*malloc(strlen(color)+1);
	strcpy(g->color, color);*/
	g->id = gid;
	p = (POINT *)malloc(sizeof(POINT));
	p->x = x; p->y = y;
	add_l(g->cuerpo, p);
	tablero[x/GORDO][y/GORDO][0] = create_obj_w(x, y, GORDO, "oval", color);
	tablero[x/GORDO][y/GORDO][1] = g->id;
	for(i=1; i<largo;i++)
	{
		x = x + GORDO;
		p = (POINT *)malloc(sizeof(POINT));
		p->x = x; p->y = y;
		add_l(g->cuerpo, p);
		tablero[x/GORDO][y/GORDO][0] = create_obj_w(x, y, GORDO, "oval", color);
		tablero[x/GORDO][y/GORDO][1] = g->id;
	}

	return g;
}

void free_g(GUSANO *g) 
{
	POINT *p;
	int item;

	if (g == NULL)
		return;

	for(p=extract_l(g->cuerpo); p != NULL; p=extract_l(g->cuerpo))
	{
		item = tablero[p->x/GORDO][p->y/GORDO][0];
		delete_w(item);
		tablero[p->x/GORDO][p->y/GORDO][0] = 0;
		tablero[p->x/GORDO][p->y/GORDO][1] = 0;
		pthread_cond_signal(&tablero_cond[p->x/GORDO][p->y/GORDO]);	

		free(p);
	}
	
	free(g->cuerpo);
	/*free(g->color);*/
	/* free(g); */
}

int move_g(GUSANO *g)
{
	POINT *p, *q;
	int item;
	struct timespec wait_time;

	if(!g->growth)
	{
		p = extract_l(g->cuerpo);
		item = tablero[p->x/GORDO][p->y/GORDO][0];
		tablero[p->x/GORDO][p->y/GORDO][0] = 0;
		tablero[p->x/GORDO][p->y/GORDO][1] = 0;
		pthread_cond_signal(&tablero_cond[p->x/GORDO][p->y/GORDO]);
	}
	else
		p = (POINT *)malloc(sizeof(POINT));
	
	q = tail_l(g->cuerpo);
	p->x = q->x; p->y = q->y;
	switch(g->dir)
	{
		case 0: /* N */
			p->y = q->y-GORDO;
			break;
		case 1: /* E */
	    p->x = q->x+GORDO;
			break;
		case 2: /* S */
	    p->y = q->y+GORDO;
			break;
		case 3: /* W */
			p->x = q->x-GORDO;
			break;
	}

	if(p->x >= MAXX || p->x < 0 || p->y >= MAXY || p->y < 0 ||
			tablero[p->x/GORDO][p->y/GORDO][1] == g->id)
	{
		if(!g->growth) delete_w(item);
		free(p);
		return 0;
	}

	while(tablero[p->x/GORDO][p->y/GORDO][1]!=0 && tablero[p->x/GORDO][p->y/GORDO][1] != g->id )
	{
		wait_time.tv_nsec = 0;
		wait_time.tv_sec = time(NULL)+10;

		if(pthread_cond_timedwait(&tablero_cond[p->x/GORDO][p->y/GORDO],&mutex, &wait_time)==ETIMEDOUT && !paused)
		{
			if(!g->growth) delete_w(item);
			free(p);
			return 0;
		}
	}
	
	add_l(g->cuerpo, p);
	
	if(g->growth)
	{
		g->growth--;
		item = create_obj_w(p->x, p->y, GORDO, "oval", g->color);
	}
	else
		coords_w(item, p->x, p->y, GORDO);
 
	/* A comerse la galleta */
	if(galleta.x/GORDO == p->x/GORDO && galleta.y/GORDO == p->y/GORDO)
	{
		g->growth += tablero[p->x/GORDO][p->y/GORDO][0];
		tablero[p->x/GORDO][p->y/GORDO][0] = 0;
		tablero[p->x/GORDO][p->y/GORDO][1] = 0;
		cookie(MAXX, MAXY);
	}

	
	tablero[p->x/GORDO][p->y/GORDO][0] = item;
	tablero[p->x/GORDO][p->y/GORDO][1] = g->id;

	return 1;
}

void cookie(int maxx, int maxy)
{
	int i, j, val;

	i = mrandom(0,maxx/GORDO);
	j = mrandom(0,maxy/GORDO);
	if(tablero[i][j][0] != 0)
	{
		cookie(maxx, maxy);
	}
	else
	{
		val = mrandom(1,9);
		tablero[i][j][0] = val;
		galleta.x=i*GORDO; galleta.y=j*GORDO;
		if(galleta_id == 0)
			galleta_id = create_obj_w(i*GORDO, j*GORDO, GORDO, "rectangle", "aquamarine");
		else
			coords_w(galleta_id, i*GORDO, j*GORDO, GORDO);
	}
}
	

int getToken(GUSANO *g, char token)
{
	int res;
	POINT *p = (POINT *)tail_l(g->cuerpo);
	/*
	debug("Worm Position\n");
	debug("x->%d\n",p->x);
	debug("y->%d\n",p->y);
	debug("Worm Direction: %d\n",g->dir);
	debug("Geting token: %c\n",token);
	*/
	
	/* TODO */
	switch (token)
	{
		case 'd': /* direccion actual */
			/*debug("entered dir\n");*/
			res = g->dir;
			/*debug("res = %d\n",res);*/
			break;
		case 'm': /* distancia al muro (0-9) en x (<0 => izq)) */
			if (p->x > MAXX/2) /* Derecha */
			{
				res = minByMod((MAXX - p->x-GORDO)/GORDO, wormWall(g,'x'));
			}
			else /* Izquierda */
			{
				res = minByMod(-p->x/GORDO, wormWall(g,'x'));
			}
			break;
		case 'n': /* distancia al muro (0-9) en y (<0 => abajo)) */
			if (p->y < MAXY/2) /* Arriba */
			{
				res = minByMod(p->y/GORDO, wormWall(g,'y'));
			}
			else /* Abajo */
			{
				res = minByMod(-(MAXY - p->y-GORDO)/GORDO, wormWall(g,'y'));
			}
			break;
		case 'c': /* distancia a cookie(0-9) en x) */
			res = (galleta.x - p->x)/GORDO;
			break;
		case 'e': /* distancia a cookie(0-9) en y) */
			res = (galleta.y - p->y)/GORDO;
			break;
		
		/* Directions */
		case 'h': /* back */
			res = 3;
			break;
		case 'j': /* down */
			res = 2;
			break;
		case 'k': /* up */
			res = 0;
			break;
		case 'l': /* forward */
			res = 1;
			break;
		default:
			res = 0;
			break;
	}
	/* debug("token %c returned: %d\n", token, res); */
	return res;
}

int wormWall(GUSANO *g, char axis)
{
	int res,i;
	int x = (((POINT *)tail_l(g->cuerpo)) -> x)/GORDO;
	int y = (((POINT *)tail_l(g->cuerpo)) -> y)/GORDO;
	
	switch(axis)
	{
		case 'x':
		res = MAXX/GORDO;
		if(g->dir!=1)
		{
			for(i=x-1;i>=0;i--)
			{
				if (tablero[i][y][0]!=0 && ((galleta.x)/GORDO != i || (galleta.y)/GORDO != y))
				{
					res = minByMod(res, i-x+1);
					break;
				}
			}
		}
		
		if(g->dir!=3)
		{
			for(i=x+1;i<MAXX/GORDO;i++)
			{
				if (tablero[i][y][0]!=0 && ((galleta.x)/GORDO != i || (galleta.y)/GORDO != y))
				{
					res = minByMod(res, i-x-1);
					break;
				}
			}
		}
		break;
		case 'y':
		res = MAXY/GORDO;
		if(g->dir != 2)
		{
			for(i=y-1;i>=0;i--)
			{
				if (tablero[x][i][0]!=0 && ((galleta.x)/GORDO != x || (galleta.y)/GORDO != i))
				{
					res = minByMod(res, i-y+1);
					break;
				}
			}
		}
		
		if(g->dir!=0)
		{
			for(i=y+1;i<MAXY/GORDO;i++)
			{
				if (tablero[x][i][0]!=0 && ((galleta.x)/GORDO != x || (galleta.y)/GORDO != i))
				{
					res = minByMod(res, i-y-1);
					break;
				}
			}
		}
		break;
		default:
			res=99;
			break;
	}
	
	/* debug("found worm wall in %c at %d\n",axis,res); */
	return res;
}

int minByMod(int a, int b)
{
	int ta = (a>0)?a:-a;
	int tb = (b>0)?b:-b;
	/* debug("minimo entre %d y %d es %d\n",a,b,(ta<tb)?a:b); */
	return (ta<tb)?a:b;
}

void *startServer(void *mode)
{
	int fds[2];
	int stdoutfd, dead_clients=0;
	int server, i, readed, sended;
	int *clients_fds;
	char buff[BUFF_SIZE];
	pthread_t *clients_threads;
	struct sigaction sig;
	/* pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER; */
	
	sig.sa_handler = SIG_IGN;
	sigemptyset (&sig.sa_mask);
	sig.sa_flags = 0;

	sigaction(SIGPIPE,&sig,NULL);
	
	clients_fds = (int *)malloc(sizeof(int)*worms_num);
	clients_threads = (pthread_t *)malloc(sizeof(pthread_t)*worms_num);
	programs = (char **)malloc(sizeof(char **)*worms_num);
	
	pipe(fds);
	if (mode && *(char *)mode == 'd')
	{
		stdoutfd = dup(STDOUT_FILENO);
	} else
		stdoutfd=-1;
	
	close(STDOUT_FILENO);
	dup(fds[WRITE]);
	close(fds[WRITE]);
	
	if ((server = j_socket()) == -1)
	{
		perror("Error de Socket");
		exit(1);
	}
	
	if (j_bind(server,PORT) == -1)
	{
		perror("Error de Bind");
		exit(1);
	}
	
	for (i = 0; i < worms_num; i++)
	{
		clients_fds[i] = j_accept(server);
		if (clients_fds[i] == -1)
		{
			fprintf(stderr,"Fallo la Conexion del cliente %d", i--);
			continue;
		}
		
		programs[i] = (char *)malloc(strlen(getpwuid(getuid())->pw_name)+20);
		sprintf(programs[i], "/tmp/worms-%s-%d", getpwuid(getuid())->pw_name, clients_fds[i]);
		
		pthread_create(&clients_threads[i],NULL,serverSlave, (void *)&clients_fds[i]);
		/* paused++; */
	}
	
	/*	
	pthread_mutex_lock(&server_mutex);
	while(paused > 2)
		pthread_cond_wait(&pause_cond, &server_mutex);
	pthread_mutex_unlock(&server_mutex);

	*/
	
	for (i = 0; i < worms_num; i++)
		pthread_join(clients_threads[i], NULL);
	
	paused--;
	pthread_cond_signal(&pause_cond);
	
	/* Redirigimos toda entrada a los clientes*/
	for(;;)
	{
		if ((readed = read(fds[READ], buff, BUFF_SIZE)) == 0)
		{
			readed = 1;
			buff[0] = EOF;
		}
		
		if (stdoutfd > 0)
			if ((sended = write(stdoutfd, buff, readed)) < readed)
				fprintf(stderr, "Se perdieron %d bytes al imprimir en la salida estandar", readed-sended);

		for (i = 0; i < worms_num && clients_fds[i] > 0; i++)
		{
			if ((sended = write(clients_fds[i], buff, readed)) <= 0 && (sended = write(clients_fds[i], buff, readed)) <= 0)
			{
					fprintf(stderr, "Se perdio la conexion con %d\n",i);
					close(clients_fds[i]);
					clients_fds[i]=-1;
					dead_clients++;
			}
		}
		
		if (buff[0] == EOF || dead_clients == worms_num)
			break;
	}
	
	paused=1;
	sleep(1);
	
	for (i = 0; i < worms_num; i++)
	{
		remove(programs[i]);
		free(programs[i]);
	}
	free(programs);
	free(clients_fds);
	free(clients_threads);
	
	exit(0);
	return NULL;
}

void stopServer()
{
	if (tpid)
	{
		waitpid(tpid,NULL,0);
		tpid=0;
	}	
	if (wpid)
	{
		waitpid(wpid,NULL,0);
		wpid=0;
	}	
}

void *serverSlave(void *s)
{
	char *prog, buff[BUFF_SIZE];
	int fd,readed;
	
	
	prog = (char *)malloc(strlen(getpwuid(getuid())->pw_name)+20);
	sprintf(prog, "/tmp/worms-%s-%d", getpwuid(getuid())->pw_name, *(int *)s);
	
	fd = open(prog,O_WRONLY|O_CREAT|O_TRUNC,0666);
	
	while((readed=read(*(int *)s, buff, BUFF_SIZE))>0)
	{
		if(buff[readed-1]==EOF)
		{
			write(fd,buff,readed-1);
			break;
		}
		else
			write(fd,buff,readed);
	}
	
	close(fd);
	
	free(prog);
	
	return NULL;
}
