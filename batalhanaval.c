#include <stdio.h>
#include <locale.h>
#define BOARD_SIZE 10
#define SHIP_SIZE 3
#define SKILL_SIZE 5

// Inicializa o tabuleiro 10x10 com 0 (água)
void initializeBoard(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            board[i][j] = 0;
}

// Exibe o tabuleiro com caracteres distintos: ' ' (água), 'N' (navio), '*' (habilidade)
void printBoard(int board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\nTabuleiro de Batalha Naval:\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0)
                printf("  ");
            else if (board[i][j] == 3)
                printf("N ");
            else if (board[i][j] == 5)
                printf("* ");
        }
        printf("\n");
    }
    printf("\nLegenda: ' ' = Água, 'N' = Navio, '*' = Área de Habilidade\n");
}

// Posiciona um navio horizontal ou vertical, retorna 1 se bem-sucedido, 0 caso contrário
int placeShip(int board[BOARD_SIZE][BOARD_SIZE], int startRow, int startCol, int size, int isHorizontal) {
    // Validação de limites
    if (isHorizontal && startCol + size > BOARD_SIZE) return 0;
    if (!isHorizontal && startRow + size > BOARD_SIZE) return 0;
    if (startRow < 0 || startCol < 0) return 0;

    // Verificar sobreposição
    for (int i = 0; i < size; i++) {
        int row = isHorizontal ? startRow : startRow + i;
        int col = isHorizontal ? startCol + i : startCol;
        if (row >= BOARD_SIZE || col >= BOARD_SIZE || board[row][col] != 0) return 0;
    }

    // Posicionar navio
    for (int i = 0; i < size; i++) {
        int row = isHorizontal ? startRow : startRow + i;
        int col = isHorizontal ? startCol + i : startCol;
        board[row][col] = 3;
    }
    return 1;
}

// Posiciona um navio diagonal (crescente ou decrescente), retorna 1 se bem-sucedido, 0 caso contrário
int placeDiagonalShip(int board[BOARD_SIZE][BOARD_SIZE], int startRow, int startCol, int size, int isUpward) {
    // Validação de limites
    if (isUpward) {
        if (startRow + size > BOARD_SIZE || startCol + size > BOARD_SIZE) return 0;
    } else {
        if (startRow - size + 1 < 0 || startCol - size + 1 < 0) return 0;
    }

    // Verificar sobreposição
    for (int i = 0; i < size; i++) {
        int row = isUpward ? startRow + i : startRow - i;
        int col = isUpward ? startCol + i : startCol - i;
        if (row >= BOARD_SIZE || col >= BOARD_SIZE || row < 0 || col < 0 || board[row][col] != 0) return 0;
    }

    // Posicionar navio
    for (int i = 0; i < size; i++) {
        int row = isUpward ? startRow + i : startRow - i;
        int col = isUpward ? startCol + i : startCol - i;
        board[row][col] = 3;
    }
    return 1;
}

// Inicializa a matriz de habilidade em forma de cone (topo na linha 0, base expansiva)
void initializeConeSkill(int skill[SKILL_SIZE][SKILL_SIZE]) {
    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            // Cone: topo no centro da linha 0, expande para baixo
            int center = SKILL_SIZE / 2;
            if (j >= center - i && j <= center + i)
                skill[i][j] = 1; // Área afetada
            else
                skill[i][j] = 0; // Não afetada
        }
    }
}

// Inicializa a matriz de habilidade em forma de cruz (centro na posição SKILL_SIZE/2)
void initializeCrossSkill(int skill[SKILL_SIZE][SKILL_SIZE]) {
    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            // Cruz: afeta linha e coluna centrais
            if (i == SKILL_SIZE / 2 || j == SKILL_SIZE / 2)
                skill[i][j] = 1; // Área afetada
            else
                skill[i][j] = 0; // Não afetada
        }
    }
}

// Inicializa a matriz de habilidade em forma de octaedro (losango centrado)
void initializeOctahedronSkill(int skill[SKILL_SIZE][SKILL_SIZE]) {
    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            // Octaedro: forma de losango, baseado na distância de Manhattan
            int center = SKILL_SIZE / 2;
            int manhattan = abs(i - center) + abs(j - center);
            if (manhattan <= center)
                skill[i][j] = 1; // Área afetada
            else
                skill[i][j] = 0; // Não afetada
        }
    }
}

// Aplica uma habilidade no tabuleiro, centrada em (centerRow, centerCol)
void applySkill(int board[BOARD_SIZE][BOARD_SIZE], int skill[SKILL_SIZE][SKILL_SIZE], int centerRow, int centerCol) {
    int offset = SKILL_SIZE / 2;
    for (int i = 0; i < SKILL_SIZE; i++) {
        for (int j = 0; j < SKILL_SIZE; j++) {
            int boardRow = centerRow - offset + i;
            int boardCol = centerCol - offset + j;
            // Verifica limites do tabuleiro e se a posição é afetada pela habilidade
            if (boardRow >= 0 && boardRow < BOARD_SIZE && boardCol >= 0 && boardCol < BOARD_SIZE && skill[i][j] == 1) {
                // Preserva navios (3), mas marca áreas afetadas com 5
                if (board[boardRow][boardCol] != 3)
                    board[boardRow][boardCol] = 5;
            }
        }
    }
}

int main() {
	setlocale(LC_ALL, "Portuguese");
    int board[BOARD_SIZE][BOARD_SIZE];
    int coneSkill[SKILL_SIZE][SKILL_SIZE];
    int crossSkill[SKILL_SIZE][SKILL_SIZE];
    int octahedronSkill[SKILL_SIZE][SKILL_SIZE];

    // Inicializa o tabuleiro
    initializeBoard(board);

    // Posiciona quatro navios
    if (!placeShip(board, 2, 3, SHIP_SIZE, 1)) // Horizontal
        printf("Erro ao posicionar navio horizontal!\n");
    if (!placeShip(board, 5, 7, SHIP_SIZE, 0)) // Vertical
        printf("Erro ao posicionar navio vertical!\n");
    if (!placeDiagonalShip(board, 0, 0, SHIP_SIZE, 1)) // Diagonal crescente
        printf("Erro ao posicionar navio diagonal crescente!\n");
    if (!placeDiagonalShip(board, 9, 9, SHIP_SIZE, 0)) // Diagonal decrescente
        printf("Erro ao posicionar navio diagonal decrescente!\n");

    // Exibe o tabuleiro com navios
    printf("Tabuleiro após posicionar navios:\n");
    printBoard(board);

    // Inicializa matrizes de habilidades
    initializeConeSkill(coneSkill);
    initializeCrossSkill(crossSkill);
    initializeOctahedronSkill(octahedronSkill);

    // Aplica habilidades no tabuleiro
    applySkill(board, coneSkill, 4, 4); // Cone centrado em (4,4)
    applySkill(board, crossSkill, 7, 2); // Cruz centrada em (7,2)
    applySkill(board, octahedronSkill, 2, 7); // Octaedro centrado em (2,7)

    // Exibe o tabuleiro final com navios e habilidades
    printf("Tabuleiro após aplicar habilidades:\n");
    printBoard(board);

    return 0;
}
