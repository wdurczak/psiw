//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // POTOMEK 1 → finger
        dup2(p[1], 1);   // stdout -> potok
        close(p[0]);
        close(p[1]);
        execlp("finger", "finger", NULL);
        perror("exec finger");
        exit(1);
    }

    if (fork() == 0) {
        // POTOMEK 2 → cut -d ' ' -f1
        dup2(p[0], 0);   // stdin <- potok
        close(p[1]);
        close(p[0]);
        execlp("cut", "cut", "-d", " ", "-f1", NULL);
        perror("exec cut");
        exit(1);
    }

    close(p[0]);
    close(p[1]);
    wait(NULL);
    wait(NULL);
}