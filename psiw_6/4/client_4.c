#include <stdio.h>      // printf()
#include <stdlib.h>     // exit()
#include <sys/ipc.h>    // ftok()
#include <sys/msg.h>    // msgget(), msgrcv(), msgsnd(), msgctl()
#include <sys/types.h>  // typy systemowe

// Definiujemy typy komunikatów (muszą być > 0, bo msg_type w System V musi być dodatni)
#define M_DANE  1   // typ: zwykła liczba do sumowania
#define M_END   2   // typ: ostatnia liczba (po niej kończymy)
#define M_WYNIK 3   // typ: wynik sumowania odsyłany klientowi

// Komunikat będzie przenosił liczbę (np. int)
struct msg_number {
    long msg_type;  // typ komunikatu (M_DANE / M_END / M_WYNIK)
    int value;      // przesyłana liczba
};

int main() {
    key_t key_c2s;   // klucz kolejki client->server
    key_t key_s2c;   // klucz kolejki server->client
    int q_c2s;       // id kolejki client->server
    int q_s2c;       // id kolejki server->client

    struct msg_number msg;  // bufor na odbierane / wysyłane komunikaty

    // Dwa klucze -> dwie kolejki (jak w poprzednim zadaniu)
    // Plik "progfile" musi istnieć
    key_c2s = ftok("progfile", 75);
    key_s2c = ftok("progfile", 76);

    // Tworzymy / otwieramy kolejki
    q_c2s = msgget(key_c2s, 0666 | IPC_CREAT);
    q_s2c = msgget(key_s2c, 0666 | IPC_CREAT);

    printf("Serwer: uruchomiony. Czekam na liczby...\n");

    long sum = 0;  // suma wszystkich liczb (long żeby się mniej przepełniało niż int)

    while (1) {
        // Odbieramy "jakikolwiek" typ z kolejki client->server:
        // 0 jako msgtyp w msgrcv oznacza: weź pierwszy dostępny komunikat (dowolnego typu)
        msgrcv(q_c2s, &msg, sizeof(msg.value), 0, 0);

        // Jeśli to jest M_DANE -> dodajemy do sumy i czekamy dalej
        if (msg.msg_type == M_DANE) {
            printf("Serwer: otrzymano M_DANE = %d\n", msg.value);
            sum += msg.value;
        }
        // Jeśli to jest M_END -> dodajemy ostatnią liczbę i kończymy odbiór
        else if (msg.msg_type == M_END) {
            printf("Serwer: otrzymano M_END = %d (koniec danych)\n", msg.value);
            sum += msg.value;
            break; // wychodzimy z pętli, bo dane się skończyły
        }
    }

    // Teraz odsyłamy wynik do klienta jako komunikat typu M_WYNIK
    msg.msg_type = M_WYNIK;
    msg.value = (int)sum; // dla prostoty wysyłamy int (w razie dużych liczb można zmienić na long)

    msgsnd(q_s2c, &msg, sizeof(msg.value), 0);

    printf("Serwer: odesłano M_WYNIK = %ld\n", sum);

    // Sprzątamy kolejki (w labach zwykle wymagane)
    msgctl(q_c2s, IPC_RMID, NULL);
    msgctl(q_s2c, IPC_RMID, NULL);

    return 0;
}