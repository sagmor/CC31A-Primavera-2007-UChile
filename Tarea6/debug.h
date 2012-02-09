#ifndef __DEBUG_H__
	#define __DEBUG_H__

	void *debugMalloc(int size);
	void debugFree(void *ptr);
	void debugStop();

	#ifdef DEBUGING
		#include <stdio.h>
		#include <stdarg.h>
		#include <stdlib.h>
		#include <unistd.h>
		
		#define debug(...) fprintf(stderr,"Debug -> "); fprintf(stderr,__VA_ARGS__)
		#define malloc(s) debugMalloc(s)
		#define free(p) debugFree(p)
	#else
		#define debug(...)  
	#endif	

#endif
