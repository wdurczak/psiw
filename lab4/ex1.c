#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int pdesk[2];
    pid_t pid;
    char buf[100];
    const char *msg = "Hallo";

    if (pipe(pdesk) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /* proces potomny – CZYTA */
        close(pdesk[1]);                 // nie będzie pisał
        ssize_t n = read(pdesk[0], buf, sizeof(buf));
        if (n < 0) {
            perror("read");
            exit(1);
        }
        buf[n] = '\0';
        printf("Potomny odczytał: %s\n", buf);
        close(pdesk[0]);
    } else {

        close(pdesk[0]);                 // nie będzie czytał
        ssize_t n = write(pdesk[1], msg, strlen(msg));
        if (n < 0) {
            perror("write");
            exit(1);
        }
        close(pdesk[1]);
    }

    return 0;
}