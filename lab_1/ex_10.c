//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char buf[4096];
    for (int i = 1; i < argc - 1; i++) {
        int in = open(argv[i], O_RDONLY);
        if (in < 0) {
            perror("błąd otwarcia pliku źródłowego");
            close(out);
            return 1;
        }
        ssize_t n;
        while ((n = read(in, buf, sizeof(buf))) > 0) {
            if (write(out, buf, n) != n) {
                perror("błąd zapisu");
                close(in);
                close(out);
                return 1;
            }
        }
        close(in);
    }
    close(out);
    return 0;
}