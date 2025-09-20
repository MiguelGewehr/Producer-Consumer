#ifndef ORDEM_H
#define ORDEM_H

#include <stdio.h>

#define MAX_NAME_LENGTH 200
#define BUFFER_SIZE 4
#define NUM_PRODUCERS 4

typedef struct {
    char name[MAX_NAME_LENGTH];
    int id;
    int qtd;
} Ordem;

// Função para ler uma ordem do arquivo
Ordem ler_ordem_do_arquivo(FILE *fp);

// Função para verificar se uma ordem é válida
int ordem_e_valida(const Ordem *ordem);

// Função para imprimir uma ordem
void imprimir_ordem(const Ordem *ordem);

// Constante para indicar fim de arquivo/fim de produção
extern const Ordem ORDEM_FIM;

#endif