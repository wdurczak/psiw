//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int pdesk[2];
    char buf[100];
    const char *msg = "Hallo od potomka 1";

    // Tworzymy potok: pdesk[0] = czytanie, pdesk[1] = pisanie
    if (pipe(pdesk) == -1) {
        perror("pipe");
        exit(1);
    }

    // Pierwsze fork() – tworzymy pierwszego potomka (PISZĄCEGO)
    pid_t p1 = fork();

    if (p1 < 0) {
        perror("fork 1");
        exit(1);
    }

    if (p1 == 0) {
        /*
            ---- POTOMEK 1 ----
            Ten proces ma wysłać wiadomość przez potok.
        */

        close(pdesk[0]); // Zamykamy czytanie – bo my tylko PISZEMY

        // Wysyłamy wiadomość
        write(pdesk[1], msg, strlen(msg));

        printf("Potomek 1: wysłałem wiadomość do potoku!\n");

        close(pdesk[1]); // Koniec pisania
        exit(0);
    }

    // Jeśli jesteśmy tutaj → to jesteśmy w RODZICU.
    // Robimy drugiego fork(), żeby utworzyć DRUGIEGO potomka.

    pid_t p2 = fork();

    if (p2 < 0) {
        perror("fork 2");
        exit(1);
    }

    if (p2 == 0) {
        /*
            ---- POTOMEK 2 ----
            Ten proces ma odebrać wiadomość z potoku.
        */

        close(pdesk[1]); // Zamykamy pisanie – bo my tylko CZYTAMY

        // Czytamy dane z potoku do bufora
        ssize_t n = read(pdesk[0], buf, sizeof(buf)-1);

        if (n >= 0) {
            buf[n] = '\0'; // Zrobienie z tego „napisu”
            printf("Potomek 2: odczytałem: %s\n", buf);
        } else {
            perror("read");
        }

        close(pdesk[0]); // Koniec czytania
        exit(0);
    }

    /*
        ---- RODZIC ----
        Nic nie robi z potokiem, tylko zamyka.
        Potomkowie już odziedziczyli deskryptory.
    */

    close(pdesk[0]);
    close(pdesk[1]);

    // Czekamy na obydwa dzieci
    wait(NULL);
    wait(NULL);

    return 0;
}