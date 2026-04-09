#ifndef SCANNER_H
#define SCANNER_H

#include "tokens.h"
#include <stdio.h>

void  scanner_init(FILE *src);
Token next_token(void);
Token peek_token(void);

#endif /* SCANNER_H */
