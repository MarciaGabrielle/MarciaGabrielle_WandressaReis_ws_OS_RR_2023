#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int inicializar_socket();
void enviar_mensagem(int sock, char *buffer);
void receber_mensagem(int sock, char *buffer);

// socket - túnel de comunicação entre dois processos
// address - endereço do socket
// buffer - buffer de dados

int main()
{
    int sock = inicializar_socket();
    char buffer[1024] = {0};

    while (1)
    {
        enviar_mensagem(sock, buffer);
        receber_mensagem(sock, buffer);
    }

    return 0;
}

int inicializar_socket()
{
    int sock = 0;                            // Inicializando o socket e a resposta do servidor
    struct sockaddr_in endereco_do_servidor; // Inicializando o endereço do servidor

    sock = socket(AF_INET, SOCK_STREAM, 0); // Criando o socket

    memset(&endereco_do_servidor, '0', sizeof(endereco_do_servidor)); // Zerando o endereço do servidor

    endereco_do_servidor.sin_family = AF_INET;   // Definindo o tipo de endereço
    endereco_do_servidor.sin_port = htons(PORT); // Definindo a porta

    inet_pton(AF_INET, "127.0.0.1", &endereco_do_servidor.sin_addr); // Definindo o endereço do servidor

    connect(sock, (struct sockaddr *)&endereco_do_servidor, sizeof(endereco_do_servidor)); // Conectando ao servidor

    return sock; // Retornando o socket
}

void enviar_mensagem(int sock, char *buffer)
{
    printf("Enter message for server: ");  // Enviando mensagem para o servidor
    scanf("%[^\n]%*c", buffer);            // Lendo a mensagem do usuário
    send(sock, buffer, strlen(buffer), 0); // Enviando a mensagem para o servidor
    memset(buffer, 0, sizeof(buffer));     // Zerando o buffer
}

void receber_mensagem(int sock, char *buffer)
{
    int resposta_do_servidor = read(sock, buffer, 1024); // Lendo a resposta do servidor
    printf("Server: %s\n", buffer);                      // Imprimindo a resposta do servidor
    memset(buffer, 0, sizeof(buffer));                   // Zerando o buffer
}