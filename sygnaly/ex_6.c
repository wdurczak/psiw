//
// Created by Wojciech Durczak on 04/11/2025.
//
// parent_wait_first_then_kill_rest.c
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>

static void term_handler(int sig) {
    (void)sig;
    // Minimalny, bezpieczny w sygnale komunikat:
    const char *m = "child: dostałem SIGTERM, kończę.\n";
    write(STDOUT_FILENO, m, 34);
    _exit(143); // 128+15 (konwencja zakończenia przez SIGTERM)
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <liczba_potomkow>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Podaj dodatnią liczbę potomków.\n");
        return 1;
    }

    pid_t *pids = calloc((size_t)n, sizeof(pid_t));
    if (!pids) { perror("calloc"); return 1; }

    // Utwórz N potomków
    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            // Spróbuj posprzątać już utworzone
            for (int j = 0; j < i; ++j) if (pids[j] > 0) kill(pids[j], SIGTERM);
            free(pids);
            return 1;
        } else if (pid == 0) {
            // --- Proces potomny ---
            struct sigaction sa = {0};
            sa.sa_handler = term_handler;
            sigemptyset(&sa.sa_mask);
            sigaction(SIGTERM, &sa, NULL);

            // Losowy sen 1..5 s
            srand((unsigned)(time(NULL) ^ getpid()));
            unsigned s = (rand() % 5) + 1;

            printf("child pid=%d: śpię %u s\n", getpid(), s);
            fflush(stdout);

            sleep(s); // jeżeli w tym czasie przyjdzie SIGTERM, handler zakończy proces
            printf("child pid=%d: skończyłem naturalnie\n", getpid());
            fflush(stdout);
            _exit(0);
        } else {
            // --- Proces macierzysty ---
            pids[i] = pid;
            printf("parent: utworzyłem child pid=%d\n", pid);
        }
    }

    // Rodzic czeka na pierwsze zakończenie
    int status;
    pid_t first = wait(&status);
    if (first < 0) { perror("wait"); free(pids); return 1; }

    printf("parent: pierwszy zakończony pid=%d", first);
    if (WIFEXITED(status))
        printf(", exit=%d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf(", signal=%d\n", WTERMSIG(status));
    else
        printf("\n");

    // Po pierwszym zakończeniu – zakończ pozostałe SIGTERM-em
    for (int i = 0; i < n; ++i) {
        pid_t pid = pids[i];
        if (pid > 0 && pid != first) {
            if (kill(pid, SIGTERM) == 0) {
                printf("parent: wysłałem SIGTERM do pid=%d\n", pid);
            } else if (errno == ESRCH) {
                printf("parent: pid=%d już nie istnieje\n", pid);
            } else {
                perror("kill");
            }
        }
    }

    // Zbierz wszystkie pozostałe dzieci (niezależnie, jak się zakończyły)
    while (1) {
        pid_t r = waitpid(-1, &status, 0);
        if (r > 0) {
            printf("parent: zebrano pid=%d", r);
            if (WIFEXITED(status))
                printf(", exit=%d\n", WEXITSTATUS(status));
            else if (WIFSIGNALED(status))
                printf(", signal=%d\n", WTERMSIG(status));
            else
                printf("\n");
        } else {
            if (errno == ECHILD) break; // brak dzieci
            if (errno == EINTR) continue;
            perror("waitpid");
            break;
        }
    }

    free(pids);
    printf("parent: koniec.\n");
    return 0;
}