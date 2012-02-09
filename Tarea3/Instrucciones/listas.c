/* Lista simplemente enlazada para almacenar cualquier cosa
 * se agrega por un lado y se extrae por el otro 
 */

#include "listas.h"
#include <stdio.h>
#include <stdlib.h>

LISTAS *init_l() {
    LISTAS *l;

    l = (LISTAS *)malloc(sizeof(LISTAS));
    if(l == NULL) return l;
    l->t = l->h = NULL;
    return l;
}

void add_l(LISTAS *l, void *val) {
   struct nodo *p = (struct nodo *)malloc(sizeof(struct nodo));
   p->next = NULL; 
   if(l->t != NULL)
	l->t->next = p;
   else
	l->h = p;
   l->t = p;
   p->val = val;
   return;
}

void *extract_l(LISTAS *l) {
   struct nodo *p;
   void *r;

   if(l->h == NULL) return NULL;

   p = l->h;
   l->h = p->next;
   r = p->val;
   free(p);
   if(l->h == NULL) l->t = NULL;
   return r;
}

int size_l(LISTAS *l) {
   struct nodo *p;
   int i;

   for(i=0, p=l->h; p!=NULL; i++, p=p->next)
	;

   return i;
}

void *head_l(LISTAS *l) {
    if(l->h == NULL) return NULL;

    return l->h->val;
}

void *tail_l(LISTAS *l) {
    if(l->t == NULL) return NULL;

    return l->t->val;
}
