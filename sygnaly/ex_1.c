#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void signal_handler(int sig) {
    switch (sig) {
        case SIGINT:
            printf("Odebrano sygnał SIGINT (Ctrl + C)\n");
            break;
        case SIGTSTP:
            printf("Odebrano sygnał SIGTSTP (Ctrl + Z)\n");
            break;
        case SIGQUIT:
            printf("Odebrano sygnał SIGQUIT (Ctrl + \\)\n");
            break;
        default:
            printf("Odebrano inny sygnał: %d\n", sig);
    }
}

int main() {

    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGQUIT, signal_handler);

    printf("Naciśnij Ctrl+C, Ctrl+Z lub Ctrl+\\, aby sprawdzić sygnał.\n");


    while (1) {
        pause();
    }

    return 0;
}