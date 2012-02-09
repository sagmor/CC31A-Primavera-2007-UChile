#ifndef __GUSANO_C_H__
	#define __GUSANO_C_H__
	
	pid_t tee(int in, int out1, int out2);
	void wish(int in);
	void handler(int sig);
	void startWish(int s, char d);
	void waitWish();
#endif
