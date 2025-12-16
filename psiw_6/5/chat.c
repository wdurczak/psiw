#include <stdio.h>      // printf(), fgets()
#include <stdlib.h>     // exit(), atoi()
#include <string.h>     // strcmp(), strcspn()
#include <unistd.h>     // getpid()
#include <sys/ipc.h>    // ftok()
#include <sys/msg.h>    // msgget(), msgsnd(), msgrcv(), msgctl()
#include <sys/types.h>  // typy systemowe
#include <pthread.h>    // pthread_create(), pthread_join()

#define MAX_TEXT 256

// Typ komunikatu (w System V musi być > 0)
#define M_TEXT 1

// Struktura wiadomości dla kolejki System V
struct msg_buffer {
    long msg_type;              // typ komunikatu (tu zawsze M_TEXT)
    char msg_text[MAX_TEXT];    // tekst wiadomości
};

// Globalne (żeby wątki miały dostęp)
int q_send;     // kolejka, na którą wysyłamy
int q_recv;     // kolejka, z której odbieramy

// Flaga zakończenia programu (współdzielona między wątkami)
volatile int running = 1;

// ---------------------------
// WĄTEK: ODBIÓR WIADOMOŚCI
// ---------------------------
// Ten wątek cały czas czeka na wiadomości w kolejce odbiorczej
// i wypisuje je na ekran.
void* receiver_thread(void* arg) {
    struct msg_buffer msg;

    while (running) {
        // Odbieramy dowolny komunikat typu M_TEXT
        // 0 w ostatnim argumencie -> tryb blokujący (czekamy aż coś przyjdzie)
        msgrcv(q_recv, &msg, sizeof(msg.msg_text), M_TEXT, 0);

        // Wypisujemy odebraną wiadomość
        printf("\n[ODEBRANO] %s\n", msg.msg_text);
        printf("Ty: ");
        fflush(stdout);

        // Jeśli treść to "exit", kończymy program (zgodnie z wymaganiem)
        if (strcmp(msg.msg_text, "exit") == 0) {
            running = 0;
            break;
        }
    }

    return NULL;
}

// ---------------------------
// WĄTEK: WYSYŁANIE WIADOMOŚCI
// ---------------------------
// Ten wątek czyta linie z klawiatury i wysyła je do drugiego programu.
void* sender_thread(void* arg) {
    struct msg_buffer msg;
    msg.msg_type = M_TEXT;

    while (running) {
        // Pobieramy linię od użytkownika
        printf("Ty: ");
        fflush(stdout);

        if (fgets(msg.msg_text, MAX_TEXT, stdin) == NULL) {
            // EOF / błąd wejścia -> kończymy
            running = 0;
            break;
        }

        // Usuwamy znak '\n' z końca, bo fgets go zostawia
        msg.msg_text[strcspn(msg.msg_text, "\n")] = '\0';

        // Wysyłamy wiadomość do drugiej strony
        msgsnd(q_send, &msg, sizeof(msg.msg_text), 0);

        // Jeśli użytkownik wpisał "exit", to:
        // - wysyłamy exit do drugiej strony
        // - sami możemy też zakończyć (logicznie)
        if (strcmp(msg.msg_text, "exit") == 0) {
            running = 0;
            break;
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {

    // Program uruchamiamy jako:
    // ./chat 1   albo   ./chat 2
    // gdzie 1 i 2 oznaczają "stronę" rozmowy
    if (argc != 2) {
        printf("Użycie: %s <1|2>\n", argv[0]);
        exit(1);
    }

    int who = atoi(argv[1]);
    if (who != 1 && who != 2) {
        printf("Argument musi być 1 albo 2\n");
        exit(1);
    }

    // Generujemy dwa klucze dla dwóch kolejek
    // UWAGA: plik "progfile" musi istnieć
    key_t key_1to2 = ftok("progfile", 90);
    key_t key_2to1 = ftok("progfile", 91);

    // Tworzymy (lub otwieramy) kolejki:
    // q_1to2 - kolejka do wysyłania z 1 do 2
    // q_2to1 - kolejka do wysyłania z 2 do 1
    int q_1to2 = msgget(key_1to2, 0666 | IPC_CREAT);
    int q_2to1 = msgget(key_2to1, 0666 | IPC_CREAT);

    // Ustalamy: z której kolejki odbieramy, a na którą wysyłamy
    if (who == 1) {
        q_send = q_1to2; // 1 wysyła do 2
        q_recv = q_2to1; // 1 odbiera od 2
        printf("=== CHAT: JESTEŚ STRONĄ 1 ===\n");
    } else {
        q_send = q_2to1; // 2 wysyła do 1
        q_recv = q_1to2; // 2 odbiera od 1
        printf("=== CHAT: JESTEŚ STRONĄ 2 ===\n");
    }

    // Tworzymy dwa wątki: nadawanie i odbieranie
    pthread_t t_send, t_recv;

    pthread_create(&t_recv, NULL, receiver_thread, NULL);
    pthread_create(&t_send, NULL, sender_thread, NULL);

    // Czekamy aż wątki zakończą pracę
    pthread_join(t_send, NULL);
    pthread_join(t_recv, NULL);

    // Sprzątanie kolejek:
    // Problem: jeśli obie strony uruchomione, to obie mogą próbować usuwać kolejki.
    // Żeby nie komplikować, usuwamy kolejki tylko gdy jesteśmy "1".
    // Dzięki temu na końcu rozmowy strona 1 posprząta obie kolejki.
    if (who == 1) {
        msgctl(q_1to2, IPC_RMID, NULL);
        msgctl(q_2to1, IPC_RMID, NULL);
    }

    printf("\nKoniec programu.\n");
    return 0;
}