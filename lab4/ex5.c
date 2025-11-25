//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main() {
    int pdesk[2];
    char buf[200];

    // Tworzymy potok
    if (pipe(pdesk) == -1) {
        perror("pipe");
        exit(1);
    }

    // Tworzymy proces potomny
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    // ----------- POTOMEK – wykonuje ls -----------
    if (pid == 0) {

        /*
            Potomek ma wykonać LS i WYSŁAĆ jego wynik do potoku.
        */

        close(pdesk[0]);     // Zamykamy czytanie – potomek tylko PISZE

        // Przeadresowujemy stdout (1) na potok do pisania
        dup2(pdesk[1], 1);

        // Uruchamiamy "ls"
        execlp("ls", "ls", NULL);

        perror("execlp");
        exit(1);
    }

    // ----------- RODZIC – czyta wynik ls i zamienia na DUŻE litery -----------

    close(pdesk[1]);   // Rodzic nie będzie pisał → zamyka zapis

    ssize_t n;
    while ((n = read(pdesk[0], buf, sizeof(buf))) > 0) {

        // Zamiana znaków na DUŻE litery
        for (int i = 0; i < n; i++) {
            buf[i] = toupper((unsigned char)buf[i]);
        }

        // Wypisujemy efekt
        write(1, buf, n);
    }

    close(pdesk[0]);
    return 0;
}