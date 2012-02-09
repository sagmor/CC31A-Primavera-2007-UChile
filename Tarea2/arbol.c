#include <stdlib.h>
#include "lista.h"
#include "arbol.h"


/* Crea un árbol con val en la raíz y con esa lista de hijos */
TREE *make_tree(void *val, LISTA *hijos)
{
	TREE *t = (TREE *)malloc(sizeof(TREE));
	
	t->value = val;
	t->childs = hijos;
	
	return t;
}

/* Obtiene el campo valor de la raíz del árbol */
void *getval_tree(TREE *t)
{
	return t->value;
}

/* Obtiene la lista de hijos de la raíz del árbol */
LISTA *getchild_tree(TREE *t)
{
	return t->childs;
}

/* Libera todo el árbol */
void free_tree(TREE *t)
{
	TREE *c;
	
	if(t->childs != NULL)
	{
		c = (TREE *)first_l(t->childs);
		while(c != NULL)
		{
			free_tree(c);
			c = (TREE *)next_l(t->childs);
		}
		free_l(t->childs);
	}
	free(t);
}

/* Aplica una funcion a todos los elementos del árbol */
void map_tree(void (*func)(void *), TREE *t)
{
	TREE *c;
		
	if(t->childs != NULL)
	{
		c = (TREE *)first_l(t->childs);
		while(c != NULL)
		{
			map_tree(func, c);
			c = (TREE *)next_l(t->childs);
		}
	}
	(*func)(t->value);	
}
