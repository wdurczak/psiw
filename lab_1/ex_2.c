//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *plik = fopen(argv[1], "r+");
    int znak;
    long pozycja;

    while ((znak = fgetc(plik)) != EOF) {
        if (znak == 'x') {
            pozycja = ftell(plik);
            fseek(plik, pozycja - 1, SEEK_SET);
            fputc('X', plik);
            fseek(plik, 0, SEEK_CUR);
        }
    }
    fclose(plik);
    printf("wykonano pomyslnie %s\n", argv[1]);
    return 0;
}