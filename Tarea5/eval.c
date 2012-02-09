#include <stdio.h>
#include <stdlib.h>
#include "arbol.h"
#include "eval.h"
#include "gusano.h"
#include <unistd.h>
#include <sys/types.h>

/* #include "debug.h" */

/* Evalua expresiones a la lisp (numeros de un solo digito) como:
	(+ 1 (/8 (*22)) 1 5 (* 2 3) (-9))
	-> 6
	(*12345)
	-> 120
	(-)
	-> 0
	(/)
	-> 1
	(+ (- 9 2) 1 4 9)
	-> 3
	(+ (? (= (? (> 4 3 2 1) (? (= 9 9) 1 2) (* 2 3)) 1) 2 4) 2)
	-> 4
	(+ (? (= (* 2 3) 6 (+ 3 2 1)) 1) 3 4)
	-> 8
	(> (+ (? (= (? (> 4 3 2 1) (? (= 9 9) 1 2) (* 2 3)) 1) 2 4) 2)
	(+ (? (= (* 2 3) 6 (+ 3 2 1)) 1) 3 4))
	-> 0
*/

int eval(GUSANO *g, FILE *progfp) 
{
	TREE *expr;
	int res;
	
	if((expr=make_expr(g, progfp)) == NULL) 
	{
		res = -1;
	}
	else
	{
		
		res = eval_tree(expr);
		/*
		#ifdef DEBUGING
			map_tree(&debugFree,expr);
		#else
		*/
			map_tree(&free,expr);
		/* #endif */
		free_tree(expr);
	}
	
	return res;
}

TREE *make_expr(GUSANO *g, FILE *progfp) {
	char c;
	NODO *n;

retry:
	c = getc(progfp);

	if(c == EOF) return NULL;

	if(c >= '0' && c <= '9')
	{
		n = (NODO *)malloc(sizeof(NODO));
		n->type = CONSTANTE;
		n->val = c - '0';
		return make_tree(n, NULL);
	}
	
	if(c == '(')
	{
		c = getc(progfp);

		while(c == ' ') c = getc(progfp);
		
		n = (NODO *)malloc(sizeof(NODO));
		n->type = OPERADOR;
		setOperator(c,n);
		return make_tree(n, make_lista(g, progfp));
	}
	
	if(c == 'd'||c == 'm'||c == 'n'||c == 'c'||c == 'e'||c == 'h'||c == 'j'||c == 'k'||c == 'l') 
	{
		n = (NODO *)malloc(sizeof(NODO));
		n->type = CONSTANTE;
		n->val = getToken(g,c);
		return make_tree(n, NULL);
	}
	
	if(c == '#') 
	{
		/* Comentario */
		while (getc(progfp) != '\n');
	}
	
	goto retry;
}

LISTA *make_lista(GUSANO *g, FILE *progfp) {
	TREE *t;
	char c = getc(progfp);

	if(c == ')')
		return NULL;

	ungetc(c, progfp);
	t = make_expr(g,progfp);
	return cons_lista(t, make_lista(g,progfp));
}

int eval_tree(TREE *expr) {
    NODO *n;
		int ret = 0;

    if(expr != NULL)
		{
			n = (NODO *)getval_tree(expr);

			if (n->type == CONSTANTE) ret = n->val;

			else if (n->type == OPERADOR) ret = n->funcion(getchild_tree(expr));
		}
		return ret;
}

void setOperator(char c, NODO *n)
{
	switch (c)
	{
		case '+':
			n->funcion = &operarSuma;
			break;
		case '-':
			n->funcion = &operarResta;
			break;
		case '*':
			n->funcion = &operarMult;
			break;
		case '/':
			n->funcion = &operarDiv;
			break;
		case '>':
			n->funcion = &operarOrder;
			break;
		case '=':
			n->funcion = &operarEqual;
			break;
		case '~':
			n->funcion = &operarNot;
			break;
		case '?':
			n->funcion = &operarIf;
			break;
		case '&':
			n->funcion = &operarAnd;
			break;
		case '|':
			n->funcion = &operarOr;
			break;
		default:
			n->funcion = &operarNull;
			break;
	}
}

int operarSuma(LISTA *l)
{
	int res = 0;
	TREE *child;

	if(l == NULL) return res;

	for(child = first_lista(l); child != NULL; child = next_lista(l)) {
		res += eval_tree(child);
	}
	
	return res;
}

int operarResta(LISTA *l)
{
	int res = 0;
	TREE *child;

	if(l == NULL) return res;

	for(child = first_lista(l); child != NULL; child = next_lista(l)) {
		res -= eval_tree(child);
	}
	
	return res;
}

int operarMult(LISTA *l)
{
	int res = 1;
	TREE *child;

	if(l == NULL) return res;

	for(child = first_lista(l); child != NULL; child = next_lista(l)) {
		res *= eval_tree(child);
	}
	
	return res;
}

int operarDiv(LISTA *l)
{
	int res = 1;
	TREE *child;

	if ((l != NULL) && ((child = first_lista(l)) != NULL))
	{
		res = eval_tree(child);
	
		for(child = next_lista(l); child != NULL; child = next_lista(l)) {
			res /= eval_tree(child);
		}
	}
	return res;
}

int operarOrder(LISTA *l)
{
	int aux, t;
	TREE *child;

	if ((l != NULL) && ((child = first_lista(l)) != NULL)) 
	{
		aux = eval_tree(child);
	
		for(child = next_lista(l); child != NULL; child = next_lista(l)) {
			if (aux <= (t=eval_tree(child))) return 0; 
			
			aux = t;
		}
		
		return 1;
	}
	
	return 1;
}

int operarEqual(LISTA *l)
{
	int aux;
	TREE *child;

	if ((l != NULL) && ((child = first_lista(l)) != NULL)) 
	{
		aux = eval_tree(child);
	
		for(child = next_lista(l); child != NULL; child = next_lista(l)) {
			if (aux != eval_tree(child)) return 0; 
		}
		
		return 1;
	}

	return 0;
}

int operarNot(LISTA *l)
{
	TREE *child;

	if ((l != NULL) && ((child = first_lista(l)) != NULL)) 
	{
		return  !eval_tree(child);
	}
	
	return !0;
}

int operarIf(LISTA *l)
{
	int aux = 0;
	TREE *child;

	if ((l != NULL) && ((child = first_lista(l)) != NULL)) 
	{
		if(eval_tree(child))
		{
			child = next_lista(l);
			if (child != NULL) return eval_tree(child);
			else return 0;
		}
		else
		{
			child = next_lista(l);
			for(child = next_lista(l); child != NULL; child = next_lista(l)) {
				aux = eval_tree(child);
			}
		}
	
		return aux;
	}
	else
	{
		return 0;
	}
}	
int operarAnd(LISTA *l)
{
	TREE *child;
	
	if(l == NULL) return 0;

	for(child = first_lista(l); child != NULL; child = next_lista(l)) {
		if( !eval_tree(child) ) return 0;
	}

	return 1;
}
int operarOr(LISTA *l)
{
	TREE *child;
	
	if(l == NULL) return 1;

	for(child = first_lista(l); child != NULL; child = next_lista(l)) {
		if( eval_tree(child) ) return 1;
	}

	return 0;
}
int operarNull(LISTA *l)
{
	return 0;
}
