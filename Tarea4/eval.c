#include <stdio.h>
#include <stdlib.h>
#include "arbol.h"
#include "eval.h"
#include "gusano.h"
#include <unistd.h>
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

int eval(GUSANO *g) 
{
	TREE *expr;
	int res;
	/*
	debug("Starting eval()\n");
	debug("Worm direction: %d\n",g->dir);
	debug("Worm position x: %d\n",((POINT *)tail_l(g->cuerpo))->x);
	debug("Worm position y: %d\n",((POINT *)tail_l(g->cuerpo))->y);
	*/
	if((expr=make_expr(g)) == NULL) 
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
	/*
	debug("eval() returned: %d\n",res);
	*/
	return res;
}

TREE *make_expr(GUSANO *g) {
	char c;
	NODO *n;

retry:
	c = getchar();
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
		c = getchar();
		
		while(c == ' ') c = getchar();
		
		n = (NODO *)malloc(sizeof(NODO));
		n->type = OPERADOR;
		setOperator(c,n);
		return make_tree(n, make_lista(g));
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
		while (getchar() != '\n');
	}
	
	goto retry;
}

LISTA *make_lista(GUSANO *g) {
    TREE *t;
    char c = getchar();

    if(c == ')')
	return NULL;
    
    ungetc(c, stdin);
    t = make_expr(g);
    return cons_lista(t, make_lista(g));
}

int eval_tree(TREE *expr) {
    NODO *n;

    if(expr == NULL) return 0;

    n = (NODO *)getval_tree(expr);

		if (n->type == CONSTANTE) return n->val;
		
		if (n->type == OPERADOR) return n->funcion(getchild_tree(expr));
		
		return 0;
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
