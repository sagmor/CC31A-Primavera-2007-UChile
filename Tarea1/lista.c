#include <stdlib.h>
#include "lista.h"

	/* inicializa una nueva lista vacía */
	LISTA *init_l()
	{
		LISTA *l = (LISTA *)malloc(sizeof(LISTA));
		
		l->header = NULL;
		l->current = NULL;
		
		return l;
	}
	
	/* agrega el elemento p al comienzo de la lista l */
	LISTA *cons_l(void *p, LISTA *l)
	{	
		struct nodo *n;
		
		if(l==NULL)
		{
			l = init_l();
		}
		n = l->header;
		l->header = (struct nodo *)malloc(sizeof(struct nodo));
		
		if (l->header == NULL) return NULL;
		
		l->header->value = p;
		l->header->next = n;
		
		return l;
	}

	/* Retorna el elemento más nuevo de la lista y se posiciona al comienzo */
	void *first_l(LISTA *l)
	{
		if (l->header == NULL) return NULL;
		
		l->current = l->header;
		
		return l->header->value;
	}

	/* Retorna el elemento siguiente de la lista */
	void *next_l(LISTA *l)
	{
		if (l->current == NULL || l->current->next == NULL) return NULL;
		
		l->current = l->current->next;
		
		return l->current->value;
	}

	/* Libera la lista completa */
	void free_l(LISTA *l)
	{
		struct nodo *n = l->header;
		struct nodo *p;
		
		while(n != NULL)
		{
			p = n;
			n = n->next;
			free(p);
		}
		
		free(l);
	}
