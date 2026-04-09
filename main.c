#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open '%s'\n", argv[1]);
        return 1;
    }
    scanner_init(fp);
    parse_program();
    fclose(fp);
    return 0;
}
