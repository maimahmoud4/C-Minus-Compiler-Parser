#ifndef PARSER_H
#define PARSER_H

/*
 * Top-level entry point.
 * Reads tokens from the scanner, validates the full program.
 * Prints a syntax-error message and exits(1) on the first error.
 * Prints "Parsing completed successfully." on success.
 */
void parse_program(void);

#endif /* PARSER_H */
