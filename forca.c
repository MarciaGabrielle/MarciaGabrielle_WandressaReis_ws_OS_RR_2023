#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_ATTEMPTS 5
#define WORD_SIZE 20

//definindo vetores
char word[WORD_SIZE];
char palavra_escondida[WORD_SIZE];
int tent_restantes = MAX_ATTEMPTS; // 5 tentativas
bool game_over = false;
pthread_mutex_t mutex; //serve para proteger uma região crítica.

//definindo a função principal do jogo
void* handle_guess(void* arg) {
    char guess;
    while (!game_over) {
        printf("\nDigite uma letra: ");
        scanf("%c", &guess);
        getchar(); // consume newline
        pthread_mutex_lock(&mutex); //obter exclusão mútua
        int i;
        bool found = false;
        for (i = 0; i < strlen(word); i++) {
            if (word[i] == guess) {
                palavra_escondida[i] = guess;
                found = true;
            }
        }
        if (!found) {
            tent_restantes--;
            printf("Letra incorreta! Tentativas restantes: %d\n", tent_restantes);
            if (tent_restantes == 0) {
                printf("Você perdeu! A palavra era: %s\n", word);
                game_over = true;
            }
        } else {
            printf("Suposição correta\n");
            printf("Palavra escondida: %s\n", palavra_escondida);
            if (strcmp(palavra_escondida, word) == 0) {
                printf("Você ganhou!\n");
                game_over = true;
            }
        }
        pthread_mutex_unlock(&mutex); //liberar a região crítica
    }
    pthread_exit(NULL);
}

int main() {
    printf("Escolha uma palavra: ");
    fgets(word, WORD_SIZE, stdin); //fgets - lê uma cadeia de caracteres do argumento stream de entrada e o armazena em str
    word[strcspn(word, "\n")] = '\0'; //strcspn - recebe duas strings como entrada, string_1 e string_2 como seu argumento e pesquisa string_1
    memset(palavra_escondida, '_', strlen(word)); //memset - usado para preencher um bloco de memória com um valor particular
    printf("Palavra escondida: %s\n", palavra_escondida);

    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&tid, NULL, handle_guess, NULL);

    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}