/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTS 2

void *client_handler(void *arg) {
    int client_sock = *((int*)arg);
    char buffer[256];
    int n;

    while ((n = read(client_sock, buffer, 255)) > 0) {
        buffer[n] = '\0';
        printf("Received message: %s\n", buffer);
        write(client_sock, "I got your message", 18);
    }

    close(client_sock);

    pthread_exit(NULL);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pthread_t threads[MAX_CLIENTS];
    int rc, i;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Error creating socket");
        exit(1);
    }

    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        exit(1);
    }

    listen(server_sock, 5);

    while (1) {
        client_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_len);

        if (client_sock < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Limit the number of clients to MAX_CLIENTS
        if (pthread_active_count() - 1 >= MAX_CLIENTS) {
            printf("Max clients reached, rejecting connection\n");
            close(client_sock);
            continue;
        }

        rc = pthread_create(&threads[i], NULL, client_handler, (void*) &client_sock);
        if (rc) {
            perror("Error creating thread");
            close(client_sock);
            continue;
        }

        i++;

        // Wait for threads to exit before reusing their IDs
        if (i >= MAX_CLIENTS) {
            for (int j = 0; j < MAX_CLIENTS; j++) {
                pthread_join(threads[j], NULL);
            }

            i = 0;
        }
    }

    close(server_sock);

    return 0;
}
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define PORT 8080
#define PLAYER_COUNT 2
#define MAX_ATTEMPTS 5

pthread_t tid[PLAYER_COUNT];
bool game_over = false;
pthread_mutex_t mutex; // serve para proteger uma região crítica.

int inicializar_socket();
char* escolher_palavra_aleatoria(char* result);
void receber_mensagem(int new_socket, char* buffer);
void enviar_mensagem(int new_socket, char* buffer);

int main() {
    char word[100];
    char secret_word[100];
    char buffer[1024] = {0};

    escolher_palavra_aleatoria(word);

    memset(secret_word, '_', strlen(word));

    int socket = inicializar_socket();
    pthread_mutex_init(&mutex, NULL);

    while (true) {
        receber_mensagem(socket, buffer);
        enviar_mensagem(socket, buffer);
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        pthread_join(tid[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *handle_guess(void *arg)
{
    char guess;
    while (!game_over)
    {
        printf("\nDigite uma letra: ");
        scanf("%c", &guess);
        getchar();                  // consume newline
        pthread_mutex_lock(&mutex); // obter exclusão mútua
        int i;
        bool found = false;
        for (i = 0; i < strlen(word); i++)
        {
            if (word[i] == guess)
            {
                palavra_escondida[i] = guess;
                found = true;
            }
        }
        if (!found)
        {
            tent_restantes--;
            printf("Letra incorreta! Tentativas restantes: %d\n", tent_restantes);
            if (tent_restantes == 0)
            {
                printf("Você perdeu! A palavra era: %s\n", word);
                game_over = true;
            }
        }
        else
        {
            printf("Suposição correta\n");
            printf("Palavra escondida: %s\n", palavra_escondida);
            if (strcmp(palavra_escondida, word) == 0)
            {
                printf("Você ganhou!\n");
                game_over = true;
            }
        }
        pthread_mutex_unlock(&mutex); // liberar a região crítica
    }
    pthread_exit(NULL);
}

char* escolher_palavra_aleatoria(char* result) {
    FILE *fp;
    char line[100];
    int count = 0, target_line, current_line = 0;

    // abre o arquivo
    fp = fopen("./palavras.txt", "r");

    // conta o número de linhas do arquivo
    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;
    }

    // gera um número aleatório entre 0 e o número de linhas do arquivo
    srand(time(NULL));
    target_line = rand() % count;

    // volta ao início do arquivo
    rewind(fp);

    // fecha o arquivo
    fclose(fp);

    line[strcspn(line, "\n")] = '\0'

    strcpy(result, line);
}

int inicializar_socket() {
    int server_fd, new_socket, mensagem_do_cliente;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    listen(server_fd, 3);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    return new_socket;
}

void receber_mensagem(int new_socket, char* buffer) {
    int mensagem_do_cliente = read(new_socket, buffer, 1024);
    printf("Client: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
}

void enviar_mensagem(int new_socket, char* buffer) {
    printf("Enter message for client: ");
    scanf("%[^\n]%*c", buffer);
    send(new_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, sizeof(buffer));
}