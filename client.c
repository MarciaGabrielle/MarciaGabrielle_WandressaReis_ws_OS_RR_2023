
#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>     
#include <stdbool.h>    
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>     

#define NULL_CHAR '\0' 

#define PORT 8080               
#define BUFFER_SIZE 2048        
#define END_GAME "Fim de jogo!" 

bool game_over = false; 

int inicializar_socket();                      
void enviar_mensagem(int sock, char *buffer);  
void receber_mensagem(int sock, char *buffer); 

int main()
{
    int sock = inicializar_socket();        
    char buffer[BUFFER_SIZE] = {NULL_CHAR};

    printf("Como você quer ser chamado no jogo? "); 
    enviar_mensagem(sock, buffer);                 

    while (true) 
    {
        receber_mensagem(sock, buffer); 

        if (game_over) 
        {
            break; 
        }

        enviar_mensagem(sock, buffer); 
    }

    return 0; 
}
 
int inicializar_socket()
{
    int sock = 0;                          
    struct sockaddr_in endereco_do_servidor; 

    sock = socket(AF_INET, SOCK_STREAM, 0); // Criando o socket

    memset(&endereco_do_servidor, NULL_CHAR, sizeof(endereco_do_servidor)); // Zerando a estrutura de endereço

    endereco_do_servidor.sin_family = AF_INET;   // Definindo o tipo de endereço
    endereco_do_servidor.sin_port = htons(PORT); // Definindo a porta do servidor

    inet_pton(AF_INET, "127.0.0.1", &endereco_do_servidor.sin_addr); // Definindo o endereço do servidor

    connect(sock, (struct sockaddr *)&endereco_do_servidor, sizeof(endereco_do_servidor)); // Conectando ao servidor

    return sock; // Retornando o descritor do socket
}

void enviar_mensagem(int sock, char *buffer)
{
    scanf("%[^\n]%*c", buffer);            
    send(sock, buffer, strlen(buffer), 0);
    memset(buffer, NULL_CHAR, BUFFER_SIZE); 
}

void receber_mensagem(int sock, char *buffer)
{
    read(sock, buffer, BUFFER_SIZE); 
    printf("%s", buffer);            

    if (strncmp(buffer + strlen(buffer) - strlen(END_GAME) - 1, END_GAME, strlen(END_GAME)) == 0) 
    {
        game_over = true; 
    }

    memset(buffer, NULL_CHAR, BUFFER_SIZE); 
}
