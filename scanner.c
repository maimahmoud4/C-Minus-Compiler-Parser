#include "scanner.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ── internal state ─────────────────────────────────────────────── */
static FILE *src_file   = NULL;
static int   cur_line   = 1;

static int   pb_valid   = 0;   /* single-char pushback          */
static int   pb_char    = 0;

static int   peek_valid = 0;   /* single-token lookahead buffer */
static Token peek_buf;

/* ── low-level character I/O ────────────────────────────────────── */
static int gc(void) {
    int c;
    if (pb_valid) { pb_valid = 0; c = pb_char; }
    else           c = fgetc(src_file);
    if (c == '\n') cur_line++;
    return c;
}

static void ugc(int c) {
    pb_char = c; pb_valid = 1;
    if (c == '\n') cur_line--;
}

/* ── keyword table ──────────────────────────────────────────────── */
typedef struct { const char *word; TokenType type; } KWEntry;
static KWEntry kw_table[] = {
    { "Program", TK_PROGRAM },
    { "int",     TK_INT     },
    { "float",   TK_FLOAT   },
    { "if",      TK_IF      },
    { "else",    TK_ELSE    },
    { "while",   TK_WHILE   },
    { NULL,      TK_ERROR   }
};
static TokenType lookup_kw(const char *s) {
    for (int i = 0; kw_table[i].word; i++)
        if (strcmp(s, kw_table[i].word) == 0) return kw_table[i].type;
    return TK_ID;
}

/* ── token name helper (used in error messages) ─────────────────── */
const char *token_type_str(TokenType t) {
    switch (t) {
        case TK_PROGRAM:  return "Program";
        case TK_INT:      return "int";
        case TK_FLOAT:    return "float";
        case TK_IF:       return "if";
        case TK_ELSE:     return "else";
        case TK_WHILE:    return "while";
        case TK_ID:       return "ID";
        case TK_NUM:      return "NUM";
        case TK_PLUS:     return "+";
        case TK_MINUS:    return "-";
        case TK_STAR:     return "*";
        case TK_SLASH:    return "/";
        case TK_LT:       return "<";
        case TK_LTE:      return "<=";
        case TK_GT:       return ">";
        case TK_GTE:      return ">=";
        case TK_EQ:       return "==";
        case TK_NEQ:      return "!=";
        case TK_ASSIGN:   return "=";
        case TK_LPAREN:   return "(";
        case TK_RPAREN:   return ")";
        case TK_LBRACKET: return "[";
        case TK_RBRACKET: return "]";
        case TK_LBRACE:   return "{";
        case TK_RBRACE:   return "}";
        case TK_SEMI:     return ";";
        case TK_EOF:      return "EOF";
        default:          return "INVALID";
    }
}

/* ── core scan ──────────────────────────────────────────────────── */
static Token scan(void) {
    Token tok;
    int   c;

    /* skip whitespace and block comments */
    for (;;) {
        c = gc();
        if (c == EOF) {
            tok.type = TK_EOF;
            snprintf(tok.lexeme, MAX_LEXEME_LEN, "EOF");
            tok.line = cur_line;
            return tok;
        }
        if (isspace(c)) continue;

        /* block comment handler */
        if (c == '/') {
            int nc = gc();
            if (nc == '*') {
                int prev = 0;
                for (;;) {
                    int cc = gc();
                    if (cc == EOF) {
                        fprintf(stderr, "Error line %d: unterminated comment\n", cur_line);
                        tok.type = TK_EOF;
                        snprintf(tok.lexeme, MAX_LEXEME_LEN, "EOF");
                        tok.line = cur_line;
                        return tok;
                    }
                    if (prev == '*' && cc == '/') break;
                    prev = cc;
                }
                continue;
            }
            ugc(nc);   /* not a comment – put nc back; '/' is an operator */
        }
        break;
    }

    tok.line = cur_line;

    /* identifiers / keywords */
    if (isalpha(c) || c == '_') {
        int i = 0;
        tok.lexeme[i++] = (char)c;
        while ((c = gc()) != EOF && (isalnum(c) || c == '_'))
            if (i < MAX_LEXEME_LEN - 1) tok.lexeme[i++] = (char)c;
        if (c != EOF) ugc(c);
        tok.lexeme[i] = '\0';
        tok.type = lookup_kw(tok.lexeme);
        return tok;
    }

    /* integer / floating-point numbers */
    if (isdigit(c)) {
        int i = 0;
        tok.lexeme[i++] = (char)c;
        while ((c = gc()) != EOF && isdigit(c))
            if (i < MAX_LEXEME_LEN - 1) tok.lexeme[i++] = (char)c;
        if (c == '.') {
            if (i < MAX_LEXEME_LEN - 1) tok.lexeme[i++] = '.';
            while ((c = gc()) != EOF && isdigit(c))
                if (i < MAX_LEXEME_LEN - 1) tok.lexeme[i++] = (char)c;
        }
        if (c != EOF) ugc(c);
        tok.lexeme[i] = '\0';
        tok.type = TK_NUM;
        return tok;
    }

    /* operators and punctuation */
    tok.lexeme[0] = (char)c; tok.lexeme[1] = '\0';
    switch (c) {
        case '+': tok.type = TK_PLUS;     return tok;
        case '-': tok.type = TK_MINUS;    return tok;
        case '*': tok.type = TK_STAR;     return tok;
        case '/': tok.type = TK_SLASH;    return tok;
        case '(': tok.type = TK_LPAREN;   return tok;
        case ')': tok.type = TK_RPAREN;   return tok;
        case '[': tok.type = TK_LBRACKET; return tok;
        case ']': tok.type = TK_RBRACKET; return tok;
        case '{': tok.type = TK_LBRACE;   return tok;
        case '}': tok.type = TK_RBRACE;   return tok;
        case ';': tok.type = TK_SEMI;     return tok;
        case '<':
            c = gc();
            if (c == '=') { tok.type = TK_LTE; snprintf(tok.lexeme,MAX_LEXEME_LEN,"<="); }
            else           { if (c!=EOF) ugc(c); tok.type = TK_LT; }
            return tok;
        case '>':
            c = gc();
            if (c == '=') { tok.type = TK_GTE; snprintf(tok.lexeme,MAX_LEXEME_LEN,">="); }
            else           { if (c!=EOF) ugc(c); tok.type = TK_GT; }
            return tok;
        case '=':
            c = gc();
            if (c == '=') { tok.type = TK_EQ; snprintf(tok.lexeme,MAX_LEXEME_LEN,"=="); }
            else           { if (c!=EOF) ugc(c); tok.type = TK_ASSIGN; }
            return tok;
        case '!':
            c = gc();
            if (c == '=') { tok.type = TK_NEQ; snprintf(tok.lexeme,MAX_LEXEME_LEN,"!="); }
            else {
                if (c!=EOF) ugc(c);
                tok.type = TK_ERROR;
                fprintf(stderr,"Lexical error line %d: unexpected '!'\n", cur_line);
            }
            return tok;
        default:
            tok.type = TK_ERROR;
            fprintf(stderr,"Lexical error line %d: unrecognised character '%c'\n",
                    cur_line, tok.lexeme[0]);
            return tok;
    }
}

/* ── public API ─────────────────────────────────────────────────── */
void scanner_init(FILE *src) {
    src_file = src; cur_line = 1; pb_valid = 0; peek_valid = 0;
}
Token next_token(void) {
    if (peek_valid) { peek_valid = 0; return peek_buf; }
    return scan();
}
Token peek_token(void) {
    if (!peek_valid) { peek_buf = scan(); peek_valid = 1; }
    return peek_buf;
}
