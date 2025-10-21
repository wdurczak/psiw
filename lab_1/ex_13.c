//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *wzorzec = argv[1];
    char linia[1024];
    while (fgets(linia, sizeof(linia), stdin)) {
        if (strstr(linia, wzorzec) != NULL)
            fputs(linia, stdout);
    }

    return 0;
}