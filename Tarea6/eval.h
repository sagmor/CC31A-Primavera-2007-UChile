#ifndef __EVAL_H__
	#define __EVAL_H__

	#include "arbol.h"
	#include "gusano_s.h"
	#include <sys/types.h>

	typedef enum {OPERADOR, CONSTANTE} TIPO;

	typedef struct {
	    TIPO type;
	    int val;
	    int (*funcion)(LISTA *);
	} NODO;
	
	int eval(GUSANO *g,FILE *progfp);

	TREE *make_expr(GUSANO *g,FILE *progfp);
	int eval_tree(TREE *expr);
	LISTA *make_lista(GUSANO *g,FILE *progfp);
	void setOperator(char c, NODO *n);
	
	int operarSuma(LISTA *l);
	int operarResta(LISTA *l);
	int operarMult(LISTA *l);
	int operarDiv(LISTA *l);
	int operarOrder(LISTA *l);
	int operarEqual(LISTA *l);
	int operarNot(LISTA *l);
	int operarIf(LISTA *l);
	int operarAnd(LISTA *l);
	int operarOr(LISTA *l);
	int operarNull(LISTA *l);
	
#endif
