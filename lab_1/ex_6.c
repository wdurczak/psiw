//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINIA 1024
#define MAX_LINII 1000

int main(int argc, char *argv[]) {

    FILE *plik = fopen(argv[1], "r");

    if (strcmp(argv[2], "znaki") == 0) {
        if (fseek(plik, 0, SEEK_END) != 0) {
            perror("błąd ustawiania pozycji");
            fclose(plik);
            return 1;
        }

        long pozycja = ftell(plik);
        for (long i = pozycja - 1; i >= 0; i--) {
            fseek(plik, i, SEEK_SET);
            int znak = fgetc(plik);
            if (znak != EOF)
                putchar(znak);
        }
    }
    else if (strcmp(argv[2], "linie") == 0) {
        char *linie[MAX_LINII];
        int liczba_linii = 0;
        char bufor[MAX_LINIA];

        while (fgets(bufor, sizeof(bufor), plik) != NULL && liczba_linii < MAX_LINII) {
            linie[liczba_linii] = strdup(bufor);
            liczba_linii++;
        }

        for (int i = liczba_linii - 1; i >= 0; i--) {
            printf("%s", linie[i]);
            free(linie[i]);
        }
    }else {
        printf("nieznany tryb: %s\n", argv[2]);
        printf(" tryby: znaki, linie\n");
        fclose(plik);
        return 1;
    }

    fclose(plik);
    return 0;
}