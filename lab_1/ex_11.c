//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
static int is_word_char(unsigned char c) {
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           (c == '_');
}
int main(int argc, char *argv[]) {
    int fd = STDIN_FILENO;
    if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) { perror("open"); return 1; }
    } else if (argc > 2) {
        dprintf(2, "użycie: %s file\n", argv[0]);
        return 1;
    }
    char buf[4096];
    ssize_t n;
    int in_word = 0;
    unsigned long long count = 0;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char c = (unsigned char)buf[i];
            if (is_word_char(c)) {
                if (!in_word) { in_word = 1; count++; }
            } else {
                in_word = 0;
            }
        }
    }
    if (fd != STDIN_FILENO) close(fd);
    if (n < 0) { perror("read"); return 1; }
    printf("%llu\n", count);
    return 0;
}