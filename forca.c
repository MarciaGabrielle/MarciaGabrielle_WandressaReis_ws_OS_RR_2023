#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_ATTEMPTS 5
#define WORD_SIZE 20

char word[WORD_SIZE];
char hidden_word[WORD_SIZE];
int attempts_left = MAX_ATTEMPTS;
bool game_over = false;
pthread_mutex_t mutex;

void* handle_guess(void* arg) {
    char guess;
    while (!game_over) {
        printf("Guess a letter: ");
        scanf("%c", &guess);
        getchar(); // consume newline
        pthread_mutex_lock(&mutex);
        int i;
        bool found = false;
        for (i = 0; i < strlen(word); i++) {
            if (word[i] == guess) {
                hidden_word[i] = guess;
                found = true;
            }
        }
        if (!found) {
            attempts_left--;
            printf("Wrong guess! Attempts left: %d\n", attempts_left);
            if (attempts_left == 0) {
                printf("Game over! The word was: %s\n", word);
                game_over = true;
            }
        } else {
            printf("Correct guess!\n");
            printf("Hidden word: %s\n", hidden_word);
            if (strcmp(hidden_word, word) == 0) {
                printf("You win!\n");
                game_over = true;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main() {
    printf("Enter a word: ");
    fgets(word, WORD_SIZE, stdin);
    word[strcspn(word, "\n")] = '\0';
    memset(hidden_word, '_', strlen(word));
    printf("Hidden word: %s\n", hidden_word);

    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&tid, NULL, handle_guess, NULL);

    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}