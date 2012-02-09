#include <stdlib.h>
#include "lista.h"
	
	/* inicializa una nueva lista vacía */
	LISTA *init_lista()
	{
		LISTA *l = (LISTA *)malloc(sizeof(LISTA));
		
		l->header = NULL;
		l->current = NULL;
		
		return l;
	}
	
	/* agrega el elemento p al comienzo de la lista l */
	LISTA *cons_lista(void *p, LISTA *l)
	{	
		struct nodo_lista *n;
		
		if(l==NULL)
		{
			l = init_lista();
		}
		n = l->header;
		l->header = (struct nodo_lista *)malloc(sizeof(struct nodo_lista));
		
		if (l->header == NULL) return NULL;
		
		l->header->value = p;
		l->header->next = n;
		
		return l;
	}

	/* Retorna el elemento más nuevo de la lista y se posiciona al comienzo */
	void *first_lista(LISTA *l)
	{
		if (l->header == NULL) return NULL;
		
		l->current = l->header;
		
		return l->header->value;
	}

	/* Retorna el elemento siguiente de la lista */
	void *next_lista(LISTA *l)
	{
		if (l->current == NULL || l->current->next == NULL) return NULL;
		
		l->current = l->current->next;
		
		return l->current->value;
	}

	/* Libera la lista completa */
	void free_lista(LISTA *l)
	{
		struct nodo_lista *n = l->header;
		struct nodo_lista *p;
		
		while(n != NULL)
		{
			p = n;
			n = n->next;
			free(p);
		}
		
		free(l);
	}
