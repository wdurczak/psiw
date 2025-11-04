#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void alarm_handler(int sig) {
    printf("\nCzas minął! Wyświetlam komunikat po zadanym czasie.\n");
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Użycie: %s <liczba_sekund>\n", argv[0]);
        return 1;
    }

    int sekundy = atoi(argv[1]);


    signal(SIGALRM, alarm_handler);

    alarm(sekundy);

    printf("Odliczanie rozpoczęte (%d sekund)...\n", sekundy);
    printf("W międzyczasie możesz wykonywać inne operacje.\n");


    while (1) {
        printf("Pracuję...\n");
        sleep(1);
    }

    return 0;
}