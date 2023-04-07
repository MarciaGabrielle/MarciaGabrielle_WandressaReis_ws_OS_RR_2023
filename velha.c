#include <stdio.h>

int main() {
    char jogador1[20], jogador2[20];
    int vez = 1, jogada, vencedor = 0;
    char tabuleiro[3][3] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}};

    printf("Jogador 1, digite seu nome: ");
    scanf("%s", jogador1);

    printf("Jogador 2, digite seu nome: ");
    scanf("%s", jogador2);

    while (vencedor == 0) {
        // Imprime o tabuleiro
        printf("\n");
        printf("%c | %c | %c\n", tabuleiro[0][0], tabuleiro[0][1], tabuleiro[0][2]);
        printf("--|---|--\n");
        printf("%c | %c | %c\n", tabuleiro[1][0], tabuleiro[1][1], tabuleiro[1][2]);
        printf("--|---|--\n");
        printf("%c | %c | %c\n", tabuleiro[2][0], tabuleiro[2][1], tabuleiro[2][2]);

        // Pede a jogada do jogador da vez
        if (vez == 1) {
            printf("%s, digite a posição que deseja jogar (1-9): ", jogador1);
        } else {
            printf("%s, digite a posição que deseja jogar (1-9): ", jogador2);
        }
        scanf("%d", &jogada);

        // Verifica se a posição escolhida é válida
        if (jogada < 1 || jogada > 9) {
            printf("Posição inválida! Tente novamente.\n");
            continue;
        }

        // Converte a posição para a linha e coluna correspondente no tabuleiro
        int linha = (jogada - 1) / 3;
        int coluna = (jogada - 1) % 3;

        // Verifica se a posição escolhida já foi jogada
        if (tabuleiro[linha][coluna] == 'X' || tabuleiro[linha][coluna] == 'O') {
            printf("Posição já jogada! Tente novamente.\n");
            continue;
        }

        // Realiza a jogada
        if (vez == 1) {
            tabuleiro[linha][coluna] = 'X';
            vez = 2;
        } else {
            tabuleiro[linha][coluna] = 'O';
            vez = 1;
        }

        // Verifica se há um vencedor
        for (int i = 0; i < 3; i++) {
            if (tabuleiro[i][0] == tabuleiro[i][1] && tabuleiro[i][1] == tabuleiro[i][2]) {
                vencedor = vez;
            }
            if (tabuleiro[0][i] == tabuleiro[1][i] && tabuleiro[1][i] == tabuleiro[2][i]) {
                vencedor = vez;
            }
        }
        if (tabuleiro[0][0] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][2]);
    }
}