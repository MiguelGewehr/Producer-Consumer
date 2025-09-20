#include "ordem.h"
#include <string.h>
#include <stdio.h>

const Ordem ORDEM_FIM = {"FIM", -1, -1};

Ordem ler_ordem_do_arquivo(FILE *fp) {
    if (!fp) {
        return ORDEM_FIM;
    }
    
    char name[MAX_NAME_LENGTH];
    int id, qtd;
    
    // Tenta ler no formato: {Nome, ID, Quantidade}
    int resultado = fscanf(fp, " {%199[^,], %d, %d}", name, &id, &qtd);
    
    if (resultado != 3) {
        // Se falhou, tenta formato alternativo: Nome ID Quantidade
        resultado = fscanf(fp, " %199s %d %d", name, &id, &qtd);
        
        if (resultado != 3) {
            return ORDEM_FIM;
        }
    }
    
    Ordem ordem;
    strncpy(ordem.name, name, MAX_NAME_LENGTH - 1);
    ordem.name[MAX_NAME_LENGTH - 1] = '\0';
    ordem.id = id;
    ordem.qtd = qtd;
    
    return ordem;
}

int ordem_e_valida(const Ordem *ordem) {
    return ordem && ordem->id != -1;
}

void imprimir_ordem(const Ordem *ordem) {
    if (!ordem) return;
    
    if (ordem_e_valida(ordem)) {
        printf("Processando ordem: %s, %d, %d\n", ordem->name, ordem->id, ordem->qtd);
    } else {
        printf("Ordem inválida ou fim de arquivo\n");
    }
}