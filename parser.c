
#include "parser.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── lookahead ──────────────────────────────────────────────────── */
static Token lookahead;

/* ── consume one token ──────────────────────────────────────────── */
static void advance(void) {
    lookahead = next_token();
}

/* ── error: print message and halt ─────────────────────────────── */
static void syntax_error(const char *expected) {
    fprintf(stderr,
        "Syntax error at line %d: unexpected token \"%s\", expected %s\n",
        lookahead.line,
        lookahead.lexeme,
        expected);
    exit(1);
}

/* ── match expected token, else error ──────────────────────────── */
static void match(TokenType t) {
    if (lookahead.type == t) {
        advance();
    } else {
        char msg[80];
        snprintf(msg, sizeof(msg), "'%s'", token_type_str(t));
        syntax_error(msg);
    }
}

/* ── forward declarations ───────────────────────────────────────── */
static void parse_declaration_list(void);
static void parse_var_declaration(void);
static void parse_type_specifier(void);
static void parse_statement_list(void);
static void parse_statement(void);
static void parse_assignment_stmt(void);
static void parse_var(void);
static void parse_compound_stmt(void);
static void parse_selection_stmt(void);
static void parse_iteration_stmt(void);
static void parse_expression(void);
static void parse_additive_expression(void);
static void parse_term(void);
static void parse_factor(void);

/* ── predicates ─────────────────────────────────────────────────── */
static int is_type_specifier(void) {
    return lookahead.type == TK_INT || lookahead.type == TK_FLOAT;
}

/* First set of <statement>: ID  '{'  if  while */
static int is_statement_first(void) {
    TokenType t = lookahead.type;
    return t == TK_ID || t == TK_LBRACE || t == TK_IF || t == TK_WHILE;
}

/* ── Rule 1: program -> Program ID { declaration-list statement-list } */
void parse_program(void) {
    advance();              /* prime the lookahead */

    match(TK_PROGRAM);
    match(TK_ID);
    match(TK_LBRACE);

    parse_declaration_list();
    parse_statement_list();

    match(TK_RBRACE);

    if (lookahead.type != TK_EOF)
        syntax_error("end of file after '}'");

    printf("Parsing completed successfully.\n");
}

/* ── Rule 2: declaration-list -> var-declaration { var-declaration } */
static void parse_declaration_list(void) {
    if (!is_type_specifier())
        syntax_error("type specifier ('int' or 'float')");

    parse_var_declaration();
    while (is_type_specifier())
        parse_var_declaration();
}

/* ── Rule 3: var-declaration -> type-specifier ID ;
 *                             | type-specifier ID [ NUM ] ;  */
static void parse_var_declaration(void) {
    parse_type_specifier();
    match(TK_ID);
    if (lookahead.type == TK_LBRACKET) {
        advance();
        match(TK_NUM);
        match(TK_RBRACKET);
    }
    match(TK_SEMI);
}

/* ── Rule 4: type-specifier -> int | float */
static void parse_type_specifier(void) {
    if (is_type_specifier())
        advance();
    else
        syntax_error("'int' or 'float'");
}


static void parse_statement_list(void) {
    parse_statement();                      /* first statement required */

    while (lookahead.type == TK_SEMI) {
        advance();                          /* consume ';' separator     */
        if (!is_statement_first()) break;   /* tolerate trailing ';'     */
        parse_statement();
    }
}

/* ── Rule 6: statement -> assignment-stmt | compound-stmt
 *                       | selection-stmt  | iteration-stmt  */
static void parse_statement(void) {
    switch (lookahead.type) {
        case TK_ID:     parse_assignment_stmt(); break;
        case TK_LBRACE: parse_compound_stmt();   break;
        case TK_IF:     parse_selection_stmt();  break;
        case TK_WHILE:  parse_iteration_stmt();  break;
        default:
            syntax_error("a statement: ID, '{', 'if', or 'while'");
    }
}

/* ── Rule 7: assignment-stmt -> var = expression */
static void parse_assignment_stmt(void) {
    parse_var();
    match(TK_ASSIGN);
    parse_expression();
}

/* ── Rule 8: var -> ID | ID [ expression ] */
static void parse_var(void) {
    match(TK_ID);
    if (lookahead.type == TK_LBRACKET) {
        advance();
        parse_expression();
        match(TK_RBRACKET);
    }
}

/* ── Rule 9: compound-stmt -> { statement-list } */
static void parse_compound_stmt(void) {
    match(TK_LBRACE);
    parse_statement_list();
    match(TK_RBRACE);
}

/*
 * ── Rule 10: selection-stmt -> if ( expression ) statement
 *                             | if ( expression ) statement else statement
 *
 * Dangling-else resolved greedily: else binds to the nearest if.
 */
static void parse_selection_stmt(void) {
    match(TK_IF);
    match(TK_LPAREN);
    parse_expression();
    match(TK_RPAREN);
    parse_statement();

    if (lookahead.type == TK_ELSE) {
        advance();
        parse_statement();
    }
}

/* ── Rule 11: iteration-stmt -> while ( expression ) statement */
static void parse_iteration_stmt(void) {
    match(TK_WHILE);
    match(TK_LPAREN);
    parse_expression();
    match(TK_RPAREN);
    parse_statement();
}

/*
 * ── Rule 12: expression -> additive-expression relop additive-expression
 *                         | additive-expression
 * ── Rule 13: relop -> <= | < | > | >= | == | !=
 */
static void parse_expression(void) {
    parse_additive_expression();

    switch (lookahead.type) {
        case TK_LT: case TK_LTE:
        case TK_GT: case TK_GTE:
        case TK_EQ: case TK_NEQ:
            advance();
            parse_additive_expression();
            break;
        default: break;
    }
}

/* ── Rule 14: additive-expression -> term { addop term }
 * ── Rule 15: addop -> + | -                               */
static void parse_additive_expression(void) {
    parse_term();
    while (lookahead.type == TK_PLUS || lookahead.type == TK_MINUS) {
        advance();
        parse_term();
    }
}

/* ── Rule 16: term -> factor { mulop factor }
 * ── Rule 17: mulop -> * | /                  */
static void parse_term(void) {
    parse_factor();
    while (lookahead.type == TK_STAR || lookahead.type == TK_SLASH) {
        advance();
        parse_factor();
    }
}

/* ── Rule 18: factor -> ( expression ) | var | NUM */
static void parse_factor(void) {
    if (lookahead.type == TK_LPAREN) {
        advance();
        parse_expression();
        match(TK_RPAREN);
    } else if (lookahead.type == TK_ID) {
        parse_var();
    } else if (lookahead.type == TK_NUM) {
        advance();
    } else {
        syntax_error("a factor: '(' expression ')', variable name, or number");
    }
}
