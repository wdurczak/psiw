//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pdesk[2];

    // Tworzymy potok: pdesk[0] = czytanie, pdesk[1] = pisanie
    if (pipe(pdesk) == -1) {
        perror("pipe");
        exit(1);
    }

    // ------------ PIERWSZY PROCES: ls ------------
    pid_t p1 = fork();
    if (p1 < 0) { perror("fork"); exit(1); }

    if (p1 == 0) {
        /*
            Tutaj jesteśmy w procesie 1 → LS
            Ten proces ma PISAĆ do potoku.
        */

        close(pdesk[0]);    // nie czytamy, więc zamykamy czytanie

        // Przeadresowanie standardowego wyjścia (stdout = 1)
        // tak aby pisanie trafiało DO POTOKU
        dup2(pdesk[1], STDOUT_FILENO);

        close(pdesk[1]);    // deskryptor już niepotrzebny po dup2

        // Uruchamiamy ls (zastępuje proces)
        execlp("ls", "ls", NULL);

        perror("execlp ls");
        exit(1);
    }

    // ------------ DRUGI PROCES: wc ------------
    pid_t p2 = fork();
    if (p2 < 0) { perror("fork"); exit(1); }

    if (p2 == 0) {
        /*
            Tutaj jesteśmy w procesie 2 → WC
            Ten proces ma CZYTAĆ z potoku.
        */

        close(pdesk[1]);    // nie piszemy, więc zamykamy pisanie

        // Przeadresowanie standardowego wejścia (stdin = 0)
        // tak aby czytanie brało dane z potoku
        dup2(pdesk[0], STDIN_FILENO);

        close(pdesk[0]);

        // Uruchamiamy wc
        execlp("wc", "wc", NULL);

        perror("execlp wc");
        exit(1);
    }

    // ------------ RODZIC ------------

    // Rodzic nie używa potoku, więc zamyka obie strony
    close(pdesk[0]);
    close(pdesk[1]);

    // Czekamy na oba procesy
    wait(NULL);
    wait(NULL);

    return 0;
}