#include <fcntl.h>
#include <stdarg.h>
#include "gusano.h"

#include "listas.h"
#include "eval.h"
#include "wish.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/* #include "debug.h" */

int tablero[MAXX/GORDO][MAXY/GORDO];
POINT galleta;
int galleta_id, wpid;
GUSANO *g;

int main (int argc, char const *argv[])
{
	int i,j;
	int dir = 1; /* 0: N, 1: E, 2: S, 3: W */ 
	char s[200];

	/* debug("Starting Application: %s\n", argv[0]); */
	
	if(argc != 2)
	{
		fprintf(stderr,"Use: %s program\n", argv[0]);
		return EXIT_FAILURE;
	}
	runWish();
	
	close(STDIN_FILENO);
	if(open(argv[1],O_RDONLY,NULL))
	{
		printf("Error al Abrir el archivo\n");
		return EXIT_FAILURE;
	}

	srand(time(NULL));

	for(i=0;i<MAXX/GORDO;i++)
		for(j=0;j<MAXY/GORDO;j++)
			tablero[i][j] = 0;
	
	init_w(MAXX, MAXY,"Gusano - Tarea 3");

	g = init_g(MAXX/2, MAXY/2, dir, 4); /* x,y,dir,largo */
	if(!galleta_id) cookie(MAXX, MAXY);
	
	for(;;)
	{
		usleep(100000);

		dir = eval(g);
		if(dir == -1)
		{
			exit(EXIT_FAILURE);
		} 
		else
		{
			g->dir = dir;

			if( !move_g(g) )
			{
				sprintf(s, "Boing!!, score = %d", size_l(g->cuerpo));
				showColitionPoint(g);
				free_g(g);
				g=NULL;
				close_w(s, 100, 100);
				waitWish(wpid);
				exit(0);
			}
		}
		
		close(STDIN_FILENO);
		
		if(open(argv[1],O_RDONLY,NULL))
		{
			printf("Error al Abrir el archivo\n");
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_FAILURE);
}

void handler(int sig)
{
	static int pausas = 0;
	if (sig == SIGINT)
	{
		pausas++;
		if (pausas&1)
		{
			/* fprintf(stderr,"Pausa\n"); */
			pause();
		}
	} else if (sig == SIGPIPE || sig == SIGCHLD)
	{
		
		/* fprintf(stderr,"Hijo murio\n"); */

		free_g(g);
		waitWish();	
		exit(EXIT_FAILURE);
	}

}

int mrandom(int i, int j) {
    long r = rand();

    return (r % (j-i)) + i;
}

GUSANO *init_g(int x, int y, int dir, int largo) {
	GUSANO *g;
	POINT *p;
	int i;

	g = (GUSANO *)malloc(sizeof(GUSANO));
	g->dir = dir;
	g->growth = 0;
	g->cuerpo = init_l();
	p = (POINT *)malloc(sizeof(POINT));
	p->x = x; p->y = y;
	add_l(g->cuerpo, p);
	tablero[x/GORDO][y/GORDO] = create_obj_w(x, y, GORDO, "oval", "red");
	for(i=1; i<largo;i++)
	{
		x = x + GORDO;
		p = (POINT *)malloc(sizeof(POINT));
		p->x = x; p->y = y;
		add_l(g->cuerpo, p);
		tablero[x/GORDO][y/GORDO] = create_obj_w(x, y, GORDO, "oval", "red");
	}

	return g;
}

void free_g(GUSANO *g) 
{
	POINT *p;
	/* int item; */

	if (g == NULL)
		return;

	for(p=extract_l(g->cuerpo); p != NULL; p=extract_l(g->cuerpo))
	{
		/*
		item = tablero[p->x/GORDO][p->y/GORDO];
		delete_w(item);
		tablero[p->x/GORDO][p->y/GORDO] = 0;
		*/
		free(p);
	}
	
	free(g->cuerpo);
	free(g);
}

int move_g(GUSANO *g) {
	POINT *p, *q;
	int item;

	if(!g->growth)
	{
		p = extract_l(g->cuerpo);
		item = tablero[p->x/GORDO][p->y/GORDO];
		tablero[p->x/GORDO][p->y/GORDO] = 0;
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
		(tablero[p->x/GORDO][p->y/GORDO] != 0 && (galleta.x != p->x || galleta.y != p->y)))
	{
		if(!g->growth) delete_w(item);
		free(p);
		return 0;
	}

	add_l(g->cuerpo, p);
	if(g->growth)
	{
		g->growth--;
		item = create_obj_w(p->x, p->y, GORDO, "oval", "red");
	}
	else
		coords_w(item, p->x, p->y, GORDO);
 
	/* A comerse la galleta */
	if(galleta.x == p->x && galleta.y == p->y)
	{
		g->growth += tablero[p->x/GORDO][p->y/GORDO];
		tablero[p->x/GORDO][p->y/GORDO] = 0;
		cookie(MAXX, MAXY);
	}

	tablero[p->x/GORDO][p->y/GORDO] = item;

	return 1;
}

void cookie(int maxx, int maxy)
{
	int i, j, val;

	i = mrandom(0,maxx/GORDO);
	j = mrandom(0,maxy/GORDO);
	if(tablero[i][j] != 0)
	{
		cookie(maxx, maxy);
	}
	else
	{
		val = mrandom(1,9);
		tablero[i][j] = val;
		galleta.x=i*GORDO; galleta.y=j*GORDO;
		if(galleta_id == 0)
			galleta_id = create_obj_w(i*GORDO, j*GORDO, GORDO, "rectangle", "blue");
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
				res = minByMod((MAXX - p->x-GORDO)/GORDO, selfWormWall(g,'x'));
			}
			else /* Izquierda */
			{
				res = minByMod(-p->x/GORDO, selfWormWall(g,'x'));
			}
			break;
		case 'n': /* distancia al muro (0-9) en y (<0 => abajo)) */
			if (p->y < MAXY/2) /* Arriba */
			{
				res = minByMod(p->y/GORDO, selfWormWall(g,'y'));
			}
			else /* Abajo */
			{
				res = minByMod(-(MAXY - p->y-GORDO)/GORDO, selfWormWall(g,'y'));
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
int selfWormWall(GUSANO *g, char axis)
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
				if (tablero[i][y]!=0 && ((galleta.x)/GORDO != i || (galleta.y)/GORDO != y))
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
				if (tablero[i][y]!=0 && ((galleta.x)/GORDO != i || (galleta.y)/GORDO != y))
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
				if (tablero[x][i]!=0 && ((galleta.x)/GORDO != x || (galleta.y)/GORDO != i))
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
				if (tablero[x][i]!=0 && ((galleta.x)/GORDO != x || (galleta.y)/GORDO != i))
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
int selfWormWallOLD(GUSANO *g, char axis)
{
	int length = 2;
	int res,i,j,tmp=-1;
	int x = (((POINT *)tail_l(g->cuerpo)) -> x)/GORDO;
	int y = (((POINT *)tail_l(g->cuerpo)) -> y)/GORDO;
	switch(axis)
	{
		case 'x':
		res = MAXX/GORDO;
		for (i=0; i<MAXX/GORDO && i!=x;i++)
		{
			for (j = y; j <= y-length && j>=0; j--)
			{
				if (tablero[i][j]!=0 && tablero[i][j]!=galleta_id)
					tmp = i;
				else
				{
					tmp=-1;
					goto skipTopX;
				}
			}
			
			/* Encontro un muro en x */
			if (tmp>0)
				goto skipBottomX;
			
			skipTopX:
			for (j = y; j <= y+length && j<MAXY/GORDO; j++)
			{
				if (tablero[i][j]!=0 && tablero[i][j]!=galleta_id)
					tmp = i;
				else
				{
					tmp=-1;
					goto skipBottomX;
				}
			}
			
			skipBottomX:
			if (tmp >= 0)
			{
				if (tmp>x)
					tmp -= x+1;
				else 
					tmp -= x-1;
				res = minByMod(res,tmp);
			}
		}
		break;
		case 'y':
		res = MAXY/GORDO;
		for (i=0; i<MAXY/GORDO && i!=y;i++)
		{
			for (j = x; j <= x-length && j>=0; j--)
			{
				if (tablero[j][i]!=0 && tablero[j][i]!=galleta_id)
					tmp = i;
				else
				{
					tmp=-1;
					goto skipTopY;
				}
			}
			
			/* Encontro un muro en x */
			if (tmp>0)
				goto skipBottomY;
			
			skipTopY:
			for (j = x; j <= x+length && j<MAXX/GORDO; j++)
			{
				if (tablero[j][i]!=0 && tablero[j][i]!=galleta_id)
					tmp = i;
				else
				{
					tmp=-1;
					goto skipBottomY;
				}
			}
			
			skipBottomY:
			if (tmp >= 0)
			{
				if (tmp>y)
					tmp -= y+1;
				else 
					tmp -= y-1;
				res = minByMod(res,tmp);
			}
		}
		break;
		default:
		res = 99;
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

void showColitionPoint(GUSANO *g)
{
	int dx = 0;
	int dy = 0;
	create_obj_w(((POINT *)tail_l(g->cuerpo)) -> x, ((POINT *)tail_l(g->cuerpo)) -> y, GORDO, "oval", "yellow");
	switch (g->dir)
	{
		case 0:
			dy = -GORDO;
			break;
		case 1:
			dx = GORDO;
			break;
		case 2:
			dy = GORDO;
			break;
		case 3:
			dx = - GORDO;
			break;
	}
	create_obj_w(((POINT *)tail_l(g->cuerpo)) -> x + dx, ((POINT *)tail_l(g->cuerpo)) -> y + dy, GORDO, "oval", "yellow");
}

void runWish()
{
	int fds[2];
	struct sigaction sig;
	
	pipe(fds);
	
	if ((wpid = fork()) == 0)
	{

		sig.sa_handler = SIG_IGN;
		sigemptyset (&sig.sa_mask);
		sig.sa_flags = 0;

		sigaction(SIGINT,&sig,NULL);

		close(fds[STDOUT_FILENO]);
		close(STDIN_FILENO);
		dup(fds[STDIN_FILENO]); close(fds[STDIN_FILENO]);
		
		execlp("wish","wish",NULL);
		exit(EXIT_FAILURE);
	}
	else if (wpid < 0)
	{
		/* Error */
		exit(EXIT_FAILURE);
	}
	sig.sa_handler = handler;
	sigemptyset(&sig.sa_mask);
  sig.sa_flags = SA_NODEFER;
	
	sigaction(SIGINT,&sig,NULL);
	sigaction(SIGCHLD,&sig,NULL);
	sigaction(SIGPIPE,&sig,NULL);

	close(fds[STDIN_FILENO]);
	close(STDOUT_FILENO);
	dup(fds[STDOUT_FILENO]); close(fds[STDOUT_FILENO]);
}

void waitWish()
{
	/* fprintf(stderr,"Esperando la muerte de wish\n"); */
	if (wpid == 0)
		return;
	waitpid(wpid,NULL,0);
	wpid=0;
}
