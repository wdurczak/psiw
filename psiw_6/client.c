#include <stdio.h>      // printf(), fgets()
#include <stdlib.h>     // exit()
#include <string.h>     // strcspn()
#include <sys/ipc.h>    // ftok()
#include <sys/msg.h>    // msgget(), msgrcv(), msgsnd()
#include <sys/types.h>  // typy systemowe

#define MAX_TEXT 256

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
};

int main() {
    key_t key_c2s;   // klucz kolejki client->server
    key_t key_s2c;   // klucz kolejki server->client
    int q_c2s;       // id kolejki client->server
    int q_s2c;       // id kolejki server->client

    struct msg_buffer msg;

    // Te same klucze co w serwerze (muszą pasować!)
    key_c2s = ftok("progfile", 65);
    key_s2c = ftok("progfile", 66);

    // Otwieramy istniejące kolejki
    // UWAGA: tu nie musimy IPC_CREAT, ale można dać też IPC_CREAT,
    // wtedy klient może "pomóc" utworzyć kolejkę, jeśli serwer jeszcze nie zrobił.
    q_c2s = msgget(key_c2s, 0666 | IPC_CREAT);
    q_s2c = msgget(key_s2c, 0666 | IPC_CREAT);

    // Pobieramy tekst od użytkownika
    printf("Klient: wpisz tekst do wysłania: ");
    fgets(msg.msg_text, MAX_TEXT, stdin);

    // Usuwamy znak '\n' z końca po fgets (żeby ładnie wyglądało)
    msg.msg_text[strcspn(msg.msg_text, "\n")] = '\0';

    // Ustawiamy typ komunikatu wysyłanego do serwera
    msg.msg_type = 1;

    // Wysyłamy tekst do serwera (kolejka client->server)
    msgsnd(q_c2s, &msg, sizeof(msg.msg_text), 0);

    printf("Klient: wysłano: %s\n", msg.msg_text);

    // Czekamy na odpowiedź od serwera (kolejka server->client)
    // Serwer wysyła typ=2, więc klient odbiera tylko typ=2
    msgrcv(q_s2c, &msg, sizeof(msg.msg_text), 2, 0);

    printf("Klient: odebrano odpowiedź: %s\n", msg.msg_text);

    return 0;
}