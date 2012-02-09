#include <stdlib.h>
#include "arbol.h"


/* Crea un árbol con val en la raíz y con esa lista de hijos */
TREE *make_tree(char val, LISTA *hijos)
{
	TREE *t = (TREE *)malloc(sizeof(TREE));
	
	t->value = val;
	t->childs = hijos;
	
	return t;
}

/* Obtiene el campo valor de la raíz del árbol */
char getval_tree(TREE *t)
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
	if(t->childs != NULL)
	{
		TREE *c = (TREE *)first_l(t->childs);
		while(c != NULL)
		{
			free_tree(c);
			c = (TREE *)next_l(t->childs);
		}
		free_l(t->childs);
	}
	free(t);
}
