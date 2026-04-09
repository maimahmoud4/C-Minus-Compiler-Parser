#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    /* Keywords */
    TK_PROGRAM = 0,
    TK_INT,
    TK_FLOAT,
    TK_IF,
    TK_ELSE,
    TK_WHILE,

    /* Identifiers and literals */
    TK_ID,
    TK_NUM,

    /* Arithmetic operators */
    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,

    /* Relational operators */
    TK_LT,
    TK_LTE,
    TK_GT,
    TK_GTE,
    TK_EQ,
    TK_NEQ,

    /* Assignment */
    TK_ASSIGN,

    /* Delimiters */
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_LBRACE,
    TK_RBRACE,
    TK_SEMI,

    /* Special */
    TK_EOF,
    TK_ERROR
} TokenType;

#define MAX_LEXEME_LEN 256

typedef struct {
    TokenType type;
    char      lexeme[MAX_LEXEME_LEN];
    int       line;
} Token;

const char *token_type_str(TokenType t);

#endif /* TOKENS_H */
