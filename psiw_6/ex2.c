#include <stdio.h>      // printf()
#include <stdlib.h>     // exit(), atoi()
#include <string.h>     // strcpy()
#include <sys/ipc.h>    // ftok(), IPC
#include <sys/msg.h>    // msgget(), msgsnd(), msgrcv(), msgctl()
#include <sys/types.h>  // typy systemowe
#include <unistd.h>     // fork()
#include <sys/wait.h>   // wait()

// Maksymalna długość tekstu komunikatu
#define MAX_TEXT 100

// Struktura komunikatu dla kolejki System V
// msg_type MUSI być typu long
struct msg_buffer {
    long msg_type;              // Typ komunikatu (1–10)
    char msg_text[MAX_TEXT];    // Treść komunikatu
};

int main(int argc, char *argv[]) {

    // ====== SPRAWDZENIE ARGUMENTU ======
    // Program wymaga jednego argumentu: typu komunikatu do odbioru
    if (argc != 2) {
        printf("Użycie: %s <typ_komunikatu 1-10>\n", argv[0]);
        exit(1);
    }

    // Konwersja argumentu tekstowego na liczbę
    int wanted_type = atoi(argv[1]);

    // Sprawdzenie poprawności zakresu
    if (wanted_type < 1 || wanted_type > 10) {
        printf("Typ komunikatu musi być z zakresu 1–10\n");
        exit(1);
    }

    key_t key;                   // Klucz IPC
    int msgid;                   // Identyfikator kolejki
    struct msg_buffer message;   // Zmienna na komunikat

    // Generowanie klucza IPC
    // Plik "progfile" musi istnieć
    key = ftok("progfile", 65);

    // Tworzenie / otwieranie kolejki komunikatów
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Tworzymy drugi proces
    pid_t pid = fork();

    // ====== PROCES RODZICA (WYSYŁAJĄCY) ======
    if (pid > 0) {

        // Wysyłamy kilka komunikatów o różnych typach
        for (int i = 1; i <= 5; i++) {

            // Ustawiamy typ komunikatu (1–5)
            message.msg_type = i;

            // Tworzymy treść komunikatu
            sprintf(message.msg_text, "Komunikat typu %d", i);

            // Wysyłamy komunikat do kolejki
            msgsnd(msgid, &message, sizeof(message.msg_text), 0);

            // Informacja diagnostyczna
            printf("Rodzic: wysłano %s\n", message.msg_text);
        }

        // Czekamy na zakończenie procesu potomnego
        wait(NULL);

        // Usuwamy kolejkę komunikatów z systemu
        msgctl(msgid, IPC_RMID, NULL);
    }

    // ====== PROCES POTOMNY (ODBIERAJĄCY) ======
    else if (pid == 0) {

        // Odbieramy TYLKO komunikat o typie podanym jako argument
        // msgrcv() z msg_type = wanted_type filtruje komunikaty
        msgrcv(msgid, &message, sizeof(message.msg_text), wanted_type, 0);

        // Wyświetlamy odebrany komunikat
        printf("Potomek: odebrano komunikat: %s (typ %ld)\n",
               message.msg_text, message.msg_type);
    }

    return 0;
}