//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static long start_last_chars(FILE *f, int n) {
    if (fseek(f, 0, SEEK_END)) return -1;
    long sz = ftell(f); if (sz < 0) return -1;
    long s = sz - n; return s < 0 ? 0 : s;
}

static long start_last_lines(FILE *f, int n) {
    if (fseek(f, 0, SEEK_END)) return -1;
    long i = ftell(f); if (i <= 0) return 0;
    int cnt = 0;
    for (i = i - 1; i >= 0 && cnt < n; --i) {
        if (fseek(f, i, SEEK_SET)) return -1;
        if (fgetc(f) == '\n') cnt++;
    }
    return (cnt < n) ? 0 : i + 2;

}

static long start_last_words(FILE *f, int n) {
    if (fseek(f, 0, SEEK_END)) return -1;
    long i = ftell(f); if (i <= 0) return 0;
    i--;
    for (; i >= 0; --i) { if (fseek(f, i, SEEK_SET)) return -1; if (!isspace(fgetc(f))) break; }
    int cnt = 0, inword = 0;
    for (; i >= 0 && cnt < n; --i) {
        if (fseek(f, i, SEEK_SET)) return -1;
        int c = fgetc(f);
        if (!isspace(c)) { if (!inword) { inword = 1; cnt++; } }
        else inword = 0;
    }
    return (cnt < n) ? 0 : i + 1 + (isspace((int)(fseek(f, i+1, SEEK_SET), fgetc(f))) ? 1 : 0);
}

static int copy_from(FILE *in, FILE *out, long start) {
    if (fseek(in, start, SEEK_SET)) return 1;
    char buf[4096]; size_t r;
    while ((r = fread(buf, 1, sizeof(buf), in)) > 0) if (fwrite(buf, 1, r, out) != r) return 1;
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "uzycie: %s <src> <dst> <znaki|slowa|linie>\n", argv[0]);
        return 1;
    }
    FILE *in = fopen(argv[1], "rb"); if (!in) { perror("src"); return 1; }
    FILE *out = fopen(argv[2], "wb"); if (!out) { perror("dst"); fclose(in); return 1; }

    long start = -1;
    if (strcmp(argv[3], "znaki") == 0)       start = start_last_chars(in, 10);
    else if (strcmp(argv[3], "linie") == 0)  start = start_last_lines(in, 10);
    else if (strcmp(argv[3], "slowa") == 0)  start = start_last_words(in, 10);
    else { fprintf(stderr, "tryb: znaki slowa linie\n"); fclose(in); fclose(out); return 1; }

    if (start < 0 || copy_from(in, out, start)) { fprintf(stderr, "blad kopiowania\n"); fclose(in); fclose(out); return 1; }

    fclose(in); fclose(out);
    return 0;
}