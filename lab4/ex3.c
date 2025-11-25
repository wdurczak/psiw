//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pdesk[2];
    char buf[200];

    // Tworzymy potok (czytanie = pdesk[0], pisanie = pdesk[1])
    if (pipe(pdesk) == -1) {
        perror("pipe");
        exit(1);
    }

    // ----------- POTOMEK 1 (piszący) -----------
    pid_t p1 = fork();
    if (p1 < 0) { perror("fork"); exit(1); }

    if (p1 == 0) {
        /*
            PROCES POTOMNY 1
            Ten proces MA PISAĆ do potoku.
        */

        close(pdesk[0]); // zamykamy czytanie — bo nie czytamy, tylko piszemy

        char *msg = "Wiadomosc od potomka 1\n";
        write(pdesk[1], msg, strlen(msg));
        printf("Potomek 1: wyslalem wiadomosc.\n");

        close(pdesk[1]); // koniec pisania → sygnal dla czytajacego
        exit(0);
    }

    // ----------- POTOMEK 2 (piszący) -----------
    pid_t p2 = fork();
    if (p2 < 0) { perror("fork"); exit(1); }

    if (p2 == 0) {
        /*
            PROCES POTOMNY 2
            Drugi proces pisze do POTOKU to samo albo coś innego
        */

        close(pdesk[0]); // zamykamy czytanie

        char *msg = "Wiadomosc od potomka 2\n";
        write(pdesk[1], msg, strlen(msg));
        printf("Potomek 2: wyslalem wiadomosc.\n");

        close(pdesk[1]);
        exit(0);
    }

    // ----------- POTOMEK 3 (czytający) -----------
    pid_t p3 = fork();
    if (p3 < 0) { perror("fork"); exit(1); }

    if (p3 == 0) {
        /*
            PROCES POTOMNY 3
            Ten ma CZYTAĆ WSZYSTKO co przyszło od dwóch poprzednich procesów.
        */

        close(pdesk[1]); // zamykamy pisanie, bo my czytamy

        // czytamy dane aż pipe się zamknie (czyli dwaj piszący zakończą)
        ssize_t n;
        while ((n = read(pdesk[0], buf, sizeof(buf)-1)) > 0) {
            buf[n] = '\0';
            printf("Potomek 3 odczytal:\n%s", buf);
        }

        close(pdesk[0]);
        exit(0);
    }

    // ----------- RODZIC -----------

    // Rodzic nic nie czyta/pisze, więc zamyka obie strony potoku
    close(pdesk[0]);
    close(pdesk[1]);

    // czeka aż 3 dzieci skończą
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}