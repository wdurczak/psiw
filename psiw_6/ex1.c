#include <stdio.h>      // printf()
#include <stdlib.h>     // exit()
#include <string.h>     // strcpy()
#include <sys/ipc.h>    // ftok(), IPC
#include <sys/msg.h>    // msgget(), msgsnd(), msgrcv(), msgctl()
#include <sys/types.h>  // typy systemowe
#include <unistd.h>     // fork()
#include <sys/wait.h>   // wait()

// Maksymalna długość tekstu komunikatu
#define MAX_TEXT 100

// Struktura komunikatu dla kolejek System V
// UWAGA: msg_type MUSI być typu long
struct msg_buffer {
    long msg_type;              // Typ komunikatu (np. 1)
    char msg_text[MAX_TEXT];    // Treść komunikatu
};

int main() {

    key_t key;          // Klucz IPC – identyfikuje kolejkę
    int msgid;          // Identyfikator kolejki komunikatów
    struct msg_buffer message; // Zmienna przechowująca komunikat

    // ftok() generuje klucz IPC na podstawie pliku i liczby
    // Plik MUSI istnieć
    key = ftok("progfile", 65);

    // msgget() tworzy kolejkę lub otwiera istniejącą
    // 0666 – prawa dostępu (rw dla wszystkich)
    // IPC_CREAT – utwórz kolejkę, jeśli nie istnieje
    msgid = msgget(key, 0666 | IPC_CREAT);

    // fork() tworzy nowy proces
    // pid > 0  → proces rodzica
    // pid == 0 → proces potomny
    pid_t pid = fork();

    // ====== PROCES RODZICA ======
    if (pid > 0) {

        // Ustawiamy typ komunikatu (dowolna liczba > 0)
        message.msg_type = 1;

        // Kopiujemy tekst "Hello" do komunikatu
        strcpy(message.msg_text, "Hello");

        // msgsnd() – wysyłanie komunikatu do kolejki
        // msgid – identyfikator kolejki
        // &message – adres komunikatu
        // sizeof(message.msg_text) – rozmiar danych (BEZ msg_type)
        // 0 – tryb blokujący
        msgsnd(msgid, &message, sizeof(message.msg_text), 0);

        // Informacja na ekran
        printf("Rodzic: wysłano komunikat: %s\n", message.msg_text);

        // Czekamy aż proces potomny zakończy działanie
        wait(NULL);

        // Usuwamy kolejkę komunikatów z systemu
        msgctl(msgid, IPC_RMID, NULL);
    }

    // ====== PROCES POTOMNY ======
    else if (pid == 0) {

        // msgrcv() – odbiór komunikatu z kolejki
        // msgid – identyfikator kolejki
        // &message – gdzie zapisać komunikat
        // sizeof(message.msg_text) – rozmiar danych
        // 1 – typ komunikatu, który chcemy odebrać
        // 0 – tryb blokujący (czekaj aż komunikat się pojawi)
        msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0);

        // Wypisujemy odebrany komunikat
        printf("Potomek: odebrano komunikat: %s\n", message.msg_text);
    }

    // Zakończenie programu
    return 0;
}