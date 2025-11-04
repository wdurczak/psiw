// loop_ls_ps.c
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static pid_t ls_supervisor = -1, ps_supervisor = -1;

static void run_forever(const char *cmd) {
    for (;;) {
        pid_t pid = fork();
        if (pid == 0) {                        // dziecko-uruchamiacz komendy
            execlp(cmd, cmd, (char*)NULL);     // uruchom cmd
            perror("execlp");                  // jeśli execlp się nie powiedzie
            _exit(127);
        }
        if (pid < 0) { perror("fork"); sleep(1); continue; }

        int st;
        while (waitpid(pid, &st, 0) < 0) {     // czekaj aż komenda się skończy
            if (errno == EINTR) continue;
            perror("waitpid");
            break;
        }
        // natychmiast powtórz (możesz dodać małe opóźnienie, np. sleep(1))
    }
}

// łagodne zakończenie: po Ctrl+C zakończ obie pętle-nadzorcy
static void sigint_handler(int sig) {
    (void)sig;
    if (ls_supervisor > 0) kill(ls_supervisor, SIGTERM);
    if (ps_supervisor > 0) kill(ps_supervisor, SIGTERM);
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);


    ls_supervisor = fork();
    if (ls_supervisor == 0) run_forever("ls");
    if (ls_supervisor < 0) { perror("fork ls"); return 1; }


    ps_supervisor = fork();
    if (ps_supervisor == 0) run_forever("ps");
    if (ps_supervisor < 0) { perror("fork ps"); kill(ls_supervisor, SIGTERM); return 1; }

    printf("Uruchomiono pętle: ls i ps. Zatrzymaj Ctrl+C.\n");


    int st;
    while (wait(&st) > 0) {}
    return 0;
}
