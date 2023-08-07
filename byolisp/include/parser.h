#ifndef PARSER_H
#define PARSER_H

#include "mpc.h"

typedef struct {
    mpc_parser_t *Number;
    mpc_parser_t *Boolean;
    mpc_parser_t *Symbol;
    mpc_parser_t *SExpr;
    mpc_parser_t *QExpr;
    mpc_parser_t *Expr;
    mpc_parser_t *BYOLisp;
} parser_grammar_t;

typedef struct {
    mpc_result_t result_data;
    int result_flag;
} parser_result_t;

parser_grammar_t parser_build_grammar();
parser_result_t parser_parse(const char *input, parser_grammar_t grammar);
void parser_free_grammar(parser_grammar_t grammar);
void parser_report_output(parser_result_t result);

#endif // PARSER_H