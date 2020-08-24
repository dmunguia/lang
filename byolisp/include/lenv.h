#ifndef LENV_H
#define LENV_H

#include <stdbool.h>
#include "sexpr.h"

struct lval_s;
typedef struct lval_s lval_t;

struct lenv_s;
typedef struct lenv_s lenv_t;

struct lenv_s {
    lenv_t* parent;
    int count;
    char** symbols;
    lval_t** values;
    bool* is_builtin;
};

lenv_t* lenv_new();
void lenv_free(lenv_t* lenv);
lval_t* lenv_get(lenv_t* lenv, char* key);
bool lenv_put(lenv_t* lenv, char* key, lval_t* value);
bool lenv_def(lenv_t* lenv, char* key, lval_t* value);
lenv_t* lenv_copy(lenv_t* lenv);
void lenv_add_builtins(lenv_t* lenv);

#endif // LENV_H