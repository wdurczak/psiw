//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    // 1: ls -l
    if (fork() == 0) {
        dup2(p1[1], 1);
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        execlp("ls", "ls", "-l", NULL);
        perror("ls");
        exit(1);
    }

    // 2: grep ^d
    if (fork() == 0) {
        dup2(p1[0], 0);
        dup2(p2[1], 1);
        close(p1[1]); close(p1[0]);
        close(p2[0]); close(p2[1]);
        execlp("grep", "grep", "^d", NULL);
        perror("grep");
        exit(1);
    }

    // 3: more
    if (fork() == 0) {
        dup2(p2[0], 0);
        close(p1[0]); close(p1[1]);
        close(p2[1]); close(p2[0]);
        execlp("more", "more", NULL);
        perror("more");
        exit(1);
    }

    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
}