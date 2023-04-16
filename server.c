#include <time.h>       // Biblioteca padrão de funções de tempo
#include <stdio.h>      // Biblioteca padrão de entrada e saída
#include <stdlib.h>     // Biblioteca padrão de funções
#include <string.h>     // Biblioteca padrão de manipulação de strings
#include <sys/socket.h> // Biblioteca padrão de soquetes
#include <arpa/inet.h>  // Biblioteca padrão de endereços de internet
#include <unistd.h>     // Biblioteca padrão de funções de entrada e saída
#include <pthread.h>    // Biblioteca padrão de threads
#include <stdbool.h>    // Biblioteca padrão de variáveis booleanas

#define NULL_CHAR '\0'  // Caractere nulo
#define HIDDEN_CHAR '_' // Caractere para representar letras não descobertas

#define PORT 8080        // Porta do servidor
#define BUFFER_SIZE 2048 // Tamanho do buffer
#define MAX_WORD_SIZE 50 // Tamanho máximo da palavra

#define MAX_CLIENTS 2  // Número máximo de clientes
#define MAX_ATTEMPTS 5 // Número máximo de tentativas

// Client Messages
#define MSG_GUESS "Digite uma letra: "                      // Mensagem para pedir uma letra
#define MSG_WIN "Você ganhou! A palavra era: "              // Mensagem de vitória
#define MSG_LOSE "Você perdeu! A palavra era: "             // Mensagem de derrota
#define COR_GUESS "Suposição correta!"                      // Mensagem de suposição correta
#define WRG_GUESS "Letra incorreta! Tentativas restantes: " // Mensagem de suposição incorreta
#define END_GAME "Fim de jogo!"                             // Mensagem de fim de jogo

char word[MAX_WORD_SIZE]; // Palavra escolhida para o jogo

int new_socket[MAX_CLIENTS]; // Array de descritores de soquete para os clientes
pthread_t tid[MAX_CLIENTS];  // Array de identificadores de threads para os clientes
pthread_mutex_t mutex;       // Mutex para proteger o acesso à variável word

int inicializar_socket();                            // Função para inicializar o socket
void *handle_guess(void *arg);                       // Função para lidar com as suposições dos clientes
void escolher_palavra_aleatoria(char *result);       // Função para escolher uma palavra aleatória do arquivo
void receber_mensagem(int new_socket, char *buffer); // Função para receber uma mensagem do cliente
void enviar_mensagem(int new_socket, char *buffer);  // Função para enviar uma mensagem para o cliente

int main() // Função Principal
{
    int sock = 0;                // Descritor de soquete
    struct sockaddr_in cli_addr; // Estrutura de endereço de internet

    escolher_palavra_aleatoria(word); // Escolhe uma palavra aleatória do arquivo

    printf("PALAVRA escolhida para os jogos: %s\n", word); // Imprime a palavra escolhida
    //printf("Palavra escolhida pelo servidor: %s\n", word);
    //printf("Tamanho da palavra escolhida pelo servidor: %d\n", strlen(word));

    sock = inicializar_socket(); // Inicializa o socket

    pthread_mutex_init(&mutex, NULL); // Inicializa o mutex

    int client_count = 0;              // Contador de clientes
    while (client_count < MAX_CLIENTS) // Enquanto o número de clientes for menor que o máximo
    {
        int clilen = sizeof(cli_addr);                                                               // Tamanho da estrutura de endereço de internet
        new_socket[client_count] = accept(sock, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen); // Aceita a conexão do cliente

        pthread_create(&tid[client_count], NULL, handle_guess, &new_socket[client_count]); // Cria uma thread para lidar com as suposições do cliente

        client_count++; // Incrementa o contador de clientes
    }

    for (int i = 0; i < MAX_CLIENTS; i++) // Para cada cliente
    {
        pthread_join(tid[i], NULL); // Aguarda a thread do cliente terminar
    }

    pthread_mutex_destroy(&mutex); // Destroi o mutex

    return 0; // Retorna 0
}

void *handle_guess(void *arg)
{
    int sock = *(int *)arg; // Descritor de soquete do cliente

    char guess;                             // Letra suposta
    char word_local[MAX_WORD_SIZE];         // Palavra escolhida para o jogo
    int tent_restantes = MAX_ATTEMPTS;      // Número de tentativas restantes
    char palavra_escondida[MAX_WORD_SIZE];  // Palavra escondida
    char buffer[BUFFER_SIZE] = {NULL_CHAR}; // Buffer

    pthread_mutex_lock(&mutex);   // Bloqueia o acesso à variável word
    strcpy(word_local, word);     // Copia a palavra escolhida para o jogo
    pthread_mutex_unlock(&mutex); // Desbloqueia o acesso à variável word

    int word_size = strlen(word_local); // Tamanho da palavra

    bool game_over = false; // Variável para indicar se o jogo acabou
    bool win = false;       // Variável para indicar se o jogador ganhou


    memset(palavra_escondida, HIDDEN_CHAR, word_size); // Preenche a palavra escondida com o caractere de letra não descoberta
    palavra_escondida[word_size -1] = NULL_CHAR;          // Adiciona o caractere nulo no final da palavra

    read(sock, buffer, BUFFER_SIZE);                         // Lê a mensagem do cliente
    memset(buffer, NULL_CHAR, BUFFER_SIZE);                  // Limpa o buffer
    sprintf(buffer, "%s\n%s", palavra_escondida, MSG_GUESS); // Formata a mensagem para enviar para o cliente

    while (!game_over) // Enquanto o jogo não acabou
    {
        enviar_mensagem(sock, buffer); // Envia a mensagem para o cliente

        receber_mensagem(sock, buffer); // Recebe a mensagem do cliente
        guess = buffer[0];              // Pega a primeira letra da mensagem

        memset(buffer, NULL_CHAR, BUFFER_SIZE); // Limpa o buffer

        bool found = false;                 // Variável para indicar se a letra foi encontrada
        for (int i = 0; i < word_size; i++) // Para cada letra da palavra
        {
            bool isGuessCorrect = word_local[i] == guess; // Verifica se a letra suposta é igual a letra da palavra

            if (isGuessCorrect) // Se a letra suposta for igual a letra da palavra
            {
                palavra_escondida[i] = guess; // Mostra a letra na palavra escondida
                found = true;                 // Indica que a letra foi encontrada
            }
        }

        if (!found) // Se a letra não foi encontrada
        {
            tent_restantes--;                                                                          // Decrementa o número de tentativas restantes
            sprintf(buffer, "%s %d\n%s\n%s", WRG_GUESS, tent_restantes, palavra_escondida, MSG_GUESS); // Formata a mensagem para enviar para o cliente
            if (tent_restantes == 0)                                                                   // Se o número de tentativas restantes for igual a 0
            {
                game_over = true; // Indica que o jogo acabou
            }
        }
        else // Se a letra foi encontrada
        {
            sprintf(buffer, "%s\n%s\n%s", COR_GUESS, palavra_escondida, MSG_GUESS); // Formata a mensagem para enviar para o cliente
        }

        bool isWordRevealed = strcmp(palavra_escondida, word_local) == 0; // Verifica se a palavra foi revelada

        if (isWordRevealed) // Se a palavra foi revelada
        {
            game_over = true; // Indica que o jogo acabou
            win = true;       // Indica que o jogador ganhou
        }
    }

    sprintf(buffer, "%s%s\n%s\n", win ? MSG_WIN : MSG_LOSE, word_local, END_GAME); // Formata a mensagem para enviar para o cliente

    enviar_mensagem(sock, buffer); // Envia a mensagem para o cliente

    pthread_exit(NULL); // Encerra a thread
    close(sock);        // Fecha o soquete
}

void escolher_palavra_aleatoria(char *result)
{
    FILE *fp;                    // Ponteiro para o arquivo
    char line[MAX_WORD_SIZE];    // Linha do arquivo
    int count = 0;               // Contador de linhas
    int random_line_number;      // Número da linha aleatória
    int current_line_number = 0; // Número da linha atual

    fp = fopen("./resources/palavras.txt", "r"); // Abre o arquivo

    while (fgets(line, sizeof(line), fp) != NULL) // Enquanto não chegar ao final do arquivo
    {
        count++; // Incrementa o contador de linhas
    }

    srand(time(NULL));                   // Inicializa o gerador de números aleatórios
    random_line_number = rand() % count; // Gera um número aleatório entre 0 e o número de linhas do arquivo

    rewind(fp); // Volta para o início do arquivo

    while (fgets(line, sizeof(line), fp) != NULL) // Enquanto não chegar ao final do arquivo
    {
        if (current_line_number == random_line_number) // Se o número da linha atual for igual ao número da linha aleatória
        {
            line[strcspn(line, "\n")] = '\0'; // Remove o caractere de nova linha
            strcpy(result, line);             //    Copia a linha para o resultado
            break;                            // Sai do laço
        }
        current_line_number++; // Incrementa o número da linha atual
    }

    fclose(fp); // Fecha o arquivo
}

int inicializar_socket()
{
    int server_fd;              // Soquete do servidor
    struct sockaddr_in address; // Endereço do servidor
    int opt = 1;                // Opção para o setsockopt

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // Cria o soquete do servidor

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Define as opções do soquete

    address.sin_family = AF_INET;         // Define o domínio do soquete
    address.sin_addr.s_addr = INADDR_ANY; // Define o endereço do soquete
    address.sin_port = htons(PORT);       // Define a porta do soquete

    bind(server_fd, (struct sockaddr *)&address, sizeof(address)); // Associa o soquete ao endereço

    listen(server_fd, 3); // Define o número máximo de conexões pendentes

    return server_fd; // Retorna o soquete do servidor
}

void receber_mensagem(int new_socket, char *buffer)
{
    read(new_socket, buffer, BUFFER_SIZE);              // Lê a mensagem do cliente
    printf("Client %lu: %s\n", pthread_self(), buffer); // Imprime a mensagem do cliente
}

void enviar_mensagem(int new_socket, char *buffer)
{
    send(new_socket, buffer, strlen(buffer), 0); // Envia a mensagem para o cliente
    memset(buffer, NULL_CHAR, BUFFER_SIZE);      // Limpa o buffer
}
