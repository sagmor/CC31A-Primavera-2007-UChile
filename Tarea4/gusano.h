#ifndef __GUSANO_H__
	#define __GUSANO_H__
	
	#include "listas.h"
	
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

	int mrandom(int i, int j);
	GUSANO *init_g(int x, int y, int dir, int largo);
	void free_g(GUSANO *g);
	int move_g(GUSANO *g);
	void cookie(int maxx, int maxy);
	int getToken(GUSANO *g, char token);
	int selfWormWall(GUSANO *g, char axis);
	int minByMod(int a, int b);
	void showColitionPoint(GUSANO *g);
	void runWish();
	void waitWish();
#endif
