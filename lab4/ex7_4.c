//
// Created by Wojciech Durczak on 25/11/2025.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // cat /etc/group
        dup2(p[1], 1);
        close(p[0]); close(p[1]);
        execlp("cat", "cat", "/etc/group", NULL);
        perror("cat");
        exit(1);
    }

    if (fork() == 0) {
        // head -5  > grupy.txt
        int fd = open("grupy.txt", O_CREAT|O_WRONLY|O_TRUNC, 0644);
        dup2(p[0], 0);
        dup2(fd, 1);
        close(fd);
        close(p[0]); close(p[1]);
        execlp("head", "head", "-5", NULL);
        perror("head");
        exit(1);
    }

    close(p[0]); close(p[1]);

    wait(NULL);
    wait(NULL);
}