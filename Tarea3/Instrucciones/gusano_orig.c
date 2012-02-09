#include "listas.h"
#include "wish.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXX 500
#define MAXY 500
#define GORDO 10

typedef struct {
    LISTAS *cuerpo;
    char dir;
    int growth;
} GUSANO;

typedef struct {
    int x, y;
} POINT;

int tablero[MAXX/GORDO][MAXY/GORDO];
POINT galleta;
int galleta_id;

int mrandom(int i, int j) {
    long r = random();

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
    for(i=1; i<largo;i++) {
	x = x + GORDO;
        p = (POINT *)malloc(sizeof(POINT));
        p->x = x; p->y = y;
	add_l(g->cuerpo, p);
        tablero[x/GORDO][y/GORDO] = create_obj_w(x, y, GORDO, "oval", "red");
    }

    return g;
}

void free_g(GUSANO *g) {
    POINT *p;
    int item;

    for(p=extract_l(g->cuerpo); p != NULL; p=extract_l(g->cuerpo)) {
	item = tablero[p->x/GORDO][p->y/GORDO];
	delete_w(item);
	tablero[p->x/GORDO][p->y/GORDO] = 0;
	free(p);
    }
}

int move_g(GUSANO *g) {
    POINT *p, *q;
    int item;

    if(!g->growth) {
    	p = extract_l(g->cuerpo);
    	item = tablero[p->x/GORDO][p->y/GORDO];
    	tablero[p->x/GORDO][p->y/GORDO] = 0;
    }
    else
        p = (POINT *)malloc(sizeof(POINT));
    
    q = tail_l(g->cuerpo);
    p->x = q->x; p->y = q->y;
    switch(g->dir) {
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

    if(p->x >= MAXX || p->x < 0 || 
       p->y >= MAXY || p->y < 0 ||
       (tablero[p->x/GORDO][p->y/GORDO] != 0 &&
      !(galleta.x == p->x && galleta.y == p->y))) { /* esta ocupado y no es la galleta */
	if(!g->growth) delete_w(item);
	free(p);
	return 0;
    }

    add_l(g->cuerpo, p);
    if(g->growth) {
	g->growth--;
	item = create_obj_w(p->x, p->y, GORDO, "oval", "red");
    }
    else
    	coords_w(item, p->x, p->y, GORDO);
 
/* A comerse la galleta */
    if(galleta.x == p->x && galleta.y == p->y) {
	g->growth += tablero[p->x/GORDO][p->y/GORDO];
	tablero[p->x/GORDO][p->y/GORDO] = 0;
	cookie(MAXX, MAXY);
    }

    tablero[p->x/GORDO][p->y/GORDO] = item;

    return 1;
}

cookie(int maxx, int maxy) {
    int i, j, val;

    do {
        i = mrandom(0,maxx/GORDO);
        j = mrandom(0,maxy/GORDO);
        val = mrandom(1,9);
    } while(tablero[i][j] != 0);

    tablero[i][j] = val;
    galleta.x=i*GORDO; galleta.y=j*GORDO;
    if(galleta_id == 0)
        galleta_id = create_obj_w(i*GORDO, j*GORDO, GORDO, "rectangle", "blue");
    else
	coords_w(galleta_id, i*GORDO, j*GORDO, GORDO);
}

main()
{
    GUSANO *g;
    int i,j;
    int dir = 1; /* 0: N, 1: E, 2: S, 3: W */ 
    char s[200];

    srandom(time(NULL));

    for(i=0;i<MAXX/GORDO;i++)
	for(j=0;j<MAXY/GORDO;j++)
	    tablero[i][j] = 0;

    init_w(MAXX, MAXY);

    g = init_g(MAXX/2, MAXY/2, dir, 4); /* x,y,dir,largo */
    if(!galleta_id) cookie(MAXX, MAXY);

    for(;;) {
	usleep(100000);
	switch(getchar()) {
	case 'h': /* back */
	    dir = 3;
 	break;
	case 'j': /* down */
	    dir = 2;
 	break;
	case 'k': /* up */
	    dir = 0;
 	break;
	case 'l': /* forward */
	    dir = 1;
 	break;
	case EOF:
	    goto fin;
	break;
	default:
	    continue;
	break;
	}
        g->dir = dir;
        if( !move_g(g) ) {
     fin:   sprintf(s, "Boing!!, score = %d", size_l(g->cuerpo));
	    free_g(g);
	    close_w(s, 100, 100);
	    exit(0);
        }
    }
}
