//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    FILE *f1 = fopen(argv[1], "rb");
    if (!f1) { perror(argv[1]); return 1; }
    FILE *f2 = fopen(argv[2], "rb");
    if (!f2) { perror(argv[2]); fclose(f1); return 1; }
    int c1, c2;
    unsigned long long pos = 1;
    unsigned long long line = 1;
    unsigned long long col  = 1;
    for (;;) {
        c1 = fgetc(f1);
        c2 = fgetc(f2);

        if (c1 == c2) {
            if (c1 == EOF) {
                puts("pliki są identyczne");
                break;
            }
            if (c1 == '\n') { line++; col = 1; }
            else { col++; }
            pos++;
            continue;
        }
        if (c1 == EOF && c2 != EOF) {
            unsigned long long extra = 1;
            while ((c2 = fgetc(f2)) != EOF) extra++;
            printf("plik %s zawiera %llu znaków więcej niż zawartość pliku %s\n",
                   argv[2], extra, argv[1]);
            break;
        } else if (c2 == EOF && c1 != EOF) {
            unsigned long long extra = 1;
            while ((c1 = fgetc(f1)) != EOF) extra++;
            printf("plik %s zawiera %llu znaków więcej niż zawartość pliku %s\n",
                   argv[1], extra, argv[2]);
            break;
        } else {
            printf("pliki różnią się od znaku nr %llu w linii %llu (znak w linii: %llu)\n",
                   pos, line, col);
            break;
        }
    }
    fclose(f1);
    fclose(f2);
    return 0;
}