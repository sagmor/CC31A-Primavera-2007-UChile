struct nodo {
    struct nodo *next;
    void *val;
};

typedef struct {
	struct nodo *t, *h;
} LISTAS;

LISTAS *init_l();
void add_l(LISTAS *, void *);
void *extract_l(LISTAS *);
int size_l(LISTAS *);
void *tail_l(LISTAS *);
void *head_l(LISTAS *);
