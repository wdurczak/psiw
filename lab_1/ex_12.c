//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
int main(int argc, char *argv[]) {
    if (argc != 2) {
        dprintf(2, "użycie: %s <plik>\n", argv[0]);
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("błąd otwarcia pliku");
        return 1;
    }
    unsigned long long licznik[26] = {0};
    unsigned long long suma = 0;
    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char c = buf[i];
            if (isalpha(c)) {
                c = toupper(c);
                licznik[c - 'A']++;
                suma++;
            }
        }
    }
    close(fd);
    if (n < 0) {
        perror("błąd odczytu");
        return 1;
    }
    if (suma == 0) {
        printf("brak liter w pliku.\n");
        return 0;
    }
    printf("częstość występowania liter (%%):\n");
    for (int i = 0; i < 26; ++i) {
        double procent = 100.0 * licznik[i] / suma;
        if (licznik[i] > 0)
            printf("%c: %.2f%%\n", 'A' + i, procent);
    }
    return 0;
}