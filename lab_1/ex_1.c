//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>

int main(int argc, char *argv[]) {
    FILE *plik = fopen(argv[1], "w");
    fprintf(plik, "%s", argv[2]);
    fclose(plik);
    printf("zapisano: %s\n", argv[1]);
    return 0;
}