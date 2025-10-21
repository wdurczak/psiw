//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int is_blank_line(const char *s) {
    for (; *s; ++s) if (!isspace((unsigned char)*s)) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    char *endp = NULL;
    long width = strtol(argv[1], &endp, 10);
    if (!endp || *endp || width <= 0) {
        fprintf(stderr, "błędna szerokość: %s\n", argv[1]);
        return 1;
    }
    const size_t W = (size_t)width;
    char line[4096];
    for (int a = 2; a < argc; ++a) {
        FILE *f = fopen(argv[a], "r");
        if (!f) { perror(argv[a]); continue; }

        size_t cur = 0;

        while (fgets(line, sizeof(line), f)) {
            if (is_blank_line(line)) {
                if (cur > 0) { putchar('\n'); cur = 0; }
                putchar('\n');
                continue;
            }

            const char *p = line;
            while (*p) {

                while (*p && isspace((unsigned char)*p)) ++p;
                if (!*p) break;


                char word[4096];
                size_t wlen = 0;
                while (*p && !isspace((unsigned char)*p)) {
                    if (wlen + 1 < sizeof(word)) word[wlen++] = *p;
                    ++p;
                }
                word[wlen] = '\0';

                if (cur == 0) {
                    fputs(word, stdout);
                    cur = wlen;
                } else if (cur + 1 + wlen <= W) {
                    putchar(' ');
                    fputs(word, stdout);
                    cur += 1 + wlen;
                } else {
                    putchar('\n');
                    fputs(word, stdout);
                    cur = wlen;
                }


            }
        }

        if (cur > 0) { putchar('\n'); }
        fclose(f);
        if (a + 1 < argc) putchar('\n');
    }

    return 0;
}