//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *plik = fopen(argv[1], "r");
    char linia[1024];
    int max_dlugosc = 0;
    int numer_linii = 0;
    int najdluzsza = 0;

    while (fgets(linia, sizeof(linia), plik) != NULL) {
        numer_linii++;
        int dlugosc = strlen(linia);
        if (linia[dlugosc - 1] == '\n')
            dlugosc--;
        if (dlugosc > max_dlugosc) {
            max_dlugosc = dlugosc;
            najdluzsza = numer_linii;
        }
    }

    fclose(plik);

    if (max_dlugosc == 0)
        printf("plik jest pusty lub nie zawiera pełnych linii.\n");
    else
        printf("najdłuższa: %d (długość: %d znaków)\n", najdluzsza, max_dlugosc);

    return 0;
}