#include "arbol.h"
#include "gusano.h"

#ifndef __EVAL_H__
	#define __EVAL_H__

	typedef enum {OPERADOR, CONSTANTE} TIPO;

	typedef struct {
	    TIPO type;
	    int val;
	    int (*funcion)(LISTA *);
	} NODO;
	
	int eval(GUSANO *g);
	
	TREE *make_expr(GUSANO *g);
	int eval_tree(TREE *expr);
	LISTA *make_lista(GUSANO *g);
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
