#include <sys/types.h>
#include <pthread.h>
#include "listas.h"
#include "eval.h"

#ifndef __GUSANO_S_H__
	#define __GUSANO_S_H__
	
	#define MAXX 500
	#define MAXY 500
	#define GORDO 10

	#define BUFF_SIZE 1024
	#define PORT 1818
	#define READ 0
	#define WRITE 1

	typedef struct{
		LISTAS *cuerpo;
		char dir;
		int growth;
		int id;
		pthread_t thread;
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
	void *startServer(void *mode);
	void stopServer();
	void *serverSlave(void *s);
#endif
