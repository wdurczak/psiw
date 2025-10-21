//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *plik = fopen(argv[1], "r");
    if (plik == NULL) {
        perror("błąd otwarcia pliku");
        return 1;
    }
    char linia[1024];
    while (fgets(linia, sizeof(linia), plik)) {
        int len = strlen(linia);
        if (linia[len - 1] == '\n') len--;
        for (int i = len - 1; i >= 0; i--)
            putchar(linia[i]);
        putchar('\n');
    }
    fclose(plik);
    return 0;
}