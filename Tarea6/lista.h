/*
 * Lista simplemente enlazada
 */
#ifndef __LISTA_H__
	#define __LISTA_H__

	struct nodo_lista {
		struct nodo_lista *next;
		void *value;
		};
		
	typedef struct {
		struct nodo_lista *header;
		struct nodo_lista *current;
		} LISTA;
	

	/* inicializa una nueva lista vacía */
	LISTA *init_lista();
	
	/* agrega el elemento p al comienzo de la lista l */
	LISTA *cons_lista(void *p, LISTA *l);

	/* Retorna el elemento más nuevo de la lista y se posiciona al comienzo */
	void *first_lista(LISTA *l);

	/* Retorna el elemento siguiente de la lista */
	void *next_lista(LISTA *l);

	/* Libera la lista completa */
	void free_lista(LISTA *l);
	
#endif
