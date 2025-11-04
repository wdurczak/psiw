#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Flaga informująca, że alarm się zakończył
volatile sig_atomic_t alarm_done = 0;

// Funkcja obsługi sygnału SIGALRM
void alarm_handler(int sig) {
    alarm_done = 1;
}

// Własna implementacja funkcji sleep
unsigned int my_sleep(unsigned int seconds) {
    // Rejestracja funkcji obsługi SIGALRM
    signal(SIGALRM, alarm_handler);
    alarm_done = 0;

    // Ustawienie alarmu na określoną liczbę sekund
    alarm(seconds);

    // Oczekiwanie, dopóki alarm się nie zakończy
    while (!alarm_done)
        pause();

    return 0; // po zakończeniu nic nie zwraca (jak oryginalny sleep)
}

int main() {
    printf("Zasypiam na 5 sekund...\n");
    my_sleep(5);
    printf("Minęło 5 sekund!\n");
    return 0;
}