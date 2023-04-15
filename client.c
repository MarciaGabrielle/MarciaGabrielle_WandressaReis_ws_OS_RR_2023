/*
Coisas que o professor provavelmente vai questionar:

OBS: Perguntei pro ChatGPT, as respostas estão abaixo de cada pergunta

1 - O que é um socket?
    Um socket é uma interface de programação que permite a comunicação entre processos em diferentes computadores por meio da rede. Ele permite que um
    processo em uma máquina se conecte a um processo em outra máquina e envie e receba dados através de uma conexão de rede. Em resumo, o socket é um ponto
    final da comunicação em uma rede e pode ser usado para estabelecer conexões TCP/IP, UDP, entre outros.

2 - O que é o AF_INET e o SOCK_STREAM?
    AF_INET é uma constante que representa o endereço da família de protocolos IPv4 usada pelo socket. "AF" significa Address Family (Família de Endereços)
    e "INET" significa Internet Protocol (Protocolo de Internet). O AF_INET é um dos vários tipos de famílias de endereços suportados pela API de soquetes
    e é usado para comunicação entre dispositivos em uma rede que usam o protocolo IPv4.

    SOCK_STREAM é um tipo de socket usado para comunicação orientada a conexão confiável. Ele fornece um fluxo de dados bidirecional, que é transmitido em
    sequência e sem perda de dados ou duplicação. Essa confiabilidade é obtida por meio de um protocolo de transporte confiável, como o protocolo TCP
    (Transmission Control Protocol).

    O SOCK_STREAM é adequado para aplicativos que requerem transmissão de dados confiável e sequencial, como transferência de arquivos, comunicação entre
    cliente e servidor, etc. Ele usa o TCP como protocolo de transporte subjacente para garantir que os dados sejam entregues ao destino corretamente e
    em ordem.

3 - O que é o htons?
    A função htons é uma função em C que converte o valor de uma porta (porta é um número inteiro que identifica um processo de rede em um host) da ordem
    dos bytes do host para a ordem dos bytes da rede. A ordem dos bytes da rede é definida como a ordem em que os bytes são transmitidos pela rede, que é
    a ordem de byte "big-endian". Em algumas arquiteturas de computadores, como o Intel x86, a ordem dos bytes do host é a ordem de byte "little-endian".

    O uso da função htons é importante porque as diferentes arquiteturas de computadores podem armazenar valores em diferentes ordens de bytes, e a rede
    exige uma ordem de byte específica para transmitir os dados corretamente. Portanto, a função htons é usada para garantir que os dados sejam transmitidos
    corretamente pela rede, independentemente da arquitetura do computador.

4 - O que é o inet_pton?
    inet_pton é uma função da biblioteca de rede em C que converte um endereço IP em sua representação binária. A função recebe como entrada uma string
    contendo um endereço IP no formato de pontos decimais (por exemplo, "192.0.2.1") e um ponteiro para um endereço em uma estrutura in_addr que conterá
    a representação binária correspondente do endereço IP.

    A função retorna um valor inteiro indicando se a conversão foi bem sucedida ou não. Se a conversão foi bem sucedida, a função retorna um valor diferente
    de zero. Caso contrário, retorna zero.

    O nome inet_pton significa "Internet address to presentation string" (endereço da Internet para string de apresentação). Essa função é uma das várias
    funções disponíveis na biblioteca de rede em C para manipular endereços IP e outros aspectos da comunicação em rede.

5 - O que a string nessa linha "scanf("%[^\n]%*c", buffer);" faz?
    A string de formato usada aqui tem duas especificações de conversão, %[^\n] e %*c.

    %[^\n]: Essa especificação de conversão instrui scanf a ler uma string de caracteres da entrada padrão até encontrar um caractere de nova linha (\n)
    ou o final da entrada. O [^\n] diz ao scanf para combinar com qualquer caractere, exceto o caractere de nova linha, o que significa que ele lerá toda
    a linha de entrada até o final da linha. Os caracteres lidos são armazenados no array de buffer fornecido como argumento para scanf.

    %*c: Esta especificação de conversão diz ao scanf para ler um único caractere da entrada padrão e descartá-lo, sem armazená-lo em nenhuma variável.
    O * em %*c indica que o caractere deve ser lido e descartado, mas não armazenado em nenhum lugar. Isso é necessário porque quando o scanf lê a linha
    de entrada usando %[^\n], ele para de ler logo antes do caractere de nova linha, mas deixa o caractere de nova linha na entrada. A especificação de
    conversão %*c consome esse caractere de nova linha, permitindo que o scanf comece a ler a próxima linha de entrada corretamente.

    Portanto, colocando tudo junto, a chamada de scanf lê uma linha de entrada da entrada padrão para o array de buffer e, em seguida, descarta o caractere
    de nova linha que o segue, deixando a entrada pronta para ler a próxima linha de entrada.Isso pode ser útil para ler linhas de texto da entrada padrão,
    pois permite ler uma linha inteira de uma só vez, em vez de ler um caractere de cada vez.

6 - O que a função strncmp faz?
    A função strncmp é uma função da linguagem de programação C que é usada para comparar duas strings (cadeias de caracteres) em C. Ela compara os n primeiros
    caracteres de duas strings e retorna um valor inteiro que indica se as strings são iguais, ou se a primeira string é maior ou menor que a segunda string,
    de acordo com a ordem lexicográfica.
*/

#include <stdio.h>      // Biblioteca padrão de entrada e saída
#include <stdlib.h>     // Biblioteca padrão de funções
#include <string.h>     // Biblioteca padrão de manipulação de strings
#include <stdbool.h>    // Biblioteca padrão de variáveis booleanas
#include <sys/socket.h> // Biblioteca padrão de soquetes
#include <arpa/inet.h>  // Biblioteca padrão de endereços de internet
#include <unistd.h>     // Biblioteca padrão de funções de entrada e saída

#define NULL_CHAR '\0' // Caractere nulo

#define PORT 8080               // Porta do servidor
#define BUFFER_SIZE 2048        // Tamanho do buffer
#define END_GAME "Fim de jogo!" // Mensagem de fim de jogo

bool game_over = false; // Variável booleana para indicar o fim do jogo

int inicializar_socket();                      // Função para inicializar o socket
void enviar_mensagem(int sock, char *buffer);  // Função para enviar mensagem
void receber_mensagem(int sock, char *buffer); // Função para receber mensagem

int main()
{
    int sock = inicializar_socket();        // Inicializando o socket
    char buffer[BUFFER_SIZE] = {NULL_CHAR}; // Buffer para armazenar as mensagens

    printf("Como você quer ser chamado no jogo? "); // Perguntando o nome do jogador
    enviar_mensagem(sock, buffer);                  // Enviando o nome do jogador

    while (true) // Loop infinito
    {
        receber_mensagem(sock, buffer); // Recebendo mensagem do servidor

        if (game_over) // Verificando se o jogo acabou
        {
            break; // Encerrando o loop
        }

        enviar_mensagem(sock, buffer); // Enviando mensagem para o servidor
    }

    return 0; // Encerrando o programa
}

int inicializar_socket()
{
    int sock = 0;                            // Descritor do socket
    struct sockaddr_in endereco_do_servidor; // Estrutura que armazena o endereço do servidor

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
    scanf("%[^\n]%*c", buffer);             // Lendo a mensagem do usuário
    send(sock, buffer, strlen(buffer), 0);  // Enviando a mensagem para o servidor
    memset(buffer, NULL_CHAR, BUFFER_SIZE); // Zerando o buffer
}

void receber_mensagem(int sock, char *buffer)
{
    read(sock, buffer, BUFFER_SIZE); // Lendo a resposta do servidor
    printf("%s", buffer);            // Imprimindo a resposta do servidor

    if (strncmp(buffer + strlen(buffer) - strlen(END_GAME) - 1, END_GAME, strlen(END_GAME)) == 0) // Verificando se o jogo acabou
    {
        game_over = true; // Finalizando o jogo
    }

    memset(buffer, NULL_CHAR, BUFFER_SIZE); // Zerando o buffer
}
