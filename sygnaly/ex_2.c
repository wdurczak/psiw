#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {

    signal(SIGINT, SIG_IGN);

    printf("Program ignoruje sygnał SIGINT (Ctrl+C).\n");
    printf("Naciśnij Ctrl+C — nic się nie stanie.\n");


    while (1) {
        printf("Działa...\n");
        sleep(2);
    }

    return 0;
}