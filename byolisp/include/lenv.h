#ifndef LENV_H
#define LENV_H

#include "sexpr.h"

struct lval_s;
typedef struct lval_s lval_t;

struct lenv_s;
typedef struct lenv_s lenv_t;

struct lenv_s {
    int count;
    char** symbols;
    lval_t** values;
};

lenv_t* lenv_new();
void lenv_free(lenv_t* lenv);
lval_t* lenv_get(lenv_t* lenv, char* key);
void lenv_put(lenv_t* lenv, char* key, lval_t* value);
void lenv_add_builtins(lenv_t* lenv);

#endif // LENV_H