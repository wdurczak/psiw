//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void run(char *cmd, char *arg1, char *arg2, int in, int out) {
    if (fork() == 0) {
        if (in  != 0) dup2(in, 0);
        if (out != 1) dup2(out, 1);
        execlp(cmd, cmd, arg1, arg2, NULL);
        perror(cmd);
        exit(1);
    }
}

int main() {
    int p1[2], p2[2], p3[2], p4[2], p5[2];
    pipe(p1); pipe(p2); pipe(p3); pipe(p4); pipe(p5);

    // ps -ef
    run("ps", "-ef", NULL, 0, p1[1]);

    // tr -s ' ' ':'
    run("tr", "-s", " : ", p1[0], p2[1]);

    // cut -d: -f1
    run("cut", "-d:", "-f1", p2[0], p3[1]);

    // sort
    run("sort", NULL, NULL, p3[0], p4[1]);

    // uniq -c
    run("uniq", "-c", NULL, p4[0], p5[1]);

    // sort -n
    dup2(p5[0], 0);
    execlp("sort", "sort", "-n", NULL);
    perror("sort");
}