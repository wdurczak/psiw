//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *wzorzec = argv[1];
    for (int i = 2; i < argc; i++) {
        FILE *plik = fopen(argv[i], "r");
        if (plik == NULL) {
            perror(argv[i]);
            continue;
        }
        char linia[1024];
        int nr_linii = 0;
        int znaleziono = 0;
        while (fgets(linia, sizeof(linia), plik)) {
            nr_linii++;
            char *pos = linia;
            while ((pos = strstr(pos, wzorzec)) != NULL) {
                int pozycja = (int)(pos - linia) + 1;
                printf("%s: linia %d, znak %d\n", argv[i], nr_linii, pozycja);
                pos += strlen(wzorzec);
                znaleziono = 1;
            }
        }
        if (!znaleziono)
            printf("%s: brak wystąpień\n", argv[i]);

        fclose(plik);
    }
    return 0;
}