#include "lista.h"

#ifndef __ARBOL_H__
	#define __ARBOL_H__
		
	typedef struct {
		void *value;
		LISTA *childs;
		} TREE;

	/* Crea un árbol con val en la raíz y con esa lista de hijos */
	TREE *make_tree(void *val, LISTA *hijos);

	/* Obtiene el campo valor de la raíz del árbol */
	void *getval_tree(TREE *t);

	/* Obtiene la lista de hijos de la raíz del árbol */
	LISTA *getchild_tree(TREE *t);

	/* Libera todo el árbol */
	void free_tree(TREE *t);
	
	/* Aplica una funcion a todos los elementos del árbol */
	void map_tree(void (*func)(void *), TREE *t);
#endif
