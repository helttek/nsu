#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEMPLATE_MAX_SIZE 16
#define BUF_SIZE 1024

int main() {
    char template[TEMPLATE_MAX_SIZE + 1];
    if (fgets(template, TEMPLATE_MAX_SIZE, stdin) == NULL) {
        fprintf(stderr, "fgets returned an error.\n");
        return EXIT_FAILURE;
    }
    size_t templateSize = strlen(template);
    if (templateSize < TEMPLATE_MAX_SIZE || template[templateSize - 1] == '\n') {
        template[templateSize - 1] = '\0';
        --templateSize;
    }
    if (templateSize == TEMPLATE_MAX_SIZE) {
        if (getchar() != '\n') {
            return EXIT_FAILURE;
        }
    }
    char buffer[BUF_SIZE];
    while (!fread(buffer, BUF_SIZE, 1, stdin)) {
    }
    return EXIT_SUCCESS;
}
