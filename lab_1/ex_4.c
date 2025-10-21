//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *plik = fopen(argv[1], "r");
    long rozmiar = ftell(plik);
    for (long i = rozmiar - 1; i >= 0; i--) {
        fseek(plik, i, SEEK_SET);
        int znak = fgetc(plik);
        if (znak != EOF)
            putchar(znak);
    }
    fclose(plik);
    return 0;
}