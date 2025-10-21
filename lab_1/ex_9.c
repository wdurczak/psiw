//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    FILE *plik;
    if (argc == 2) {
        plik = fopen(argv[1], "r");
    } else {
        plik = stdin;
    }
    int znak;
    while ((znak = fgetc(plik)) != EOF) {
        putchar(toupper(znak));
    }
    if (plik != stdin)
        fclose(plik);
    return 0;
}