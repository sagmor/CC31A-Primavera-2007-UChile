/*
 * Lista simplemente enlazada
 */
#ifndef LISTA_H
	#define LISTA_H

	struct nodo {
		struct nodo *next;
		void *value;
		};

	typedef struct {
		struct nodo *header;
		struct nodo *current;
		} LISTA;
	

	/* inicializa una nueva lista vacía */
	LISTA *init_l();
	
	/* agrega el elemento p al comienzo de la lista l */
	LISTA *cons_l(void *p, LISTA *l);

	/* Retorna el elemento más nuevo de la lista y se posiciona al comienzo */
	void *first_l(LISTA *l);

	/* Retorna el elemento siguiente de la lista */
	void *next_l(LISTA *l);

	/* Libera la lista completa */
	void free_l(LISTA *l);
	
#endif
