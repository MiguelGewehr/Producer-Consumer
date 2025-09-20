#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include "ordem.h"

// Buffer circular compartilhado
Ordem buffer[BUFFER_SIZE];
int write_pos = 0;

// Semáforos para controle de acesso (usando semáforos nomeados para compatibilidade macOS)
sem_t *sem_empty;  // Posições vazias no buffer
sem_t *sem_full;   // Posições ocupadas no buffer  
sem_t *sem_mutex;  // Exclusão mútua no acesso ao buffer

typedef struct {
    char arquivo[256];
    int id_produtor;
} ProducerArgs;

void* funcao_produtor(void* arg) {
    ProducerArgs* args = (ProducerArgs*)arg;
    FILE* arquivo = fopen(args->arquivo, "r");
    
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir arquivo: %s\n", args->arquivo);
        pthread_exit(NULL);
    }
    
    int itens_produzidos = 0;
    Ordem ordem;
    
    printf("[Produtor %d] Iniciado, lendo arquivo: %s\n", args->id_produtor, args->arquivo);
    
    // Lê ordens do arquivo até o fim
    while ((ordem = ler_ordem_do_arquivo(arquivo)).id != -1) {
        // Espera por espaço vazio no buffer
        sem_wait(sem_empty);
        
        // Acesso exclusivo ao buffer
        sem_wait(sem_mutex);
        
        buffer[write_pos] = ordem;
        write_pos = (write_pos + 1) % BUFFER_SIZE;
        
        sem_post(sem_mutex);
        
        // Sinaliza item disponível
        sem_post(sem_full);
        
        itens_produzidos++;
        printf("[Produtor %d] Item produzido: %s\n", args->id_produtor, ordem.name);
    }
    
    // Insere sinal de fim de produção
    sem_wait(sem_empty);
    sem_wait(sem_mutex);
    
    buffer[write_pos] = ORDEM_FIM;
    write_pos = (write_pos + 1) % BUFFER_SIZE;
    
    sem_post(sem_mutex);
    sem_post(sem_full);
    
    fclose(arquivo);
    printf("[Produtor %d] Finalizado. Itens produzidos: %d\n", args->id_produtor, itens_produzidos);
    
    pthread_exit(NULL);
}

void* funcao_consumidor(void* arg) {
    int read_pos = 0;
    int itens_processados = 0;
    int produtores_ativos = NUM_PRODUCERS;
    
    printf("[Consumidor] Iniciado\n");
    
    while (produtores_ativos > 0) {
        // Espera por item no buffer
        sem_wait(sem_full);
        
        // Lê item do buffer (não precisa de mutex aqui pois só o consumidor lê)
        Ordem item = buffer[read_pos];
        read_pos = (read_pos + 1) % BUFFER_SIZE;
        
        // Libera posição no buffer
        sem_post(sem_empty);
        
        if (ordem_e_valida(&item)) {
            imprimir_ordem(&item);
            itens_processados++;
        } else {
            // Recebeu sinal de fim de um produtor
            produtores_ativos--;
            printf("[Consumidor] Produtor finalizado. Restam: %d\n", produtores_ativos);
        }
    }
    
    printf("[Consumidor] Finalizado. Itens processados: %d\n", itens_processados);
    pthread_exit(NULL);
}

int main() {
    pthread_t produtores[NUM_PRODUCERS];
    pthread_t consumidor;
    ProducerArgs args[NUM_PRODUCERS];
    
    // Inicializa semáforos (usando semáforos nomeados para compatibilidade macOS)
    sem_unlink("/sem_empty"); // Remove se existir
    sem_unlink("/sem_full");
    sem_unlink("/sem_mutex");
    
    sem_empty = sem_open("/sem_empty", O_CREAT | O_EXCL, 0644, BUFFER_SIZE);
    sem_full = sem_open("/sem_full", O_CREAT | O_EXCL, 0644, 0);
    sem_mutex = sem_open("/sem_mutex", O_CREAT | O_EXCL, 0644, 1);
    
    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("Erro ao criar semáforos");
        exit(1);
    }
    
    printf("=== Laboratório Produtor-Consumidor ===\n");
    printf("Buffer size: %d\n", BUFFER_SIZE);
    printf("Número de produtores: %d\n", NUM_PRODUCERS);
    printf("========================================\n\n");
    
    // Cria threads produtoras
    char arquivos[NUM_PRODUCERS][256] = {
        "data/ordem1.txt",
        "data/ordem2.txt", 
        "data/ordem3.txt",
        "data/ordem4.txt"
    };
    
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        args[i].id_produtor = i + 1;
        strcpy(args[i].arquivo, arquivos[i]);
        
        if (pthread_create(&produtores[i], NULL, funcao_produtor, &args[i]) != 0) {
            fprintf(stderr, "Erro ao criar thread produtora %d\n", i);
            exit(1);
        }
    }
    
    // Cria thread consumidora
    if (pthread_create(&consumidor, NULL, funcao_consumidor, NULL) != 0) {
        fprintf(stderr, "Erro ao criar thread consumidora\n");
        exit(1);
    }
    
    // Aguarda finalização de todas as threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(produtores[i], NULL);
    }
    
    pthread_join(consumidor, NULL);
    
    // Limpa recursos
    sem_close(sem_empty);
    sem_close(sem_full);
    sem_close(sem_mutex);
    
    sem_unlink("/sem_empty");
    sem_unlink("/sem_full");
    sem_unlink("/sem_mutex");
    
    printf("\n=== Programa finalizado com sucesso ===\n");
    
    return 0;
}