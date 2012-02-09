#include "arbol.h"

#ifndef EVAL_H
	#define EVAL_H

	typedef enum {OPERADOR, CONSTANTE} TIPO;

	typedef struct {
	    TIPO type;
	    int val;
	    int (*funcion)(LISTA *);
	} NODO;
	
	TREE *make_expr();
	int eval_tree(TREE *expr);
	LISTA *make_l();
	void setOperator(char c, NODO *n);
	
	int operarSuma(LISTA *l);
	int operarResta(LISTA *l);
	int operarMult(LISTA *l);
	int operarDiv(LISTA *l);
	int operarOrder(LISTA *l);
	int operarEqual(LISTA *l);
	int operarNot(LISTA *l);
	int operarIf(LISTA *l);
	int operarNull(LISTA *l);
	
#endif
