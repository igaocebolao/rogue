#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <utility>
#include <sstream>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <vector>
#include <cstdlib>


using namespace std;

// --- Dimensões e limites ---
const int ALTURA      = 25;
const int LARGURA     = 56;
const int MAX_SALAS   = 16;
const int MAX_RANKING = 100;

// --- Tipos de tile ---
const int VAZIO             = 0;
const int PAREDE            = 1;
const int CAMINHO           = 2;
const int PORTA             = 3;
const int CAMINHO_ESCONDIDO = 4;
const int PORTAL            = 5;
const int BAU               = 6;
const int XP                = 7;
const int NPC               = 8;
const int MORTO_VIVO        = 9;
const int COBRA             = 10;
const int BOSS              = 11;


// --- Structs ---
// Representa uma posição no mapa em coordenadas de linha e coluna.
struct Pos { int x, y; };

// Define os limites de cada sala descoberta no mapa.
struct Sala {
    int x1, y1, x2, y2;
    bool descoberta;
};

// Estado do jogador, incluindo inventário, atributos e visualização.
struct Jogador {
    Pos pos;
    int vida, vida_max;
    int salas_descobertas;
    int xp;
    int chave;
    int espada;
    int escudo;
    int escudo_quebrado;
    int faca;
    int pocao_vida;
    int carne;
    int armadura;
    int trevo_sorte;
    int dano_bonus;
    int defesa_bonus;
    int chance_desvio;
    int visual_estado;      // 0 normal, 1 curado, 2 armadura ativa, 3 escudo ativo, 4 espada, 5 faca
    int visual_turnos;      // quantos frames o visual temporário permanece
};

// --- Globais ---
int     mapa[ALTURA][LARGURA];
bool    visivel[ALTURA][LARGURA];
Sala    salas[MAX_SALAS];
int     qtd_salas = 0;
int max_inimigos_fase = 6; // Quantidade máxima de inimigos que aparecem em cada fase.
int dificuldade = 2; // 1 = fácil, 2 = médio, 3 = difícil
const int DIFICULDADE_FACIL = 1;
const int DIFICULDADE_MEDIA = 2;
const int DIFICULDADE_DIFICIL = 3;
Jogador jogador;
bool minerador_conversado = false;
bool sala_boss_criada = false;
int boss_vida = 0;
bool boss_derrotado = false;
int boss_x = 0;
int boss_y = 0;
bool ranking_salvo_por_vitoria = false;
bool computador_jogando = false;
string jogador_nome;
chrono::steady_clock::time_point inicio_partida;
int fase_atual = 1;
int inimigo_x[16] = {}, inimigo_y[16] = {}, inimigo_vivo[16] = {}, inimigo_vida[16] = {};
int cobra_x[5] = {}, cobra_y[5] = {}, cobra_viva[5] = {}, cobra_vida[5] = {};

// Patrulha dos inimigos: eixo (0=horizontal, 1=vertical) e direcao atual (+1 ou -1)
int inimigo_eixo[16];
int inimigo_dir[16];


const int mapa_original[ALTURA][LARGURA] = {
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0,
1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	4, 	4, 	4, 	4, 	0, 	0, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	4, 	4, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	0, 	0, 	4, 	0, 	0, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	0, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2,  2, 	3, 	4, 	4, 	4, 	4, 	4, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	3, 	4, 	4, 	0, 	0, 	4, 	0, 	4, 	4, 	4, 	3, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	3, 	4, 	4, 	4, 	4, 	4, 	3, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	0, 	0, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	4, 	4, 	4, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	1, 	1, 	3, 	1, 	1, 	1, 	0, 	4, 	4, 	0, 	4, 	0, 	4, 	3, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	4, 	4, 	0, 	1, 	1, 	1, 	3, 	1, 	1, 	1,
0, 	0, 	4, 	4, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	4, 	4, 	4, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	4, 	4, 	0, 	0, 	0,
0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	1, 	3, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0,
1, 	1, 	1, 	3, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	8, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	3, 	4, 	4, 	4, 	4, 	1, 	1, 	1, 	1, 	3, 	1, 	1, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	4, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	3, 	4, 	4, 	0, 	0, 	0, 	4, 	4, 	3, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	4, 	0, 	4, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	3, 	4, 	4, 	3, 	2, 	2, 	2, 	2, 	3, 	4, 	4, 	3, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	4, 	4, 	4, 	0, 	0, 	1, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	0, 	4, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	1, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	4, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	1, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	4, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	4, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	0, 	0, 	4, 	4, 	4, 	4, 	4, 	4, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	4, 	4, 	4, 	4, 	4, 	4, 	4, 	4, 	4, 	4, 	0, 	4, 	4, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	1,
0, 	0, 	0, 	0, 	4, 	4, 	4, 	4, 	4, 	4, 	4, 	0, 	0, 	0, 	0, 	0, 	4, 	4, 	4, 	4, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	4, 	4, 	0, 	0, 	0, 	0, 	4, 	4, 	4, 	4, 	0, 	0, 	0, 	4, 	4, 	4, 	0, 	0,
0, 	0, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	1, 	1, 	3, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	1,
1, 	1, 	1, 	1, 	3, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	4, 	0, 	0, 	0, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	0, 	0, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	4, 	4, 	4, 	4, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	3, 	4, 	0, 	0, 	0, 	4, 	4, 	0, 	4, 	3, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	0, 	0, 	4, 	4, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	4, 	4, 	0, 	0, 	4, 	4, 	4, 	4, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	2, 	1,
1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	4, 	4, 	4, 	4, 	0, 	4, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1, 	1
};


const int mapa_original_fase2[ALTURA][LARGURA] = {
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,
0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	1,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,
0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	4,	0,	1,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,	0,	0,	0,	1,	3,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,
0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	3,	4,	4,	4,	4,	4,	3,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	3,	4,	4,	4,	4,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	1,	0,	0,	4,	0,	0,	1,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,	0,	4,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	4,	0,	0,	1,	1,	1,	3,	1,	0,	0,	0,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	0,	4,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	1,	8,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	4,	0,	0,	4,	1,	1,	1,	1,	1,	1,	1,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	1,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	4,	0,	0,	4,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	3,	2,	2,	2,	2,	3,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	0,	0,	4,	0,	0,	4,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	4,	0,	0,	0,	4,	0,	0,	4,	0,	0,	4,	0,	0,	0,	1,	2,	2,	2,	2,	1,	4,	0,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	4,	0,	0,	4,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	0,	0,	0,	0,	4,	4,	4,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	4,	4,	4,	0,	1,	2,	2,	2,	2,	1,	4,	0,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	4,	4,	4,	4,	3,	2,	2,	2,	2,	2,	1,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	4,	0,	1,	1,	1,	1,	1,	1,	4,	0,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	4,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	4,	0,	1,	2,	2,	2,	2,	1,	4,	0,	4,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	4,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	3,	1,	0,	1,	1,	1,	1,	3,	1,	1,	1,	1,	4,	0,	1,	2,	2,	2,	2,	3,	4,	0,	4,	0,	1,	1,	1,	1,	1,	3,	1,	1,	1,	1,	0,	4,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	4,	4,	3,	2,	2,	2,	2,	1,	0,	0,	4,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	4,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	1,	2,	2,	2,	2,	1,	0,	0,	4,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	4,	0,	0,	0,	1,	2,	2,	2,	2,	2,	1,
0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	1,	2,	2,	2,	2,	1,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	4,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,
0,	0,	0,	1,	1,	1,	1,	1,	3,	1,	1,	1,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	3,	4,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	4,	4,	4,	0,	0,	1,	1,	3,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	4,	4,	4,	4,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
};

const int mapa_boss[ALTURA][LARGURA] = {
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
};

// ===============================
// DANO POR CONTATO (INIMIGO)
// ===============================
static bool aplicar_dano_inimigo(int &vida){ // controla dano ao jogador 1 quando encosta no inimigo
    static bool tem_ultimo = false;
    static chrono::steady_clock::time_point ultimo_dano{};
    const int I_contato = 100;

    auto agora = chrono::steady_clock::now();
    if(tem_ultimo){
        auto delta_contato = chrono::duration_cast<chrono::milliseconds>(agora - ultimo_dano).count();
        if(delta_contato < I_contato){
            return false;
        }
    }
    tem_ultimo = true;
    ultimo_dano = agora;

    int dano_contato = 8;
    switch (dificuldade) {
        case DIFICULDADE_FACIL: dano_contato = 6; break;
        case DIFICULDADE_DIFICIL: dano_contato = 10; break;
        default: dano_contato = 8; break;
    }

    vida = vida - dano_contato;
    return true;
}


// ---------------------------------------------------------------------------
// Utilitários de console
// ---------------------------------------------------------------------------

int probabilidade_por_dificuldade(int facil, int media, int dificil) {
    switch (dificuldade) {
        case DIFICULDADE_FACIL: return facil;
        case DIFICULDADE_DIFICIL: return dificil;
        default: return media;
    }
}

void limpar_tela() {
    system("cls");
}

void posicionar_cursor(int linha, int col) {
    COORD pos = { static_cast<SHORT>(col), static_cast<SHORT>(linha) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// ---------------------------------------------------------------------------
// Visibilidade
// ---------------------------------------------------------------------------

void resetar_visibilidade() {
    for (int i = 0; i < ALTURA; ++i){
        for (int j = 0; j < LARGURA; ++j){
            visivel[i][j] = false;
        }
    }
}

void revelar_sala(int indice) {
    if (indice < 0 || indice >= qtd_salas) return;
    if (!salas[indice].descoberta) {
        salas[indice].descoberta = true;
        ++jogador.salas_descobertas;
    }
    for (int i = salas[indice].x1; i <= salas[indice].x2; ++i)
        for (int j = salas[indice].y1; j <= salas[indice].y2; ++j)
            if (i >= 0 && i < ALTURA && j >= 0 && j < LARGURA)
                visivel[i][j] = true;
}

int encontrar_sala(int x, int y) {
    for (int i = 0; i < qtd_salas; ++i)
        if (x >= salas[i].x1 && x <= salas[i].x2 && y >= salas[i].y1 && y <= salas[i].y2)
            return i;
    return -1;
}

void revelar_tile_corredor(int x, int y) {
    if (x < 0 || x >= ALTURA || y < 0 || y >= LARGURA) return;
    int t = mapa[x][y];
    if (t == CAMINHO_ESCONDIDO || t == PORTA || t == CAMINHO)
        visivel[x][y] = true;
}

// ---------------------------------------------------------------------------
// Inicialização
// ---------------------------------------------------------------------------

bool posicao_reservada_portal(int linha, int coluna){
    return (linha == 12 && coluna == 23) ||  // portal da fase 1
           (linha == 11 && coluna == 26);    // portal da fase 2
}

bool pode_spawnar_item(int m[ALTURA][LARGURA], int linha, int coluna){
    return m[linha][coluna] == CAMINHO && !posicao_reservada_portal(linha, coluna);
}

// ======================================
// SPAWN BAÚS
// ======================================
void spawn_baus(int m[ALTURA][LARGURA], int quantidade){
    int colocados = 0;
    while(colocados < quantidade){
        int linha = rand() % 25;
        int coluna = rand() % 56;
        if(pode_spawnar_item(m, linha, coluna)){
            m[linha][coluna] = BAU;
            colocados++;
        }
    }
}

// ======================================
// SPAWN XPs
// ======================================
void spawn_xp(int m[ALTURA][LARGURA], int quantidade){
    int colocados = 0;
    while(colocados < quantidade){
        int linha = rand() % 25;
        int coluna = rand() % 56;
        if(pode_spawnar_item(m, linha, coluna)){
            m[linha][coluna] = XP;
            colocados++;
        }
    }
}

// ===============================
// SPAWN DOS MORTOS VIVOS
// ===============================
// Indice 5 = sala F (sala inicial do jogador) - nunca spawna inimigos
const int SALA_INICIAL = 5;

void spawn_mortos_vivos(int m[ALTURA][LARGURA], int inimigo_x[], int inimigo_y[], int inimigo_vivo[], int qtd_inimigos){
    int colocados = 0;

    // Controla quais salas ja tem um morto-vivo (maximo 1 por sala)
    bool sala_ocupada[MAX_SALAS] = {};

    // Monta lista de salas disponiveis (exceto sala inicial)
    int salas_disponiveis[MAX_SALAS];
    int qtd_disponiveis = 0;
    for(int s = 0; s < qtd_salas; s++){
        if(s != SALA_INICIAL) salas_disponiveis[qtd_disponiveis++] = s;
    }

    // Embaralha a lista para ordem aleatoria
    for(int s = qtd_disponiveis - 1; s > 0; s--){
        int r = rand() % (s + 1);
        int tmp = salas_disponiveis[s];
        salas_disponiveis[s] = salas_disponiveis[r];
        salas_disponiveis[r] = tmp;
    }

    int tentativas_sala = 0;
    while(colocados < qtd_inimigos && tentativas_sala < qtd_disponiveis){
        int num_sala = salas_disponiveis[tentativas_sala++];
        if(sala_ocupada[num_sala]) continue;

        Sala &s = salas[num_sala];

        // Tenta ate 20 posicoes dentro da sala
        bool colocou = false;
        for(int t = 0; t < 20 && !colocou; t++){
            int i = s.x1 + rand() % (s.x2 - s.x1 + 1);
            int j = s.y1 + rand() % (s.y2 - s.y1 + 1);
            if(m[i][j] == CAMINHO){
                m[i][j] = MORTO_VIVO;
                inimigo_x[colocados] = i;
                inimigo_y[colocados] = j;
                inimigo_vivo[colocados] = 1;
                // Define eixo de patrulha aleatorio (0=horizontal, 1=vertical)
                inimigo_eixo[colocados] = rand() % 2;
                inimigo_dir[colocados]  = (rand() % 2 == 0) ? 1 : -1;
                sala_ocupada[num_sala] = true;
                colocados++;
                colocou = true;
            }
        }
    }
}

void criar_sala_boss();
void revelar_sala_boss();
void spawn_cobras(int m[ALTURA][LARGURA], int cobra_x[], int cobra_y[], int cobra_viva[]);

// Ajusta os atributos iniciais do jogador e o número de inimigos conforme a dificuldade selecionada.
void configurar_dificuldade() {
    switch (dificuldade) {
        case DIFICULDADE_FACIL:
            jogador.vida = 100;
            jogador.vida_max = 100;
            jogador.chance_desvio = 55;
            max_inimigos_fase = 4;
            break;
        case DIFICULDADE_DIFICIL:
            jogador.vida = 100;
            jogador.vida_max = 100;
            jogador.chance_desvio = 25;
            max_inimigos_fase = 8;
            break;
        default:
            jogador.vida = 100;
            jogador.vida_max = 100;
            jogador.chance_desvio = 40;
            max_inimigos_fase = 6;
            break;
    }
}

void configurar_salas_fase1() {
    qtd_salas = 0;
    salas[qtd_salas++] = { 3,  0,  7,  6, false}; // A
    salas[qtd_salas++] = { 3, 14,  8, 23, false}; // B
    salas[qtd_salas++] = { 3, 33,  7, 43, false}; // C
    salas[qtd_salas++] = { 3, 49,  7, 55, false}; // D
    salas[qtd_salas++] = {10,  0, 17,  7, false}; // E
    salas[qtd_salas++] = { 9, 18, 15, 28, false}; // F — INICIAL (indice 5 = SALA_INICIAL)
    salas[qtd_salas++] = { 9, 36, 13, 43, false}; // G
    salas[qtd_salas++] = {10, 48, 17, 55, false}; // H
    salas[qtd_salas++] = {20,  0, 24, 13, false}; // I
    salas[qtd_salas++] = {19, 22, 24, 35, false}; // J
    salas[qtd_salas++] = {19, 44, 24, 55, false}; // K
    salas[qtd_salas++] = {11, 10, 15, 15, false}; // L
}

void configurar_salas_fase2() {
    qtd_salas = 0;
    salas[qtd_salas++] = { 3,  3,  7, 10, false}; // A
    salas[qtd_salas++] = { 3, 16,  7, 20, false}; // B
    salas[qtd_salas++] = { 3, 34,  7, 40, false}; // C
    salas[qtd_salas++] = { 7, 24, 13, 29, false}; // D — INICIAL (indice 3, SALA_INICIAL_FASE2)
    salas[qtd_salas++] = { 13, 24, 19, 29, false}; // E
    salas[qtd_salas++] = { 8, 49, 18, 55, false}; // F
    salas[qtd_salas++] = {15,  3, 19, 11, false}; // G
    salas[qtd_salas++] = {15, 13, 21, 21, false}; // H
    salas[qtd_salas++] = {15, 34, 22, 43, false}; // I
    salas[qtd_salas++] = {0, 44, 4, 49, false}; // F
}

void preparar_fase(int fase) {
    // Índice da sala onde o jogador nasce (usado em spawn_mortos_vivos para não spawnar lá)
    int sala_spawn_idx = SALA_INICIAL; // fase 1 padrão

    if (fase == 1) {
        for (int i = 0; i < ALTURA; ++i)
            for (int j = 0; j < LARGURA; ++j)
                mapa[i][j] = mapa_original[i][j];

        configurar_salas_fase1();
        spawn_baus(mapa, 6);
        spawn_xp(mapa, 20);
        jogador.pos = {12, 23}; // centro da sala F (indice 5)
        jogador.salas_descobertas = 0;
        jogador.xp = 0;
        jogador.chave = 0;
        jogador.espada = 0;
        jogador.escudo = 0;
        jogador.escudo_quebrado = 0;
        jogador.faca = 0;
        jogador.pocao_vida = 0;
        jogador.carne = 0;
        jogador.armadura = 0;
        jogador.trevo_sorte = 0;
        jogador.dano_bonus = 0;
        jogador.defesa_bonus = 0;
        jogador.visual_estado = 0;
        jogador.visual_turnos = 0;
        configurar_dificuldade();
        sala_spawn_idx = SALA_INICIAL; // indice 5 = sala F
    } else if (fase == 2) {
        for (int i = 0; i < ALTURA; ++i)
            for (int j = 0; j < LARGURA; ++j)
                mapa[i][j] = mapa_original_fase2[i][j];

        configurar_salas_fase2();
        spawn_baus(mapa, 4);
        spawn_xp(mapa, 12);
        jogador.pos = {10, 27}; // centro da sala D (indice 3), unico tile CAMINHO valido
        jogador.salas_descobertas = 0;
        jogador.visual_estado = 0;
        jogador.visual_turnos = 0;
        sala_spawn_idx = 3; // indice 3 = sala D (INICIAL da fase 2)
    } else {
        criar_sala_boss();

        jogador.pos = {19, 27};
        fase_atual = fase;
        boss_derrotado = false;
        resetar_visibilidade();
        revelar_sala_boss();

        for (int i = 0; i < 16; ++i) {
            inimigo_vivo[i] = 0;
            inimigo_vida[i] = 0;
        }
        for (int i = 0; i < 5; ++i) {
            cobra_viva[i] = 0;
            cobra_vida[i] = 0;
        }
        jogador.visual_estado = 0;
        jogador.visual_turnos = 0;
        return;
    }

    fase_atual = fase;
    sala_boss_criada = false;
    boss_derrotado = false;
    minerador_conversado = false;
    resetar_visibilidade();

    for (int i = 0; i < 16; ++i) {
        inimigo_vivo[i] = 0;
        inimigo_vida[i] = 0;
    }
    for (int i = 0; i < 5; ++i) {
        cobra_viva[i] = 0;
        cobra_vida[i] = 0;
    }

    {
        int colocados = 0;
        bool sala_ocupada[MAX_SALAS] = {};
        int salas_disponiveis[MAX_SALAS];
        int qtd_disponiveis = 0;
        for (int s = 0; s < qtd_salas; s++)
            if (s != sala_spawn_idx) salas_disponiveis[qtd_disponiveis++] = s;
        for (int s = qtd_disponiveis - 1; s > 0; s--) {
            int r = rand() % (s + 1);
            int tmp = salas_disponiveis[s]; salas_disponiveis[s] = salas_disponiveis[r]; salas_disponiveis[r] = tmp;
        }
        int tentativas_sala = 0;
        while (colocados < max_inimigos_fase && tentativas_sala < qtd_disponiveis) {
            int num_sala = salas_disponiveis[tentativas_sala++];
            if (sala_ocupada[num_sala]) continue;
            Sala &sl = salas[num_sala];
            bool colocou = false;
            for (int t = 0; t < 20 && !colocou; t++) {
                int i = sl.x1 + rand() % (sl.x2 - sl.x1 + 1);
                int j = sl.y1 + rand() % (sl.y2 - sl.y1 + 1);
                if (mapa[i][j] == CAMINHO) {
                    mapa[i][j] = MORTO_VIVO;
                    inimigo_x[colocados] = i; inimigo_y[colocados] = j; inimigo_vivo[colocados] = 1;
                    inimigo_eixo[colocados] = rand() % 2;
                    inimigo_dir[colocados]  = (rand() % 2 == 0) ? 1 : -1;
                    sala_ocupada[num_sala] = true; colocados++; colocou = true;
                }
            }
        }
    }

    int vida_inimigo_base = 24;
    int vida_cobra_base = 14;
    switch (dificuldade) {
        case DIFICULDADE_FACIL:
            vida_inimigo_base = 18;
            vida_cobra_base = 10;
            break;
        case DIFICULDADE_DIFICIL:
            vida_inimigo_base = 30;
            vida_cobra_base = 18;
            break;
    }
    for (int i = 0; i < max_inimigos_fase; ++i)
        if (inimigo_vivo[i] == 1) inimigo_vida[i] = vida_inimigo_base;

    spawn_cobras(mapa, cobra_x, cobra_y, cobra_viva);
    for (int i = 0; i < 5; ++i)
        if (cobra_viva[i] == 1) cobra_vida[i] = vida_cobra_base;

    int sala_inicial_idx = encontrar_sala(jogador.pos.x, jogador.pos.y);
    if (sala_inicial_idx != -1) revelar_sala(sala_inicial_idx);
}

// Cria o mapa inicial e prepara a primeira fase do jogo.
void criar_mapa() {
    preparar_fase(1);
}

// ---------------------------------------------------------------------------
// Renderização
// ---------------------------------------------------------------------------

// Desenha o mapa na tela, mostrando o jogador, os tiles visíveis e o HUD com status.
void exibir_mapa() {
    posicionar_cursor(0, 0);
    auto agora = chrono::steady_clock::now();
    int tempo_segundos = chrono::duration_cast<chrono::seconds>(
    agora - inicio_partida
    ).count();

    cout << "Vida: " << jogador.vida << "/" << jogador.vida_max
         << "  Salas: " << jogador.salas_descobertas << "/" << qtd_salas
         << "  XP: " << jogador.xp
         << "  Chaves: " << jogador.chave
         << "  Dano: +" << jogador.dano_bonus
         << "  Defesa: +" << jogador.defesa_bonus
         << "  Desvio: " << jogador.chance_desvio << "%"
         << "  Tempo: " << tempo_segundos << "s" << endl;

    if (jogador.visual_turnos > 0) {
        --jogador.visual_turnos;
        if (jogador.visual_turnos == 0) {
            jogador.visual_estado = 0;
        }
    }

    for (int i = 0; i < ALTURA; ++i) {
        for (int j = 0; j < LARGURA; ++j) {
            if (jogador.pos.x == i && jogador.pos.y == j) {
                switch (jogador.visual_estado) {
                    case 1:
                        printf("\033[92m"); cout << "■"; printf("\033[0m"); break;
                    case 2:
                        printf("\033[96m"); cout << "◙"; printf("\033[0m"); break;
                    case 3:
                        printf("\033[93m"); cout << "■"; printf("\033[0m"); break;
                    case 4:
                        printf("\033[95m"); cout << "■"; printf("\033[0m"); break;
                    case 5:
                        printf("\033[94m"); cout << "■"; printf("\033[0m"); break;
                    default:
                        cout << "■";
                        break;
                }
            } else if (!visivel[i][j]) {
                cout << " ";
            } else {
                switch (mapa[i][j]) {
                    case PAREDE:            cout << "█"; break;
                    case CAMINHO:           cout << " "; break;
                    case PORTA:             cout << "+"; break;
                    case CAMINHO_ESCONDIDO: cout << "#"; break;
                    case PORTAL:            printf("\033[94m"); cout << "O"; printf("\033[0m"); break;
                    case BAU:               cout << "B"; break;
                    case XP:                cout << "*"; break;
                    case NPC:               printf("\033[92m"); cout << "■"; printf("\033[0m"); break;
                    case MORTO_VIVO:  printf("\033[91m"); cout << "X"; printf("\033[0m"); break;
                    case COBRA:         printf("\033[93m"); cout << "~"; printf("\033[0m"); break;
                    case BOSS:          printf("\033[93m"); cout << "X"; printf("\033[0m"); break;
                    default:                cout << " "; break;
                }
            }
        }
        cout << '\n';
    }
}

// ---------------------------------------------------------------------------
// Movimentação
// ---------------------------------------------------------------------------

bool comprar_habilidade(int custo, Jogador &jogador){
    if(jogador.xp < custo){
        cout << "\nXP insuficiente. Você tem " << jogador.xp
             << " XP, mas precisa de " << custo << "." << endl;
        return false;
    }

    jogador.xp -= custo;
    return true;
}

void abrir_loja_habilidades(){
    limpar_tela();
    if (!minerador_conversado) {
        cout << "Minerador: Opa, parece que você está na mesma situação que eu." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Há quanto tempo estou aqui? Meses? Anos? Parei de contar faz tempo." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Tome cuidado. Este lugar é traiçoeiro e está cheio de perigos." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: E não espere que alguém venha te resgatar. Se existe uma saída, teremos que encontrá-la nós mesmos." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: O homem que caiu aqui antes de mim falava sobre um portal escondido na masmorra." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Ele dizia que a única forma de encontrá-lo era eliminar todos os perigos deste lugar." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Nunca soube se era verdade. Aquele sujeito parecia meio maluco." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Enfim... enquanto explora a masmorra, você vai ganhar experiência enfrentando os perigos deste lugar." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Traga essa experiência para mim. Anos preso aqui me ensinaram alguns truques de sobrevivência." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Posso ajudá-lo a ficar mais forte, mais resistente ou até mais rápido." << endl;
        system("pause");
        limpar_tela();
        cout << "Minerador: Não me pergunte como funciona. Depois de tanto tempo neste lugar, aprendi que algumas coisas simplesmente acontecem...\n" << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        minerador_conversado = true;
        limpar_tela();
    }

    cout << "MINERADOR:" << endl;
    cout << "Voce quer melhorar suas habilidades? (s ou n)" << endl;
    char loja = '\0';
    while (true) {
        cin >> loja;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            limpar_tela();
            cout << "Tecla invalida. Digite 's' para sim ou 'n' para nao." << endl;
            system("pause");
            limpar_tela();
            cout << "MINERADOR:" << endl;
            cout << "Voce quer melhorar suas habilidades? (s ou n)" << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (loja == 'n' || loja == 'N') {
            limpar_tela();
            cout << "Tchau" << endl;
            system("pause");
            limpar_tela();
            return;
        }

        if (loja == 's' || loja == 'S') {
            break;
        }

        limpar_tela();
        cout << "Tecla invalida. Digite 's' para sim ou 'n' para nao." << endl;
        system("pause");
        limpar_tela();
        cout << "MINERADOR:" << endl;
        cout << "Voce quer melhorar suas habilidades? (s ou n)" << endl;
    }

    limpar_tela();
    cout << "===== LOJA DE HABILIDADES =====" << endl;
    cout << "Seu XP: " << jogador.xp << endl;
    cout << "1 - Melhorar dano (+2)        Custo: 3 XP" << endl;
    cout << "2 - Melhorar defesa (+1)      Custo: 3 XP" << endl;
    cout << "3 - Melhorar desvio (+10%)    Custo: 4 XP" << endl;
    cout << "Escolha: ";

    while (true) {
        int resposta = 0;
        if (!(cin >> resposta)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            limpar_tela();
            cout << "Opcao invalida. Digite 1, 2 ou 3." << endl;
            system("pause");
            limpar_tela();
            cout << "===== LOJA DE HABILIDADES =====" << endl;
            cout << "Seu XP: " << jogador.xp << endl;
            cout << "1 - Melhorar dano (+2)        Custo: 3 XP" << endl;
            cout << "2 - Melhorar defesa (+1)      Custo: 3 XP" << endl;
            cout << "3 - Melhorar desvio (+10%)    Custo: 4 XP" << endl;
            cout << "Escolha: ";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (resposta) {
            case 1:
                if (comprar_habilidade(3, jogador)) {
                    jogador.dano_bonus += 2;
                    cout << "\nDano melhorado! Bonus atual: +" << jogador.dano_bonus << endl;
                }
                break;
            case 2:
                if (comprar_habilidade(3, jogador)) {
                    jogador.defesa_bonus += 1;
                    cout << "\nDefesa melhorada! Bonus atual: " << jogador.defesa_bonus << endl;
                }
                break;
            case 3:
                if (jogador.chance_desvio >= 90) {
                    cout << "\nSua chance de desvio ja esta no maximo." << endl;
                } else if (comprar_habilidade(4, jogador)) {
                    jogador.chance_desvio += 10;
                    if (jogador.chance_desvio > 90) jogador.chance_desvio = 90;
                    cout << "\nDesvio melhorado! Chance atual: " << jogador.chance_desvio << "%" << endl;
                }
                break;
            default:
                limpar_tela();
                cout << "Opcao invalida. Digite 1, 2 ou 3." << endl;
                system("pause");
                limpar_tela();
                cout << "===== LOJA DE HABILIDADES =====" << endl;
                cout << "Seu XP: " << jogador.xp << endl;
                cout << "1 - Melhorar dano (+2)        Custo: 3 XP" << endl;
                cout << "2 - Melhorar defesa (+1)      Custo: 3 XP" << endl;
                cout << "3 - Melhorar desvio (+10%)    Custo: 4 XP" << endl;
                cout << "Escolha: ";
                continue;
        }
        break;
    }

    system("pause");
    limpar_tela();
}

void criar_sala_boss();
void revelar_sala_boss();
void limpar_cobras(int m[ALTURA][LARGURA], int cobra_x[], int cobra_y[], int cobra_viva[]);
void salvar_ranking(const string &nome, int score, bool ranking_salvo_por_vitoria);
void preparar_fase(int fase);
bool usar_pocao_computador();
void spawn_cobras(int m[ALTURA][LARGURA], int cobra_x[], int cobra_y[], int cobra_viva[]);
void mostrar_inventario(const Jogador &jogador);
bool processar_uso_item(char resposta, Jogador &jogador);

void recuperar_vida(Jogador &jogador, int quantidade) {
    jogador.vida += quantidade;
    if (jogador.vida > jogador.vida_max) {
        jogador.vida = jogador.vida_max;
    }
}

// Usa um item de cura do inventário do jogador e aplica recuperação de vida.
bool consumir_item(Jogador *jogador, int &quantidade, int recuperacao) {
    if (jogador == nullptr || quantidade <= 0) {
        return false;
    }

    --quantidade;
    recuperar_vida(*jogador, recuperacao);
    return true;
}

void aplicar_efeito_armadura(Jogador &jogador) {
    jogador.vida_max += 20;
    recuperar_vida(jogador, 20);
}

void mostrar_inventario(const Jogador &jogador) {
    cout << "ITENS:" << '\n';
    cout << "A - " << jogador.escudo << " escudo(s)" << endl;
    cout << "B - " << jogador.espada << " espada(s)" << endl;
    cout << "C - " << jogador.faca << " faca(s)" << endl;
    cout << "D - " << jogador.pocao_vida << " poção(ões) de vida" << endl;
    cout << "E - " << jogador.carne << " carne(s)" << endl;
    cout << "F - " << jogador.armadura << " armadura(s)" << endl;
    cout << "G - " << jogador.trevo_sorte << " trevo(s) da sorte" << endl;
    cout << "\nDigite a letra do item para usar/consumir (ou outra tecla para sair)." << endl;
}

bool processar_uso_item(char resposta, Jogador &jogador) {
    if (resposta == 'd' || resposta == 'D') {
        if (consumir_item(&jogador, jogador.pocao_vida, 25)) {
            jogador.visual_estado = 1;
            jogador.visual_turnos = 6;
            cout << "\nVocê consumiu uma poção e recuperou 25 de vida." << endl;
            cout << "Vida atual: " << jogador.vida << "/" << jogador.vida_max << "." << endl;
        } else {
            cout << "\nVocê não possui poções de vida." << endl;
        }
        return true;
    }

    if (resposta == 'e' || resposta == 'E') {
        if (consumir_item(&jogador, jogador.carne, 15)) {
            jogador.visual_estado = 1;
            jogador.visual_turnos = 6;
            cout << "\nVocê consumiu carne e recuperou 15 de vida." << endl;
            cout << "Vida atual: " << jogador.vida << "/" << jogador.vida_max << "." << endl;
        } else {
            cout << "\nVocê não possui carne." << endl;
        }
        return true;
    }

    return false;
}

// Gera um item aleatório dentro do baú. As probabilidades são ajustadas pela dificuldade do jogo.
void conceder_item_bau(Jogador &jogador) {
    int chance_armadilha = probabilidade_por_dificuldade(6, 10, 14);
    int chance_xp = probabilidade_por_dificuldade(18, 20, 16);
    int chance_pocao = probabilidade_por_dificuldade(24, 20, 16);
    int chance_carne = probabilidade_por_dificuldade(18, 15, 12);
    int chance_faca = probabilidade_por_dificuldade(10, 10, 10);
    int chance_escudo = probabilidade_por_dificuldade(8, 8, 10);
    int chance_espada = probabilidade_por_dificuldade(6, 7, 10);
    int chance_armadura = probabilidade_por_dificuldade(4, 5, 8);
    int chance_trevo = probabilidade_por_dificuldade(6, 5, 4);

    int total = chance_armadilha + chance_xp + chance_pocao + chance_carne + chance_faca
              + chance_escudo + chance_espada + chance_armadura + chance_trevo;
    int r = rand() % total;

    if (r < chance_armadilha) {
        jogador.vida = (jogador.vida > 0) ? jogador.vida / 2 : 0;
        cout << "\nO bau era uma armadilha explosiva! Perdeu metade da vida." << endl;
    } else if (r < chance_armadilha + chance_xp) {
        int xp_ganho = 2 + rand() % 3;
        jogador.xp += xp_ganho;
        cout << "\nO bau continha " << xp_ganho << " XP!" << endl;
    } else if (r < chance_armadilha + chance_xp + chance_pocao) {
        jogador.pocao_vida++;
        cout << "\nVocê encontrou uma poção de vida!" << endl;
    } else if (r < chance_armadilha + chance_xp + chance_pocao + chance_carne) {
        jogador.carne++;
        cout << "\nVocê encontrou carne!" << endl;
    } else if (r < chance_armadilha + chance_xp + chance_pocao + chance_carne + chance_faca) {
        jogador.faca++;
        cout << "\nVocê encontrou uma faca!" << endl;
    } else if (r < chance_armadilha + chance_xp + chance_pocao + chance_carne + chance_faca + chance_escudo) {
        jogador.escudo++;
        jogador.escudo_quebrado = 0;
        cout << "\nVocê encontrou um escudo!" << endl;
    } else if (r < chance_armadilha + chance_xp + chance_pocao + chance_carne + chance_faca + chance_escudo + chance_espada) {
        jogador.espada++;
        cout << "\nVocê encontrou uma espada!" << endl;
    } else if (r < chance_armadilha + chance_xp + chance_pocao + chance_carne + chance_faca + chance_escudo + chance_espada + chance_armadura) {
        jogador.armadura++;
        aplicar_efeito_armadura(jogador);
        cout << "\nVocê encontrou uma armadura!" << endl;
    } else {
        jogador.trevo_sorte++;
        cout << "\nVocê encontrou um trevo da sorte!" << endl;
    }
}

// Move o jogador no mapa, interage com tiles especiais e abre baús ou portas quando necessário.
void mover_jogador(char direcao, int cobra_x[], int cobra_y[], int cobra_viva[]) {
    int nx = jogador.pos.x;
    int ny = jogador.pos.y;

    switch (direcao) {
        case 'w': case 'W': --nx; break;
        case 's': case 'S': ++nx; break;
        case 'a': case 'A': --ny; break;
        case 'd': case 'D': ++ny; break;
        case 'b': case 'B':{
        limpar_tela();
        mostrar_inventario(jogador);
        char resposta;
        cin >> resposta;
        processar_uso_item(resposta, jogador);
        system("pause");
        limpar_tela();
        return;
        }
        default: return;
    }

    if (nx < 0 || nx >= ALTURA || ny < 0 || ny >= LARGURA) return;

    int tile = mapa[nx][ny];
    if (tile == PAREDE || tile == VAZIO) return;
    if (tile == MORTO_VIVO || tile == COBRA || tile == BOSS) return;
    if (tile == NPC) return;
    if (tile == PORTAL) {
        limpar_tela();
        limpar_cobras(mapa, cobra_x, cobra_y, cobra_viva);
        if (fase_atual == 1) {
            preparar_fase(2);
        } else if (fase_atual == 2) {
            preparar_fase(3);
        }
        return;
    }
    if (tile == XP) {
        mapa[nx][ny] = CAMINHO;
        jogador.xp++;
    }

    if (tile == CAMINHO_ESCONDIDO) {
        int chance_pedra = probabilidade_por_dificuldade(0, 1, 2);
        int chance_buraco = probabilidade_por_dificuldade(0, 1, 2);

        if (chance_pedra > 0 && rand() % 100 < chance_pedra) {
            jogador.vida = jogador.vida - 20;
            cout<<'\n'<<"Uma pedra caiu na sua cabeça, você perdeu 20 pontos de vida."<<endl;
            if(computador_jogando) Sleep(2400);
            else {
                cout << "\nPressione Enter para continuar..." << endl;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            limpar_tela();
        }
        if (chance_buraco > 0 && rand() % 200 < chance_buraco) {
            cout<<'\n'<<"Você caiu num buraco fundo e morreu."<<endl;
            if(computador_jogando) Sleep(2400);
            else {
                cout << "\nPressione Enter para continuar..." << endl;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            limpar_tela();
            jogador.vida=0;
        }
    }

    if (tile == BAU) {
        if(jogador.chave < 1){
            cout << "\nO bau precisa de uma chave para abrir." << endl;
            if(computador_jogando) Sleep(1200);
            else system("pause");
            limpar_tela();
            return;
        }

        jogador.chave = jogador.chave - 1;
        int chance_dobro = probabilidade_por_dificuldade(30, 40, 50);
        if (jogador.trevo_sorte > 0 && rand() % 100 < chance_dobro) {
            conceder_item_bau(jogador);
            conceder_item_bau(jogador);
        } else {
            conceder_item_bau(jogador);
        }
        mapa[nx][ny] = CAMINHO;
        if(computador_jogando) Sleep(2400);
        else system("pause");
        limpar_tela();
    }

    jogador.pos.x = nx;
    jogador.pos.y = ny;

    revelar_tile_corredor(nx, ny);

    int sala = encontrar_sala(nx, ny);
    if (sala != -1) revelar_sala(sala);
}

// ===============================
// MOVIMENTO DOS INIMIGOS
// ===============================
void mover_inimigos(int m[ALTURA][LARGURA], int inimigo_x[], int inimigo_y[], int inimigo_vivo[], int qtd_inimigos, chrono::steady_clock::time_point &inicio, int jogador_x, int jogador_y){

    auto agora = chrono::steady_clock::now();
    auto tempo_inimigo = chrono::duration_cast<chrono::milliseconds>(agora - inicio).count();

    if(tempo_inimigo > 800){
        // Apaga posicoes antigas do mapa
        for(int i = 0; i < ALTURA; i++)
            for(int j = 0; j < LARGURA; j++)
                if(m[i][j] == MORTO_VIVO) m[i][j] = CAMINHO;

        for(int z = 0; z < qtd_inimigos; z++){
            if(inimigo_vivo[z] == 0) continue;

            int novox = inimigo_x[z];
            int novoy = inimigo_y[z];

            // Move na direcao atual do eixo de patrulha
            if(inimigo_eixo[z] == 0){
                // Horizontal: move na coluna
                novoy = inimigo_y[z] + inimigo_dir[z];
            } else {
                // Vertical: move na linha
                novox = inimigo_x[z] + inimigo_dir[z];
            }

            // Verifica se o proximo tile e caminho livre (sem parede, vazio ou outro inimigo)
            bool livre = (novox >= 0 && novox < ALTURA && novoy >= 0 && novoy < LARGURA)
                      && (m[novox][novoy] == CAMINHO);

            if(livre){
                // Verifica se nao esta ocupado por outro inimigo
                bool ocupado = false;
                for(int k = 0; k < qtd_inimigos; k++){
                    if(k != z && inimigo_vivo[k] == 1 &&
                       inimigo_x[k] == novox && inimigo_y[k] == novoy){
                        ocupado = true;
                    }
                }
                if(!ocupado){
                    inimigo_x[z] = novox;
                    inimigo_y[z] = novoy;
                }
            } else {
                // Bateu em obstáculo: inverte direcao
                inimigo_dir[z] = -inimigo_dir[z];
            }
        }

        // Reescreve inimigos no mapa
        for(int z = 0; z < qtd_inimigos; z++)
            if(inimigo_vivo[z] == 1) m[inimigo_x[z]][inimigo_y[z]] = MORTO_VIVO;

        inicio = chrono::steady_clock::now();

        // Verifica colisao com jogador
        for(int z = 0; z < qtd_inimigos; z++){
            if(inimigo_vivo[z] == 1 && inimigo_x[z] == jogador_x && inimigo_y[z] == jogador_y){
                aplicar_dano_inimigo(jogador.vida);
            }
        }
    }
}

// ===============================
// MOVIMENTO DO BOSS
// ===============================
void mover_boss(int m[ALTURA][LARGURA],
                chrono::steady_clock::time_point &inicio,
                int jogador_x,
                int jogador_y)
{
    if(!sala_boss_criada)
        return;

    if(boss_derrotado)
        return;

    if(boss_vida <= 0)
        return;

    auto agora = chrono::steady_clock::now();

    if(chrono::duration_cast<chrono::milliseconds>(agora - inicio).count() < 300)
        return;

    int novo_x = boss_x;
    int novo_y = boss_y;

    int dx = jogador_x - boss_x;
    int dy = jogador_y - boss_y;

    // tenta andar no eixo com maior distância
    if(abs(dx) >= abs(dy))
    {
        novo_x += (dx > 0) ? 1 : -1;
    }
    else
    {
        novo_y += (dy > 0) ? 1 : -1;
    }

    // Se encontrou parede tenta o outro eixo
    if(m[novo_x][novo_y] != CAMINHO)
    {
        novo_x = boss_x;
        novo_y = boss_y;

        if(abs(dx) >= abs(dy))
            novo_y += (dy > 0) ? 1 : -1;
        else
            novo_x += (dx > 0) ? 1 : -1;
    }

    if(m[novo_x][novo_y] == CAMINHO)
    {
        m[boss_x][boss_y] = CAMINHO;

        boss_x = novo_x;
        boss_y = novo_y;

        m[boss_x][boss_y] = BOSS;
    }

    // dano ao jogador
    if(boss_x == jogador.pos.x &&
       boss_y == jogador.pos.y)
    {
        aplicar_dano_inimigo(jogador.vida);
    }

    inicio = chrono::steady_clock::now();
}


// ===============================
// SPAWN DAS COBRAS
// ===============================

void spawn_cobras(int m[ALTURA][LARGURA], int cobra_x[], int cobra_y[], int cobra_viva[]){
    int chance_aparecer = probabilidade_por_dificuldade(25, 50, 70);
    int qtd_cobras = 0;
    for(int c = 0; c < 5; c++){
        if(rand() % 100 < chance_aparecer) qtd_cobras++;
    }

    int colocados = 0;
    int tentativas = 0;
    while(colocados < qtd_cobras && tentativas < 10000){
        tentativas++;
        int i = rand() % ALTURA;
        int j = rand() % LARGURA;
        if(m[i][j] == CAMINHO_ESCONDIDO){
            m[i][j] = COBRA;
            cobra_x[colocados] = i;
            cobra_y[colocados] = j;
            cobra_viva[colocados] = 1;
            colocados++;
        }
    }
    // Marca as cobras que nao apareceram como mortas
    for(int c = colocados; c < 5; c++)
        cobra_viva[c] = 0;
}

// ===============================
// MOVIMENTO DAS COBRAS (perseguicao)
// ===============================
void mover_cobras(int m[ALTURA][LARGURA], int cobra_x[], int cobra_y[], int cobra_viva[], chrono::steady_clock::time_point &inicio_cobra, int jogador_x, int jogador_y){

    auto agora = chrono::steady_clock::now();
    auto delta = chrono::duration_cast<chrono::milliseconds>(agora - inicio_cobra).count();

    if(delta < 800) return;

    // Apaga cobras do mapa
    for(int i = 0; i < ALTURA; i++)
        for(int j = 0; j < LARGURA; j++)
            if(m[i][j] == COBRA) m[i][j] = CAMINHO_ESCONDIDO;

    for(int z = 0; z < 5; z++){
        if(cobra_viva[z] == 0) continue;

        int novox = cobra_x[z];
        int novoy = cobra_y[z];

        // Perseguicao: move no eixo de maior distancia
        int dx = jogador_x - cobra_x[z];
        int dy = jogador_y - cobra_y[z];

        if(abs(dx) >= abs(dy)){
            novox = cobra_x[z] + (dx > 0 ? 1 : -1);
        } else {
            novoy = cobra_y[z] + (dy > 0 ? 1 : -1);
        }

        // Cobra pode andar em CAMINHO_ESCONDIDO
        int tile = (novox >= 0 && novox < ALTURA && novoy >= 0 && novoy < LARGURA)
                   ? m[novox][novoy] : PAREDE;

        if(tile == CAMINHO_ESCONDIDO){
            // Verifica colisao com outra cobra
            bool ocupado = false;
            for(int k = 0; k < 5; k++){
                if(k != z && cobra_viva[k] == 1 &&
                   cobra_x[k] == novox && cobra_y[k] == novoy){
                    ocupado = true;
                }
            }
            if(!ocupado){
                cobra_x[z] = novox;
                cobra_y[z] = novoy;
            }
        } else {
            // Bloqueada no eixo principal: tenta o eixo secundario
            novox = cobra_x[z];
            novoy = cobra_y[z];
            if(abs(dx) >= abs(dy)){
                novoy = cobra_y[z] + (dy > 0 ? 1 : -1);
            } else {
                novox = cobra_x[z] + (dx > 0 ? 1 : -1);
            }
            tile = (novox >= 0 && novox < ALTURA && novoy >= 0 && novoy < LARGURA)
                   ? m[novox][novoy] : PAREDE;
            if(tile == CAMINHO_ESCONDIDO){
                cobra_x[z] = novox;
                cobra_y[z] = novoy;
            }
        }
    }

    // Reescreve cobras no mapa
    for(int z = 0; z < 5; z++)
        if(cobra_viva[z] == 1) m[cobra_x[z]][cobra_y[z]] = COBRA;

    inicio_cobra = chrono::steady_clock::now();
}

void limpar_cobras(int m[ALTURA][LARGURA], int cobra_x[], int cobra_y[], int cobra_viva[]) {
    for (int z = 0; z < 5; z++) {
        if (cobra_viva[z] == 1) {
            if (m[cobra_x[z]][cobra_y[z]] == COBRA) {
                m[cobra_x[z]][cobra_y[z]] = CAMINHO_ESCONDIDO;
            }
            cobra_viva[z] = 0;
        }
    }
}

// ===============================
// COMBATE EM TURNOS
// ===============================

int dano_jogador(){
    int dano_base = 4;
    if (jogador.faca > 0) dano_base = 7;
    if (jogador.espada > 0) dano_base = 12;
    int dano = dano_base + jogador.dano_bonus - (jogador.armadura * 2);
    if (dano < 1) dano = 1;
    return dano;
}

const char* nome_arma_jogador(){
    if (jogador.espada > 0) return "Espada";
    if (jogador.faca > 0) return "Faca";
    return "Punhos";
}

bool encontrar_inimigo_para_luta(int inimigo_x[], int inimigo_y[], int inimigo_vivo[], int qtd_inimigos, int cobra_x[], int cobra_y[], int cobra_viva[], int &tipo, int &indice){
    const int dx[5] = {0, -1, 1, 0, 0};
    const int dy[5] = {0, 0, 0, -1, 1};

    for(int d = 0; d < 5; d++){
        int x = jogador.pos.x + dx[d];
        int y = jogador.pos.y + dy[d];

        if (mapa[x][y] == BOSS) {
            tipo = BOSS;
            indice = 0;
            return true;
        }

        for(int z = 0; z < qtd_inimigos; z++){
            if(inimigo_vivo[z] == 1 && inimigo_x[z] == x && inimigo_y[z] == y){
                tipo = MORTO_VIVO;
                indice = z;
                return true;
            }
        }

        for(int z = 0; z < 5; z++){
            if(cobra_viva[z] == 1 && cobra_x[z] == x && cobra_y[z] == y){
                tipo = COBRA;
                indice = z;
                return true;
            }
        }
    }

    return false;
}

bool encontrar_npc_para_loja(int &x, int &y) {
    const int dx[5] = {0, -1, 1, 0, 0};
    const int dy[5] = {0, 0, 0, -1, 1};

    for (int d = 0; d < 5; d++) {
        int nx = jogador.pos.x + dx[d];
        int ny = jogador.pos.y + dy[d];
        if (nx >= 0 && nx < ALTURA && ny >= 0 && ny < LARGURA && mapa[nx][ny] == NPC) {
            x = nx;
            y = ny;
            return true;
        }
    }
    return false;
}

int dano_inimigo_luta(int tipo){
    return (tipo == COBRA) ? 5 : 8;
}

void spawn_portal_sala_inicial() {
    // Posição inicial depende da fase atual
    int px, py, r1, c1, r2, c2;
    if (fase_atual == 1) {
        px = 12; py = 23;       // centro sala F fase 1
        r1 =  9; c1 = 18; r2 = 15; c2 = 28;
    } else {
        px = 11; py = 26;       // centro sala D fase 2
        r1 =  8; c1 = 24; r2 = 14; c2 = 29;
    }

    if (mapa[px][py] == CAMINHO || mapa[px][py] == BAU || mapa[px][py] == XP) {
        mapa[px][py] = PORTAL;
        return;
    }
    for (int i = r1; i <= r2; ++i) {
        for (int j = c1; j <= c2; ++j) {
            if (mapa[i][j] == CAMINHO) {
                mapa[i][j] = PORTAL;
                return;
            }
        }
    }
    for (int i = r1; i <= r2; ++i) {
        for (int j = c1; j <= c2; ++j) {
            if (mapa[i][j] == BAU || mapa[i][j] == XP) {
                mapa[i][j] = PORTAL;
                return;
            }
        }
    }
}

void criar_sala_boss() {
    if (sala_boss_criada) return;

    for (int i = 0; i < ALTURA; ++i)
        for (int j = 0; j < LARGURA; ++j)
            mapa[i][j] = mapa_boss[i][j];

    boss_x = 12;
    boss_y = 27;

    mapa[boss_x][boss_y] = BOSS;
    boss_vida = 50;
    sala_boss_criada = true;
}

void revelar_sala_boss() {
    for (int i = 0; i < ALTURA; ++i) {
        for (int j = 0; j < LARGURA; ++j) {
            visivel[i][j] = true;
        }
    }
}

void ganhar_chave_inimigo(){
    if(jogador.chave < 6){
        jogador.chave++;
        cout << "\nVoce obteve uma chave. Chaves: " << jogador.chave <<  endl;
    }
}

void mostrar_luta(const char* nome_inimigo, int vida_inimigo){
    limpar_tela();
    cout << "===== LUTA =====" << endl;
    cout << "Jogador: " << jogador.vida << "/" << jogador.vida_max
         << "  Arma: " << nome_arma_jogador() << endl;
    cout << nome_inimigo << ": " << vida_inimigo << endl;
    cout << "\n1 - Atacar" << endl;
    cout << "2 - Desviar" << endl;
    cout << "3 - Defender" << endl;
    cout << "Escolha: ";
}

void turno_inimigo_luta(int tipo, int acao_jogador){
    int dano = dano_inimigo_luta(tipo);

    if(acao_jogador == 2){
        if(rand() % 100 < jogador.chance_desvio){
            cout << "\nVocê desviou do ataque." << endl;
            return;
        }
        cout << "\nVocê tentou desviar, mas falhou." << endl;
    } else if(acao_jogador == 3){
        int chance_bloqueio = probabilidade_por_dificuldade(60, 50, 40);
        int chance_quebra = probabilidade_por_dificuldade(25, 35, 45);
        bool bloqueou_tudo = jogador.escudo > 0 && !jogador.escudo_quebrado && (rand() % 100 < chance_bloqueio);
        if(bloqueou_tudo){
            cout << "\nSeu escudo bloqueou todo o dano." << endl;
            if (rand() % 100 < chance_quebra) {
                jogador.escudo = 0;
                jogador.escudo_quebrado = 1;
                cout << "Seu escudo foi destruído!" << endl;
            }
            return;
        }
        dano = (dano + 1) / 2;
        cout << "\nVocê defendeu e reduziu o dano." << endl;
    }

    dano -= jogador.defesa_bonus;
    if(dano < 0) dano = 0;

    jogador.vida -= dano;
    cout << "\nO inimigo causou " << dano << " de dano." << endl;
}

// Inicia uma luta contra um inimigo ou boss, usando ponteiros para referenciar a vida do alvo correto.
void iniciar_luta(int tipo, int indice, int inimigo_x[], int inimigo_y[], int inimigo_vivo[], int inimigo_vida[], int cobra_x[], int cobra_y[], int cobra_viva[], int cobra_vida[]){
    int *vida_alvo;
    const char* nome_inimigo;
    if (tipo == COBRA) {
        vida_alvo = &cobra_vida[indice];
        nome_inimigo = "Cobra";
    } else if (tipo == BOSS) {
        vida_alvo = &boss_vida;
        nome_inimigo = "Boss";
    } else {
        vida_alvo = &inimigo_vida[indice];
        nome_inimigo = "Morto-vivo";
    }

    if(*vida_alvo <= 0){
        if (tipo == COBRA) *vida_alvo = 14;
        else if (tipo == BOSS) *vida_alvo = 50;
        else *vida_alvo = 24;
    }

    while(jogador.vida > 0 && *vida_alvo > 0){
        mostrar_luta(nome_inimigo, *vida_alvo);

        int acao = 0;
        if(computador_jogando){
            usar_pocao_computador();
            acao = 1;
            int dano = dano_jogador();
            *vida_alvo -= dano;
            cout << "\nComputador atacou e causou " << dano << " de dano." << endl;
        } else {
            char escolha = _getch();

            if(escolha == '1' || escolha == 'a' || escolha == 'A'){
                acao = 1;
                int dano = dano_jogador();
                *vida_alvo -= dano;
                cout << "\nVocê atacou e causou " << dano << " de dano." << endl;
            } else if(escolha == '2' || escolha == 'd' || escolha == 'D'){
                acao = 2;
            } else if(escolha == '3' || escolha == 'f' || escolha == 'F'){
                acao = 3;
            } else {
                cout << "\nEscolha invalida." << endl;
                Sleep(computador_jogando ? 1800 : 1500);
                continue;
            }
        }

        if(*vida_alvo <= 0){
            cout << "\nVocê derrotou o " << nome_inimigo << "!" << endl;
            jogador.xp += (tipo == COBRA) ? 1 : 2;
            ganhar_chave_inimigo();

            if(tipo == COBRA){
                cobra_viva[indice] = 0;
                mapa[cobra_x[indice]][cobra_y[indice]] = CAMINHO_ESCONDIDO;
            } else if (tipo == BOSS) {
                mapa[boss_x][boss_y] = CAMINHO;
                boss_derrotado = true;
                auto fim_partida = chrono::steady_clock::now();
                int tempo_segundos = static_cast<int>(chrono::duration_cast<chrono::seconds>(fim_partida - inicio_partida).count());
                limpar_tela();
                cout << "\nParabens, você venceu e conseguiu escapar!" << endl;
                cout << "Tempo da run: " << tempo_segundos << " segundos." << endl;
                if (!jogador_nome.empty()) {
                    ranking_salvo_por_vitoria = true;
                    salvar_ranking(jogador_nome, tempo_segundos, ranking_salvo_por_vitoria);

                }
                if(!computador_jogando){
                    cout << "\nPressione Enter para voltar ao menu..." << endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                }
            } else {
                inimigo_vivo[indice] = 0;
                mapa[inimigo_x[indice]][inimigo_y[indice]] = CAMINHO;
                bool existe_morto_vivo = false;
                for(int z = 0; z < max_inimigos_fase; z++){
                    if(inimigo_vivo[z] == 1){
                        existe_morto_vivo = true;
                        break;
                    }
                }
                if(!existe_morto_vivo){
                    spawn_portal_sala_inicial();
                }
            }

            Sleep(computador_jogando ? 1800 : 1500);
            limpar_tela();
            return;
        }

        turno_inimigo_luta(tipo, acao);
        Sleep(computador_jogando ? 1800 : 1500);
    }

    limpar_tela();
}

// ---------------------------------------------------------------------------
// Ranking
// ---------------------------------------------------------------------------

string obter_timestamp() {
    time_t agora = time(nullptr);
    struct tm local_tm;
    tm* temp = localtime(&agora);
    if (temp != nullptr)
    local_tm = *temp;
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &local_tm);
    return string(buffer);
}

// Salva o ranking em arquivo usando nome, tempo e se a partida terminou em vitória.
void salvar_ranking(const string &nome, int score, bool ranking_salvo_por_vitoria) {
    ofstream arquivo("ranking.txt", ios::app);
    if (arquivo.is_open())
        arquivo << nome << " " << score << " " << obter_timestamp() << " " << ranking_salvo_por_vitoria << "\n";
}

// Exibe o ranking ordenado. Vitórias aparecem primeiro e, entre vitórias, menor tempo vence.
void mostrar_ranking() {
    ifstream arquivo("ranking.txt");
    if (!arquivo.is_open()) {
        cout << "Ranking vazio ainda." << endl;
        return;
    }

    string nomes[MAX_RANKING];
    int    scores[MAX_RANKING];
    string datas[MAX_RANKING];
    int    total = 0;
    string linha;
    bool vitoria[MAX_RANKING];

    while (total < MAX_RANKING && getline(arquivo, linha)) {
    if (linha.empty()) continue;

    // Formato: "nome com espaços score dd-mm-yyyy hh:mm:ss vitoria"
    // Divide pelo último espaço em espaços: pega os 4 últimos tokens pelo final
    istringstream iss(linha);
    vector<string> tokens;
    string token;

    while (iss >> token) tokens.push_back(token);

        // Precisa de pelo menos 4 campos: score data hora vitoria
        if (tokens.size() < 4) continue;

        int n = tokens.size();
        string vit_str  = tokens[n-1];
        string hora_str = tokens[n-2];
        string data_str = tokens[n-3];
        string score_str = tokens[n-4];

        int score = 0;
        try { score = stoi(score_str); }
        catch (...) { continue; }

        // Nome é tudo antes do score
        string nome = "";
        for (int k = 0; k < n-4; ++k) {
            if (k > 0) nome += " ";
            nome += tokens[k];
        }

        int vit = (vit_str == "1") ? 1 : 0;

        nomes[total]   = nome;
        scores[total]  = score;
        datas[total]   = data_str + " " + hora_str;
        vitoria[total] = (bool)vit;
        ++total;
    }

    if (total == 0) {
        cout << "Ranking vazio." << endl;
        return;
    }

    // Vitórias aparecem antes das derrotas; entre iguais, menor tempo = melhor
    for (int i = 0; i < total - 1; ++i){
        for (int j = i + 1; j < total; ++j) {
            bool troca = false;
            if (vitoria[j] && !vitoria[i]) {
            // j venceu, i perdeu → j sobe
                troca = true;
            } else if (vitoria[i] == vitoria[j]) {
                if (vitoria[i]) {
                    // Ambos venceram → menor tempo primeiro
                    if (scores[j] < scores[i]) troca = true;
                } else {
                    // Ambos perderam → MAIOR tempo primeiro
                    if (scores[j] > scores[i]) troca = true;
                }
            }

            if (troca) {
                swap(nomes[i],   nomes[j]);
                swap(scores[i],  scores[j]);
                swap(datas[i],   datas[j]);
                swap(vitoria[i], vitoria[j]);
            }
            }
        }



    cout << "===== RANKING =====" << endl;
    for (int i = 0; i < total; ++i) {
        cout << endl << i + 1 << " - " << nomes[i]
             << " -> " << scores[i] << " seg"
             << (datas[i].empty() ? "" : " (" + datas[i] + ")")
             << " [" << (vitoria[i] ? "VITORIA" : "DERROTA") << "]";
    }
}

// ---------------------------------------------------------------------------
// Menus auxiliares
// ---------------------------------------------------------------------------

void mostrar_regras() {
    limpar_tela();
    cout << "===== SOBRE / REGRAS =====" << endl;
    cout << "Use WASD ou as setas do teclado para se mover." << endl;
    cout << "Explore o mapa descobrindo todas as salas (total: 12)." << endl;
    cout << "Você pode batalhar com inimigos (cobras e mortos vivos) apertando 'q' perto deles." << endl;
    cout << "Fale com o Minerador apertando 'r' quando estiver ao lado dele e gaste xp na loja para melhorar suas habilidades." << endl;
    cout << "Inspecione o seu inventário apertando a tecla 'b'." << endl;
    cout << "Abra baús com chaves dropadas por inimigos." << endl;
    cout << "Use 'P' para pausar o jogo a qualquer momento." << endl;
    cout << "Itens consumíveis mudam o visual do personagem temporariamente durante o jogo." << endl;
    cout << "A dificuldade afeta o número de inimigos, a vida deles, as chances dos baús e a força das defesas." << endl;
    cout << "A pontuação final é baseada no tempo de duração da run e se você venceu ou não." << endl;
    cout << "\n====== ITENS ==========" << endl;
    cout << "Faca = Possível coletar dentro dos Baús e causa 7 de dano base." << endl;
    cout << "Espada = Possível coletar dentro dos Baús e causa 12 de dano base." << endl;
    cout << "Escudo = Possível coletar dentro dos Baús e defende todos os ataques, mas pode quebrar." << endl;
    cout << "Poção = Possível coletar dentro dos Baús e recupera 25 de vida." << endl;
    cout << "Carne = Cura 15 de vida e some após usar." << endl;
    cout << "Armadura = Aumenta a vida máxima em 20 e reduz 2 de dano." << endl;
    cout << "Trevo da Sorte = Dá 40% de chance de ganhar dois itens ao abrir um baú." << endl;
    cout << "\nPressione Enter para voltar ao menu..." << endl;
    cin.get();
}

void mostrar_historia_inicial() {
    limpar_tela();
    cout << "Você acorda com uma forte dor de cabeça. Ao olhar em volta, percebe que está preso no fundo de um enorme buraco."<< endl;
    cout<< "Sua memória está confusa e você não consegue se lembrar de como foi parar ali."<<endl;
    cout<< "Desesperado, você grita por ajuda. Após alguns instantes, um velho minerador aparece vindo da escuridão."<<endl;
    cout<< "Talvez ele saiba o que aconteceu... ou como sair deste lugar. Converse com ele e descubra uma forma de escapar." << endl;
    cout << "\nPressione Enter para continuar..." << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}


// ---------------------------------------------------------------------------
// Modo computador
// ---------------------------------------------------------------------------

bool tile_andavel_computador(int tile) {
    if(tile == BAU) return jogador.chave > 0;
    return tile == CAMINHO || tile == CAMINHO_ESCONDIDO || tile == PORTA || tile == XP || tile == PORTAL || tile == NPC || tile == BOSS;
}

int distancia_computador(Pos alvo) {
    if(alvo.x < 0 || alvo.x >= ALTURA || alvo.y < 0 || alvo.y >= LARGURA) return -1;

    bool visitado[ALTURA][LARGURA] = {};
    int distancia[ALTURA][LARGURA] = {};
    Pos fila[ALTURA * LARGURA];
    int ini = 0, fim = 0;

    fila[fim++] = jogador.pos;
    visitado[jogador.pos.x][jogador.pos.y] = true;

    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};

    while(ini < fim){
        Pos atual = fila[ini++];
        if(atual.x == alvo.x && atual.y == alvo.y) return distancia[atual.x][atual.y];

        for(int d = 0; d < 4; d++){
            int nx = atual.x + dx[d];
            int ny = atual.y + dy[d];
            if(nx < 0 || nx >= ALTURA || ny < 0 || ny >= LARGURA) continue;
            if(visitado[nx][ny]) continue;
            if(!tile_andavel_computador(mapa[nx][ny])) continue;

            visitado[nx][ny] = true;
            distancia[nx][ny] = distancia[atual.x][atual.y] + 1;
            fila[fim++] = {nx, ny};
        }
    }

    return -1;
}

bool procurar_alvo_computador(int tipo_alvo, Pos &alvo) {
    bool achou = false;
    int melhor_dist = 100000;

    for(int i = 0; i < ALTURA; i++){
        for(int j = 0; j < LARGURA; j++){
            if(mapa[i][j] != tipo_alvo) continue;
            int dist = distancia_computador({i, j});
            if(dist >= 0 && dist < melhor_dist){
                melhor_dist = dist;
                alvo = {i, j};
                achou = true;
            }
        }
    }

    return achou;
}

bool procurar_tile_computador(int tipo_alvo, Pos &alvo) {
    bool achou = false;
    int melhor_dist = 100000;

    for(int i = 0; i < ALTURA; i++){
        for(int j = 0; j < LARGURA; j++){
            if(mapa[i][j] != tipo_alvo) continue;
            int dist = abs(jogador.pos.x - i) + abs(jogador.pos.y - j);
            if(dist < melhor_dist){
                melhor_dist = dist;
                alvo = {i, j};
                achou = true;
            }
        }
    }

    return achou;
}

bool procurar_posicao_perto_computador(int x, int y, Pos &alvo) {
    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};
    bool achou = false;
    int melhor_dist = 100000;

    for(int d = 0; d < 4; d++){
        int nx = x + dx[d];
        int ny = y + dy[d];
        if(nx < 0 || nx >= ALTURA || ny < 0 || ny >= LARGURA) continue;
        if(!tile_andavel_computador(mapa[nx][ny])) continue;
        int dist = distancia_computador({nx, ny});
        if(dist >= 0 && dist < melhor_dist){
            melhor_dist = dist;
            alvo = {nx, ny};
            achou = true;
        }
    }

    return achou;
}

bool escolher_alvo_computador(Pos &alvo) {
    Pos npc;
    if(jogador.xp >= 3 && !minerador_conversado && procurar_tile_computador(NPC, npc) && procurar_posicao_perto_computador(npc.x, npc.y, alvo)) return true;

    if(jogador.chave > 0 && procurar_alvo_computador(BAU, alvo)) return true;

    for(int z = 0; z < max_inimigos_fase; z++){
        if(inimigo_vivo[z] == 1 && procurar_posicao_perto_computador(inimigo_x[z], inimigo_y[z], alvo)) return true;
    }

    for(int z = 0; z < 5; z++){
        if(cobra_viva[z] == 1 && procurar_posicao_perto_computador(cobra_x[z], cobra_y[z], alvo)) return true;
    }

    if(fase_atual == 3 && !boss_derrotado){
        if(procurar_posicao_perto_computador(boss_x, boss_y, alvo)) return true;
    }

    if(procurar_alvo_computador(PORTAL, alvo)) return true;

    if(procurar_alvo_computador(XP, alvo)) return true;
    return false;
}

char passo_para_alvo_computador(Pos alvo) {
    bool visitado[ALTURA][LARGURA] = {};
    int pai_x[ALTURA][LARGURA];
    int pai_y[ALTURA][LARGURA];
    Pos fila[ALTURA * LARGURA];
    int ini = 0, fim = 0;

    for(int i = 0; i < ALTURA; i++){
        for(int j = 0; j < LARGURA; j++){
            pai_x[i][j] = -1;
            pai_y[i][j] = -1;
        }
    }

    fila[fim++] = jogador.pos;
    visitado[jogador.pos.x][jogador.pos.y] = true;

    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};
    const char dir[4] = {'w', 's', 'a', 'd'};

    while(ini < fim){
        Pos atual = fila[ini++];
        if(atual.x == alvo.x && atual.y == alvo.y) break;

        for(int d = 0; d < 4; d++){
            int nx = atual.x + dx[d];
            int ny = atual.y + dy[d];
            if(nx < 0 || nx >= ALTURA || ny < 0 || ny >= LARGURA) continue;
            if(visitado[nx][ny]) continue;
            if(!tile_andavel_computador(mapa[nx][ny])) continue;

            visitado[nx][ny] = true;
            pai_x[nx][ny] = atual.x;
            pai_y[nx][ny] = atual.y;
            fila[fim++] = {nx, ny};
        }
    }

    if(!visitado[alvo.x][alvo.y]) return 0;

    Pos passo = alvo;
    while(!(pai_x[passo.x][passo.y] == jogador.pos.x && pai_y[passo.x][passo.y] == jogador.pos.y)){
        int px = pai_x[passo.x][passo.y];
        int py = pai_y[passo.x][passo.y];
        if(px == -1 || py == -1) return 0;
        passo = {px, py};
    }

    for(int d = 0; d < 4; d++){
        if(jogador.pos.x + dx[d] == passo.x && jogador.pos.y + dy[d] == passo.y) return dir[d];
    }

    return 0;
}

bool usar_pocao_computador() {
    if(jogador.pocao_vida <= 0) return false;
    if(jogador.vida > jogador.vida_max / 2) return false;

    jogador.pocao_vida--;
    recuperar_vida(jogador, 25);

    limpar_tela();
    cout << "Computador usou uma pocao de vida." << endl;
    cout << "Vida atual: " << jogador.vida << "/" << jogador.vida_max
         << " | Pocoes restantes: " << jogador.pocao_vida << endl;
    Sleep(2200);
    limpar_tela();
    return true;
}

void comprar_loja_computador() {
    limpar_tela();
    cout << "Computador conversou com o Minerador e gastou XP na loja." << endl;
    minerador_conversado = true;

    while(jogador.xp >= 3 && jogador.dano_bonus < 10){
        jogador.xp -= 3;
        jogador.dano_bonus += 2;
        cout << "Dano melhorado! Bonus atual: +" << jogador.dano_bonus << endl;
    }
    while(jogador.xp >= 3 && jogador.defesa_bonus < 4){
        jogador.xp -= 3;
        jogador.defesa_bonus += 1;
        cout << "Defesa melhorada! Bonus atual: +" << jogador.defesa_bonus << endl;
    }
    while(jogador.xp >= 4 && jogador.chance_desvio < 70){
        jogador.xp -= 4;
        jogador.chance_desvio += 10;
        cout << "Desvio melhorado! Chance atual: " << jogador.chance_desvio << "%" << endl;
    }

    Sleep(2200);
    limpar_tela();
}

char escolher_acao_computador() {
    usar_pocao_computador();

    int npc_x = 0, npc_y = 0;
    if(jogador.xp >= 3 && !minerador_conversado && encontrar_npc_para_loja(npc_x, npc_y)){
        return 'r';
    }

    int tipo_inimigo = 0;
    int indice_inimigo = -1;
    if(encontrar_inimigo_para_luta(inimigo_x, inimigo_y, inimigo_vivo, max_inimigos_fase, cobra_x, cobra_y, cobra_viva, tipo_inimigo, indice_inimigo)){
        return 'q';
    }

    Pos alvo;
    if(escolher_alvo_computador(alvo)){
        return passo_para_alvo_computador(alvo);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Loop principal do jogo
// ---------------------------------------------------------------------------

int selecionar_modo_partida() {
    int opcao = 0;
    while (opcao < 1 || opcao > 2) {
        limpar_tela();
        cout << "===== MODO DE JOGO =====" << endl;
        cout << "1 - Jogar sozinho" << endl;
        cout << "2 - Deixar o computador jogar" << endl;
        cout << "Escolha: ";

        if (!(cin >> opcao)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcao = 0;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    return opcao;
}

int selecionar_dificuldade() {
    int opcao = 0;
    while (opcao < 1 || opcao > 3) {
        limpar_tela();
        cout << "===== DIFICULDADE =====" << endl;
        cout << "1 - Facil" << endl;
        cout << "2 - Medio" << endl;
        cout << "3 - Dificil" << endl;
        cout << "Escolha: ";

        if (!(cin >> opcao)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcao = 0;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    return opcao;
}

void jogar(bool auto_computador = false) {
    // Reseta estado global de partida anterior
    fase_atual = 1;
    sala_boss_criada = false;
    boss_derrotado = false;
    ranking_salvo_por_vitoria = false;
    minerador_conversado = false;
    computador_jogando = auto_computador;

    criar_mapa(); // chama preparar_fase(1) — já reseta visibilidade e spawns

    auto inicio = chrono::steady_clock::now();
    inicio_partida = inicio;

    auto inicio_cobra = chrono::steady_clock::now();
    auto inicio_boss = chrono::steady_clock::now();
    bool jogador_ja_moveu = false;

    limpar_tela();


    while (jogador.vida > 0) {
        exibir_mapa();
        mover_inimigos(mapa,inimigo_x,inimigo_y,inimigo_vivo,max_inimigos_fase, inicio,jogador.pos.x, jogador.pos.y);
        mover_boss(mapa, inicio_boss, jogador.pos.x, jogador.pos.y);
        if(jogador_ja_moveu){
            mover_cobras(mapa, cobra_x, cobra_y, cobra_viva, inicio_cobra, jogador.pos.x, jogador.pos.y);
        } else {
            inicio_cobra = chrono::steady_clock::now();
        }

        // Dano das cobras ao jogador
        for(int z = 0; z < 5; z++)
            if(cobra_viva[z] == 1 && cobra_x[z] == jogador.pos.x && cobra_y[z] == jogador.pos.y)
                aplicar_dano_inimigo(jogador.vida);

        char direcao = 0;
        if(computador_jogando){
            direcao = escolher_acao_computador();
            Sleep(450);
        } else if (_kbhit()) {
            int tecla = _getch();

            if (tecla == 0 || tecla == 224) {
                int tecla2 = _getch();
                switch (tecla2) {
                    case 72: direcao = 'w'; break;
                    case 80: direcao = 's'; break;
                    case 75: direcao = 'a'; break;
                    case 77: direcao = 'd'; break;
                }
            } else {
                direcao = static_cast<char>(tecla);
            }
        }

        if (direcao) {
            if (direcao == 'p' || direcao == 'P') {
                posicionar_cursor(ALTURA + 2, 0);
                cout << "Jogo pausado. Pressione P para continuar.      ";
                while (true) {
                    if (_kbhit()) {
                        int tecla_pausa = _getch();
                        if (tecla_pausa == 'p' || tecla_pausa == 'P') {
                            limpar_tela();
                            break;
                        }
                    }
                    Sleep(100);
                }
                continue;
            }

            if(direcao == 'q' || direcao == 'Q'){
                int tipo_inimigo = 0;
                int indice_inimigo = -1;
                if(encontrar_inimigo_para_luta(inimigo_x, inimigo_y, inimigo_vivo, max_inimigos_fase,cobra_x, cobra_y, cobra_viva, tipo_inimigo, indice_inimigo)){
                    iniciar_luta(tipo_inimigo, indice_inimigo,inimigo_x, inimigo_y, inimigo_vivo, inimigo_vida,cobra_x, cobra_y, cobra_viva, cobra_vida);
                    if (boss_derrotado) {
                        break;
                    }
                    inicio = chrono::steady_clock::now();
                    inicio_cobra = chrono::steady_clock::now();
                } else {
                    posicionar_cursor(ALTURA + 2, 0);
                    cout << "Nao ha inimigo perto para lutar.       ";
                    Sleep(computador_jogando ? 1800 : 700);
                }
                continue;
            }

            if(direcao == 'r' || direcao == 'R'){
                int npc_x = 0, npc_y = 0;
                if(encontrar_npc_para_loja(npc_x, npc_y)){
                    if(computador_jogando){
                        comprar_loja_computador();
                    } else {
                        abrir_loja_habilidades();
                    }
                    inicio = chrono::steady_clock::now();
                    inicio_cobra = chrono::steady_clock::now();
                } else {
                    posicionar_cursor(ALTURA + 2, 0);
                    cout << "Nao ha NPC perto para conversar.       ";
                    Sleep(computador_jogando ? 1800 : 700);
                }
                continue;
            }

            Pos pos_anterior = jogador.pos;
            mover_jogador(direcao, cobra_x, cobra_y, cobra_viva);
            if(jogador.pos.x != pos_anterior.x || jogador.pos.y != pos_anterior.y){
                jogador_ja_moveu = true;
            }


        }
    }
    if (jogador.vida <= 0) {
        auto fim_partida = chrono::steady_clock::now();
        int tempo_segundos = static_cast<int>(
            chrono::duration_cast<chrono::seconds>(fim_partida - inicio_partida).count()
        );
        limpar_tela();
        cout << "\n=== GAME OVER! ===" << endl;
        cout << "Salas descobertas: " << jogador.salas_descobertas << "/" << qtd_salas << endl;
        cout << "Pressione Enter para continuar...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Salva com tempo, igual à vitória
        salvar_ranking(jogador_nome, tempo_segundos, false);

    }
    computador_jogando = false;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    SetConsoleOutputCP(65001);

    ///ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, A SEGUIR.
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);

    short int CX = 0, CY = 0;
    COORD coord;
    coord.X = CX;
    coord.Y = CY;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0,0});
    ///ALERTA: NAO MODIFICAR O TRECHO DE CODIGO, ACIMA.

    srand(time(NULL));
    PlaySound(TEXT("musica-rogue.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);//inicia a tocar a música

    int menu = 0;
    while (menu != 4) {
        limpar_tela();
        cout << "======== ROGUELIKE =========" << endl;
        cout << "1 - Jogar"        << endl;
        cout << "2 - Sobre/Regras" << endl;
        cout << "3 - Ranking"      << endl;
        cout << "4 - Sair"         << endl;
        cout << "Escolha: ";

        if (!(cin >> menu)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (menu) {
            case 1: {
                string nome;
                int modo_partida = selecionar_modo_partida();
                dificuldade = selecionar_dificuldade();
                cout << "Digite seu nome: ";
                getline(cin >> ws, nome);
                jogador_nome = nome;
                ranking_salvo_por_vitoria = false;
                mostrar_historia_inicial();
                jogar(modo_partida == 2);
                break;}
            case 2:{
                mostrar_regras();
                break;}
            case 3:{
                limpar_tela();
                mostrar_ranking();
                cout << "\nPressione Enter para voltar..." << endl;
                cin.get();
                break;}
            case 4:{
                cout << "Encerrando..." << endl;
                break;}
            default:
                cout << "Opcao invalida." << endl;
                break;
        }
    }
    PlaySound(NULL, 0, 0);//para de tocar a música

    return 0;
}
