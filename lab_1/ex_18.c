//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char katalog[1024];
    struct dirent **lista;
    int n;
    if (argc == 1) {
        if (getcwd(katalog, sizeof(katalog)) == NULL) {
            perror("błąd getcwd");
            return 1;
        }
    } else {
        strncpy(katalog, argv[1], sizeof(katalog) - 1);
        katalog[sizeof(katalog) - 1] = '\0';
    }
    n = scandir(katalog, &lista, NULL, alphasort);
    if (n < 0) {
        perror("błąd odczytu katalogu");
        return 1;
    }
    printf("zawartość katalogu: %s\n", katalog);
    for (int i = 0; i < n; i++) {
        printf("%s\n", lista[i]->d_name);
        free(lista[i]);
    }
    free(lista);
    return 0;
}