#include "tabuleiro.h"

char meuTabuleiro[LINHAS][COLUNAS] = {NULL_TARGET};
char tabuleiroAdversario[LINHAS][COLUNAS] = {NULL_TARGET};
char tabelaTiros[LINHAS][COLUNAS] = {' '};
char tabelaTirosInimigo[LINHAS][COLUNAS] = {' '};

char json_meus_barcos[1000];
int minhasCasasAcertadas = 0;
int inimigoCasasAcertadas = 0;

enum EstadoDoJogo game = CONECTANDO;
Barcos meusBarcos[QUANTIDADE_BARCOS];

void limpa_tabuleiros() {
    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++){
            meuTabuleiro[i][j] = NULL_TARGET;
            tabuleiroAdversario[i][j] = NULL_TARGET;
            tabelaTiros[i][j] = HIDDEN_TARGET;
            tabelaTirosInimigo[i][j] = HIDDEN_TARGET;
        }
    }
}

void mostrar_tabuleiro(char *tabuleiro) {
    printf("\n");
    printf("--------------------------------------");
    printf("\n  ");

    for(int j = 0; j < COLUNAS; j++){
        printf(" %d ", j);
    }
    printf("\n");

    for(int i = 0; i < LINHAS; i++){
        printf("%d  %c ", i, tabuleiro[i*LINHAS]);
        for(int j = 1; j < COLUNAS; j++){
            printf(" %c ", tabuleiro[(i*LINHAS)+j]);
        }
        printf("\n");
    }
}

int posicionar_barco(char *tabuleiro, Barcos *barco) {
    if(barco->direcao == 'H'){
        if(barco->x + barco->tamanho > COLUNAS) return -1;
        for (int i = 0; i < barco->tamanho; i++) {
            if (tabuleiro[(barco->y*COLUNAS) + barco->x + i] != NULL_TARGET) return -2;
        }

        for (int i = 0; i < barco->tamanho; i++) {
            tabuleiro[(barco->y*COLUNAS) + barco->x + i] = BOAT_TARGET;
        }
    } else {
        if(barco->y + barco->tamanho > LINHAS) return -1;
        for (int i = 0; i < barco->tamanho; i++) {
            if (tabuleiro[(barco->y*COLUNAS) + barco->x + i*COLUNAS] != NULL_TARGET) return -2;
        }

        for (int i = 0; i < barco->tamanho; i++) {
            tabuleiro[(barco->y*COLUNAS) + barco->x + i*COLUNAS] = BOAT_TARGET;
        }
    }

    return 1;
}

int atira(char *tabuleiro_base, char *tabuleiro_alvo, int linha, int coluna) {
    if(linha < 0 || linha >= LINHAS || coluna < 0 || coluna >= COLUNAS) {
        printf("Escolha uma regiao valida, tiro fora!\n");
        return 0;
    }

    if(tabuleiro_alvo[linha*COLUNAS + coluna] != HIDDEN_TARGET) {
        printf("Alvo ja foi atingido, escolha uma regiao valida!\n");
        return 0;
    }

    if(tabuleiro_base[linha*COLUNAS + coluna] != NULL_TARGET) {
        tabuleiro_alvo[linha*COLUNAS + coluna] = BOAT_TARGET;
        printf("Acertou!\n");
        return 1;
    } else {
        tabuleiro_alvo[linha*COLUNAS + coluna] = WATER_SHOT;
        printf("Errou :(\n");
        return -1;
    }
}

void gera_posicoes_barcos() {
    srand(time(NULL));   // Initialization, should only be called once.
    double randomNum;

    meusBarcos[0].tamanho = 5;
    meusBarcos[1].tamanho = 4;
    meusBarcos[2].tamanho = 3;
    meusBarcos[3].tamanho = 3;
    meusBarcos[4].tamanho = 2;
    meusBarcos[5].tamanho = 2;

    for(int i = 0; i < QUANTIDADE_BARCOS; i++) {
        do {
            //printf("Tentando posicionar o barco...\n");
            randomNum = ((double)rand()/RAND_MAX);
            meusBarcos[i].direcao = (randomNum > 0.5) ? 'H' : 'V';
            randomNum = ((double)rand()/RAND_MAX);
            meusBarcos[i].x = (int)(randomNum*9);
            randomNum = ((double)rand()/RAND_MAX);
            meusBarcos[i].y = (int)(randomNum*9);
        } while(posicionar_barco(&meuTabuleiro[0][0], &meusBarcos[i]) <= 0);
    }

    gera_json_meus_barcos();

    //printf("\n%s\n", json_meus_barcos);
    //ler_do_json();
}

void gera_json_meus_barcos() {
    char meus_barcos[QUANTIDADE_BARCOS][200];
    char posicaoGeral[100];
    char tipo_barco[50];
    char posicoes[5][10];

    memset(json_meus_barcos, '\0', sizeof(json_meus_barcos)); 
    for (int i = 0; i < QUANTIDADE_BARCOS; i++)
    {
        int addX = meusBarcos[i].x, addY = meusBarcos[i].y;
        for(int j = 0; j < meusBarcos[i].tamanho; j++) {
            sprintf(posicoes[j], "[%d, %d]", addX, addY);
            if(meusBarcos[i].direcao == 'H') {
                addX++;
            } else {
                addY++;
            }
        }
        
        switch (meusBarcos[i].tamanho)
        {
            case 5:
                sprintf(tipo_barco, "\"tipo\": \"porta-avioes\",");
                sprintf(posicaoGeral, "\"posicoes\": [%s, %s, %s, %s, %s]", posicoes[0], posicoes[1], posicoes[2], posicoes[3], posicoes[4]);
                break;

            case 4:
                sprintf(tipo_barco, "\"tipo\": \"encouracado\",");
                sprintf(posicaoGeral, "\"posicoes\": [%s, %s, %s, %s]", posicoes[0], posicoes[1], posicoes[2], posicoes[3]);
                break;

            case 3:
                sprintf(tipo_barco, "\"tipo\": \"cruzador\",");
                sprintf(posicaoGeral, "\"posicoes\": [%s, %s, %s]", posicoes[0], posicoes[1], posicoes[2]);
                break;

            case 2:
                sprintf(tipo_barco, "\"tipo\": \"destroier\",");
                sprintf(posicaoGeral, "\"posicoes\": [%s, %s]", posicoes[0], posicoes[1]);
                break;
            
            default:
                break;
        }

        sprintf(meus_barcos[i],"{%s %s}", &tipo_barco[0], &posicaoGeral[0]);
    }
    
    sprintf(json_meus_barcos,"[%s, %s, %s, %s, %s, %s]", meus_barcos[0], meus_barcos[1], meus_barcos[2], meus_barcos[3], meus_barcos[4], meus_barcos[5]);
}

void ler_do_json(char *json) {
    //char *json = "[{\"tipo\": \"porta-avioes\", \"posicoes\": [[3, 5], [3, 6], [3, 7], [3, 8], [3, 9]]}, {\"tipo\": \"encouracado\", \"posicoes\": [[5, 0], [5, 1], [5, 2], [5, 3]]}, {\"tipo\": \"cruzador\", \"posicoes\": [[1, 3], [1, 4], [1, 5]]}, {\"tipo\": \"cruzador\", \"posicoes\": [[2, 0], [2, 1], [2, 2]]}, {\"tipo\": \"destroier\", \"posicoes\": [[1, 1], [1, 2]]}, {\"tipo\": \"destroier\", \"posicoes\": [[0, 6], [0, 7]]}]";
    char buf[7];
    int offset = 0;
    int posX, posY;
    int tamanhoJson = strlen(json);

    int contadorCasas = 0;

    while((++offset) + 6 < tamanhoJson) {
        strncpy(buf, (json+(offset*sizeof(char))),sizeof(buf));
        posX = -1;
        posY = -1;
        sscanf(buf, "[%d, %d]", &posX, &posY);

        if(posX >= 0 && posX <= 9 && posY >= 0 && posY <= 9) {
            //printf("\nAchou posicoes %d e %d\n", posX, posY);
            tabuleiroAdversario[posY][posX] = BOAT_TARGET;
            contadorCasas++;
            if(contadorCasas == TOTAL_CASAS) break;
        }
    }

    if(contadorCasas == TOTAL_CASAS) {
        printf("\nEstado de jogo do adversario recebido!\n");
    } else {
        printf("\nTentou ler do JSON, mas falhou.\n");
    }

    //tabuleiroAdversario
}