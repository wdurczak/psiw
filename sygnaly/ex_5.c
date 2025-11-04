// sigcld_demo.c
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>


static volatile sig_atomic_t reaped = 0;

// Handler: przechwytujemy SIGCHLD i reaperujemy dzieci bez blokowania
static void chld_handler(int sig) {
    (void)sig;
    int status;
    // Zbierz wszystkie zakończone dzieci (gdyby przyszło kilka SIGCHLD)
    while (waitpid(-1, &status, WNOHANG) > 0) {
        reaped++;
    }
}

static pid_t spawn_child(int id, unsigned sec) {
    pid_t pid = fork();
    if (pid == 0) {
        // Dziecko: chwilę "pracuje" i wychodzi
        fprintf(stdout, "[child %d] pid=%d: śpię %u s i kończę.\n",
                id, getpid(), sec);
        fflush(stdout);
        sleep(sec);
        _exit(10 + id); // różne kody wyjścia
    }
    return pid;
}

int main(int argc, char **argv) {
    if (argc != 2 || (strcmp(argv[1], "catch") != 0 && strcmp(argv[1], "ignore") != 0)) {
        fprintf(stderr, "Użycie: %s <catch|ignore>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "catch") == 0) {
        // PRZECHWYTYWANIE SIGCHLD
        struct sigaction sa = {0};
        sa.sa_handler = chld_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // nie wysyłaj na stop/cont
        sigaction(SIGCHLD, &sa, NULL);

        printf("[parent] tryb: CATCH – instaluję handler SIGCHLD i sam reaperuję dzieci.\n");
    } else {
        // IGNOROWANIE SIGCHLD
        struct sigaction sa = {0};
        sa.sa_handler = SIG_IGN;     // ignoruj sygnał
        sa.sa_flags   = SA_NOCLDWAIT; // upewnij się, że jądro automatycznie sprzątnie
        sigaction(SIGCHLD, &sa, NULL);

        printf("[parent] tryb: IGNORE – ignoruję SIGCHLD; dzieci nie tworzą zombie.\n");
    }

    // Utwórz kilka procesów potomnych
    for (int i = 0; i < 3; ++i) {
        pid_t pid = spawn_child(i+1, 1 + (i % 2)); // czasy: 1,2,1 s
        if (pid < 0) { perror("fork"); exit(1); }
        printf("[parent] utworzono child pid=%d\n", pid);
    }

    // Odczekaj chwilę aż dzieci się zakończą
    sleep(3);

    // Sprawdź, co widzi rodzic
    int status;
    pid_t r = waitpid(-1, &status, WNOHANG);
    if (r == 0) {
        printf("[parent] waitpid: brak dzieci do zebrania (0)\n");
    } else if (r > 0) {
        printf("[parent] waitpid: NIEOCZEKIWANIE zebrałem pid=%d (powinno być zrobione w handlerze)\n", r);
    } else { // r == -1
        if (errno == ECHILD) {
            printf("[parent] waitpid: ECHILD – brak dzieci (typowe w trybie IGNORE, bo jądro posprzątało).\n");
        } else {
            perror("waitpid");
        }
    }

    printf("[parent] zebrane przez handler (catch): %d\n", reaped);
    printf("[parent] koniec.\n");
    return 0;
}