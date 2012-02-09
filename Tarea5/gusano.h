#ifndef __GUSANO_H__
	#define __GUSANO_H__
	
	#include "listas.h"
	#include <sys/types.h>
	#include "eval.h"

	#define MAXX 500
	#define MAXY 500
	#define GORDO 10
	
	typedef struct {
	    LISTAS *cuerpo;
	    char dir;
	    int growth;
			int id;
			thread_t thread;
			const char *program;
			const char *color;
	} GUSANO;

	typedef struct {
	    int x, y;
	} POINT;

	void *wormThread(void *g); 
	int mrandom(int i, int j);
	GUSANO *init_g(int x, int y, int dir, int largo, const char *color, int gid);
	void free_g(GUSANO *g);
	int move_g(GUSANO *g);
	void cookie(int maxx, int maxy);
	int getToken(GUSANO *g, char token);
	int wormWall(GUSANO *g, char axis);
	int minByMod(int a, int b);
	void showColitionPoint(GUSANO *g);
	void runWish(char mode);
	void waitWish();
#endif
