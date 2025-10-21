//
// Created by Wojciech Durczak on 21/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    struct stat info;
    if (stat(argv[1], &info) == -1) {
        perror("błąd odczytu informacji o pliku");
        return 1;
    }


    printf("uprawnienia: ");
    printf( (S_ISDIR(info.st_mode)) ? "d" : "-");
    printf( (info.st_mode & S_IRUSR) ? "r" : "-");
    printf( (info.st_mode & S_IWUSR) ? "w" : "-");
    printf( (info.st_mode & S_IXUSR) ? "x" : "-");
    printf( (info.st_mode & S_IRGRP) ? "r" : "-");
    printf( (info.st_mode & S_IWGRP) ? "w" : "-");
    printf( (info.st_mode & S_IXGRP) ? "x" : "-");
    printf( (info.st_mode & S_IROTH) ? "r" : "-");
    printf( (info.st_mode & S_IWOTH) ? "w" : "-");
    printf( (info.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");


    printf("liczba dowiązań: %lu\n", info.st_nlink);


    struct passwd *pw = getpwuid(info.st_uid);
    struct group  *gr = getgrgid(info.st_gid);
    printf("Właściciel: %s\n", pw ? pw->pw_name : "nieznany");
    printf("Grupa: %s\n", gr ? gr->gr_name : "nieznana");


    printf("Rozmiar: %lld bajtów\n", (long long)info.st_size);


    char *czas = ctime(&info.st_mtime);
    if (czas) {
        czas[strcspn(czas, "\n")] = 0;
        printf("Ostatnia modyfikacja: %s\n", czas);
    }


    printf("nazwa pliku: %s\n", argv[1]);

    return 0;
}