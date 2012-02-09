#include <stdio.h>
#include "arbol.h"

/* Evalua expresiones a la lisp (numeros de un solo digito) como:
   (+ 1 2)
   -> 3
   (* 2 3)
   -> 6
   (+ 1 (* 3 2) 4)
   -> 11
   (+12)      ; es como (+ 1 2)
   -> 3
*/
TREE *make_expr();

LISTA *make_l() {
    TREE *t;
    char c = getchar();

    if(c == ')')
	return NULL;
    
    ungetc(c, stdin);
    t = make_expr();
    return cons_l(t, make_l());
}

TREE *make_expr() {
    char c;
    LISTA *l;

retry:
    c = getchar();
    if(c == EOF) return NULL;

    if(c >= '0' && c <= '9')
	return make_tree(c, NULL);

    if(c == '(') {
   	c = getchar();
	return make_tree(c, make_l());
    }
    goto retry;
}

int eval_op(char op, int res, int val) {
    switch(op) {
	case '+': return (res + val);
	case '*': return (res * val); 
	case '/': return (res / val); 
	case '-': return (res - val); 
	default: return -1;
    }
}

int eval_tree(TREE *expr) {
    LISTA *l;
    int res;
    TREE *child;
    char op;
    int first = 1;

    if(expr == NULL) return 0;

    op = getval_tree(expr);
    l = getchild_tree(expr);

    if(op >= '0' && op <= '9') return op-'0';  /* Hoja */

    if(op == '+' || op == '-') res = 0;
    else res = 1;

    if(l == NULL) return res;

    for(child = first_l(l); child != NULL; child = next_l(l)) {
	if(first && op == '/') {
	    res = eval_tree(child);
	    first = 0;
	}
	else
	    res = eval_op(op, res, eval_tree(child)); 
    }
  
    return res;
}

main() {
    TREE *expr;

    while((expr=make_expr()) != NULL) {
	printf("-> %d\n", eval_tree(expr));
        free_tree(expr);
    }
}
