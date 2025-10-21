//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {

    FILE *plik = fopen(argv[1], "rb");
    int znak;
    int tekstowy = 1;
    while ((znak = fgetc(plik)) != EOF) {
        if (!isascii(znak)) {
            tekstowy = 0;
            break;
        }
    }
    fclose(plik);
    if (tekstowy)
        printf("plik '%s' jest plikiem tekstowym\n", argv[1]);
    else
        printf("plik '%s' NIE jest plikiem tekstowym\n", argv[1]);

    return 0;
}