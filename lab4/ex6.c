//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pdesk[2];
    char buf[256];

    // Tworzymy potok: pdesk[0] = czytanie, pdesk[1] = pisanie
    if (pipe(pdesk) == -1) {
        perror("pipe");
        exit(1);
    }

    // ============================
    // 1. Pierwszy potomek — robi "ls"
    // ============================
    if (fork() == 0) {

        // Ten proces PISZE do potoku → zamyka stronę do czytania
        close(pdesk[0]);

        // Przekierowujemy stdout → na potok
        dup2(pdesk[1], STDOUT_FILENO);

        close(pdesk[1]); // już niepotrzebny po dup2

        // Uruchamiamy ls
        execlp("ls", "ls", NULL);

        perror("execlp ls");
        exit(1);
    }

    // ============================
    // 2. Drugi potomek — czyta ls, zamienia litery i wypisuje
    // ============================
    if (fork() == 0) {

        // Ten proces CZYTA → więc zamyka stronę do pisania
        close(pdesk[1]);

        // Czytamy dane z potoku
        ssize_t n;
        while ((n = read(pdesk[0], buf, sizeof(buf))) > 0) {

            // KONWERSJA małe → DUŻE (to co robi "tr a-z A-Z")
            for (ssize_t i = 0; i < n; i++) {
                buf[i] = toupper((unsigned char)buf[i]);
            }

            // Wypisujemy na standardowe wyjście
            write(STDOUT_FILENO, buf, n);
        }

        close(pdesk[0]);
        exit(0);
    }

    // ============================
    // 3. Rodzic nic nie robi → zamyka potok i czeka
    // ============================

    close(pdesk[0]);
    close(pdesk[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}