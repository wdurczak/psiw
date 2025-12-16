//
// Created by Wojciech Durczak on 16/12/2025.
//
#include <stdio.h>      // printf()
#include <stdlib.h>     // exit()
#include <string.h>     // strlen()
#include <ctype.h>      // toupper()
#include <sys/ipc.h>    // ftok()
#include <sys/msg.h>    // msgget(), msgrcv(), msgsnd(), msgctl()
#include <sys/types.h>  // typy systemowe

#define MAX_TEXT 256

// Struktura komunikatu System V
// UWAGA: msg_type MUSI być typu long
struct msg_buffer {
    long msg_type;              // typ komunikatu (np. 1)
    char msg_text[MAX_TEXT];    // dane
};

int main() {
    key_t key_c2s;   // klucz kolejki client->server
    key_t key_s2c;   // klucz kolejki server->client
    int q_c2s;       // id kolejki client->server
    int q_s2c;       // id kolejki server->client

    struct msg_buffer msg;      // bufor na odebrane dane

    // Tworzymy dwa różne klucze na podstawie dwóch różnych "projekt id"
    // Plik "progfile" musi istnieć (touch progfile)
    key_c2s = ftok("progfile", 65);
    key_s2c = ftok("progfile", 66);

    // Tworzymy / otwieramy kolejki
    // 0666 -> prawa rw dla wszystkich (w labach ok)
    // IPC_CREAT -> utwórz jeśli nie ma
    q_c2s = msgget(key_c2s, 0666 | IPC_CREAT);
    q_s2c = msgget(key_s2c, 0666 | IPC_CREAT);

    printf("Serwer: uruchomiony, czekam na wiadomość...\n");

    // Odbieramy wiadomość od klienta
    // msg_type = 1 (umawiamy się, że klient wysyła typ=1)
    // 0 -> blokuj aż coś przyjdzie
    msgrcv(q_c2s, &msg, sizeof(msg.msg_text), 1, 0);

    printf("Serwer: odebrano: %s\n", msg.msg_text);

    // Zamiana małych liter na duże w miejscu (in-place)
    // toupper działa poprawnie dla znaków ASCII (a-z -> A-Z)
    for (int i = 0; msg.msg_text[i] != '\0'; i++) {
        msg.msg_text[i] = (char)toupper((unsigned char)msg.msg_text[i]);
    }

    // Ustawiamy typ odpowiedzi (np. 2)
    // To pozwala klientowi odebrać konkretny typ komunikatu
    msg.msg_type = 2;

    // Odsyłamy przetworzony tekst do klienta
    msgsnd(q_s2c, &msg, sizeof(msg.msg_text), 0);

    printf("Serwer: odesłano (UPPERCASE): %s\n", msg.msg_text);

    // W labach często wypada posprzątać kolejki.
    // UWAGA: w realnym systemie serwer zwykle działa w pętli
    // i usuwa kolejki dopiero na końcu działania.
    msgctl(q_c2s, IPC_RMID, NULL);
    msgctl(q_s2c, IPC_RMID, NULL);

    return 0;
}