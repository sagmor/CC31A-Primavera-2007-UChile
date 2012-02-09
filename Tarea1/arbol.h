#include "lista.h"

#ifndef ARBOL_HEADER_SEEN
	#define ARBOL_HEADER_SEEN
	
	typedef struct {
		char value;
		LISTA *childs;
		} TREE;

	/* Crea un árbol con val en la raíz y con esa lista de hijos */
	TREE *make_tree(char val, LISTA *hijos);

	/* Obtiene el campo valor de la raíz del árbol */
	char getval_tree(TREE *t);

	/* Obtiene la lista de hijos de la raíz del árbol */
	LISTA *getchild_tree(TREE *t);

	/* Libera todo el árbol */
	void free_tree(TREE *t);

#endif
