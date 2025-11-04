// loop_ls_ps_sigint_stop_ps.c
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static volatile sig_atomic_t stop_ps = 0;

// Obsługa SIGINT – zatrzymuje wykonywanie ps
static void sigint_handler(int sig) {
    (void)sig;
    stop_ps = 1;
    const char *msg = "\nOtrzymano SIGINT — ps zostanie zatrzymane.\n";
    write(STDOUT_FILENO, msg, 46);
}

// Funkcja uruchamiająca w pętli dane polecenie
static void run_forever(const char *cmd, const volatile sig_atomic_t *stop_flag) {
    for (;;) {
        if (stop_flag && *stop_flag) {
            // zatrzymaj wykonywanie polecenia, ale nie kończ procesu
            sleep(1);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            execlp(cmd, cmd, (char*)NULL);
            perror("execlp");
            _exit(127);
        }
        if (pid < 0) {
            perror("fork");
            sleep(1);
            continue;
        }

        int st;
        while (waitpid(pid, &st, 0) < 0) {
            if (errno == EINTR) continue;
            perror("waitpid");
            break;
        }
    }
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    pid_t ls_pid = fork();
    if (ls_pid == 0) run_forever("ls", NULL);
    if (ls_pid < 0) { perror("fork ls"); exit(1); }

    pid_t ps_pid = fork();
    if (ps_pid == 0) run_forever("ps", &stop_ps);
    if (ps_pid < 0) { perror("fork ps"); kill(ls_pid, SIGTERM); exit(1); }

    printf("Uruchomiono pętle: ls (ciągle) i ps (zatrzymuje się po SIGINT).\n");
    printf("Użyj Ctrl+C, aby zatrzymać tylko ps.\n");

    // proces macierzysty czeka na potomków
    int st;
    while (wait(&st) > 0) {}
    return 0;
}