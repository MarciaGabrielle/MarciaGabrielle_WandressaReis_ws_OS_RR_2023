#include <time.h>       
#include <stdio.h>      
#include <stdlib.h>    
#include <string.h>     
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <pthread.h>    
#include <stdbool.h>    

#define NULL_CHAR '\0'  
#define HIDDEN_CHAR '_' 

#define PORT 8080        
#define BUFFER_SIZE 2048 
#define MAX_WORD_SIZE 50 

#define MAX_CLIENTS 2  
#define MAX_ATTEMPTS 5 

// Client Messages
#define MSG_GUESS "Digite uma letra: "                      
#define MSG_WIN "Você ganhou! A palavra era: "              
#define MSG_LOSE "Você perdeu! A palavra era: "             
#define COR_GUESS "Suposição correta!"                      
#define WRG_GUESS "Letra incorreta! Tentativas restantes: " 
#define END_GAME "Fim de jogo!"                            

char word[MAX_WORD_SIZE]; // Palavra escolhida para o jogo

int new_socket[MAX_CLIENTS]; 
pthread_t tid[MAX_CLIENTS];  
pthread_mutex_t mutex;       

int inicializar_socket();                           
void *handle_guess(void *arg);                       
void escolher_palavra_aleatoria(char *result);       
void receber_mensagem(int new_socket, char *buffer); 
void enviar_mensagem(int new_socket, char *buffer);  

int main() // Função Principal
{
    int sock = 0;                // Descritor de soquete
    struct sockaddr_in cli_addr; // Estrutura de endereço de internet

    escolher_palavra_aleatoria(word); // Escolhe uma palavra aleatória do arquivo

    printf("Palavra escolhida para os jogos: %s\n", word); // Imprime a palavra escolhida

    sock = inicializar_socket(); // Inicializa o socket

    pthread_mutex_init(&mutex, NULL); // Inicializa o mutex

    int client_count = 0;              
    while (client_count < MAX_CLIENTS) 
    {
        int clilen = sizeof(cli_addr);                                                               
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

    char guess;                             
    char word_local[MAX_WORD_SIZE];      
    int tent_restantes = MAX_ATTEMPTS;      
    char palavra_escondida[MAX_WORD_SIZE];  
    char buffer[BUFFER_SIZE] = {NULL_CHAR}; 

    pthread_mutex_lock(&mutex);   // Bloqueia o acesso à variável word
    strcpy(word_local, word);     // Copia a palavra escolhida para o jogo
    pthread_mutex_unlock(&mutex); // Desbloqueia o acesso à variável word

    int word_size = strlen(word_local); // Tamanho da palavra

    bool game_over = false; 
    bool win = false;       

    memset(palavra_escondida, HIDDEN_CHAR, word_size); 
    palavra_escondida[word_size] = NULL_CHAR;          // Adiciona o caractere nulo no final da palavra

    read(sock, buffer, BUFFER_SIZE);                         
    memset(buffer, NULL_CHAR, BUFFER_SIZE);                  
    sprintf(buffer, "%s\n%s", palavra_escondida, MSG_GUESS); 

    while (!game_over) 
    {
        enviar_mensagem(sock, buffer); 
        receber_mensagem(sock, buffer); 
        guess = buffer[0];              

        memset(buffer, NULL_CHAR, BUFFER_SIZE); // Limpa o buffer

        bool found = false;                 
        for (int i = 0; i < word_size; i++) 
        {
            bool isGuessCorrect = word_local[i] == guess; // Verifica se a letra suposta é igual a letra da palavra

            if (isGuessCorrect) 
            {
                palavra_escondida[i] = guess; 
                found = true;                
            }
        }

        if (!found) 
        {
            tent_restantes--;                                                                          
            sprintf(buffer, "%s %d\n%s\n%s", WRG_GUESS, tent_restantes, palavra_escondida, MSG_GUESS); 
            if (tent_restantes == 0)                                                                   
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
            game_over = true; 
            win = true;       
        }
    }

    sprintf(buffer, "%s%s\n%s\n", win ? MSG_WIN : MSG_LOSE, word_local, END_GAME); // Formata a mensagem para enviar para o cliente

    enviar_mensagem(sock, buffer); 

    pthread_exit(NULL); // Encerra a thread
    close(sock);       
}

void escolher_palavra_aleatoria(char *result)
{
    FILE *fp;                    // Ponteiro para o arquivo
    char line[MAX_WORD_SIZE];    
    int count = 0;               
    int random_line_number;      
    int current_line_number = 0; 

    fp = fopen("./resources/palavras.txt", "r"); // Abre o arquivo

    while (fgets(line, sizeof(line), fp) != NULL) // Enquanto não chegar ao final do arquivo
    {
        count++; // Incrementa o contador de linhas
    }

    srand(time(NULL));                   
    random_line_number = rand() % count; 

    rewind(fp); // Volta para o início do arquivo

    while (fgets(line, sizeof(line), fp) != NULL) // Enquanto não chegar ao final do arquivo
    {
        if (current_line_number == random_line_number) 
        {
            line[strcspn(line, "\n")] = '\0'; 
            strcpy(result, line);             
            break;                            
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

    address.sin_family = AF_INET;         
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT);      

    bind(server_fd, (struct sockaddr *)&address, sizeof(address)); // Associa o soquete ao endereço

    listen(server_fd, 3); 

    return server_fd; 
}

void receber_mensagem(int new_socket, char *buffer) // Recebe
{
    read(new_socket, buffer, BUFFER_SIZE);              // Lê a mensagem do cliente
    printf("Client %lu: %s\n", pthread_self(), buffer); 
}

void enviar_mensagem(int new_socket, char *buffer)
{
    send(new_socket, buffer, strlen(buffer), 0); // Envia a mensagem para o cliente
    memset(buffer, NULL_CHAR, BUFFER_SIZE);      // Limpa o buffer
}
