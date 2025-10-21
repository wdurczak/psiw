//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *src = fopen(argv[1], "r");
    FILE *dst = fopen(argv[2], "w");
    int znak;
    while ((znak = fgetc(src)) != EOF) {
        fputc(znak, dst);
    }
    fclose(src);
    fclose(dst);
    printf("zawartość pliku skopiowana %s do %s\n", argv[1], argv[2]);
    return 0;
}