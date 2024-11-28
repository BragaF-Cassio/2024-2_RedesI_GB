#ifndef TABULEIRO_H
#define TABULEIRO_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LINHAS 10
#define COLUNAS 10
#define QUANTIDADE_BARCOS 6
#define TOTAL_CASAS 19

#define NULL_TARGET '~'
#define BOAT_TARGET '*'
#define HIDDEN_TARGET '.'
#define WATER_SHOT 'O'

enum EstadoDoJogo {
  CONECTANDO,
  POSICIONANDO_BARCOS,
  AGUARDANDO_POSICIONAR_BARCOS,
  MEU_TURNO,
  AGUARDANDO_TURNO_ADVERSARIO,
  RESULTADO
}; 

typedef struct structBarcos {
    int x, y;
    int tamanho;
    char direcao; // 'H' para horizontal, 'V' para vertical
} Barcos;

extern char meuTabuleiro[LINHAS][COLUNAS];
extern char tabuleiroAdversario[LINHAS][COLUNAS];
extern char tabelaTiros[LINHAS][COLUNAS];
extern char tabelaTirosInimigo[LINHAS][COLUNAS];

extern enum EstadoDoJogo game;
extern Barcos meusBarcos[QUANTIDADE_BARCOS];
extern char json_meus_barcos[1000];

extern int minhasCasasAcertadas;
extern int inimigoCasasAcertadas;

void mostrar_tabuleiro(char *tabuleiro);
void limpa_tabuleiros();
int posicionar_barco(char *tabuleiro, Barcos *barco);
int atira(char *tabuleiro_base, char *tabuleiro_alvo, int linha, int coluna);
void gera_posicoes_barcos();
void gera_json_meus_barcos();
void ler_do_json(char *json);

#endif