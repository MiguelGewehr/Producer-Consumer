#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct Ordem
{
    char name[200];
    int id;
    int qtd;
}Ordem;

Ordem buffer[4];
int writepos = 0; 

sem_t EMPTY; 
sem_t FULL; 
sem_t LOCK; 

Ordem ordem_leitura_e_retorno(FILE *fp) {
    
    char name[200];
    int id;
    int qtd;
    int fimArquivo = fscanf(fp, "{%[^,], %d, %d}\n", name, &id, &qtd);

    if (fimArquivo <= 0)
        return (Ordem){"Vazio", -1, -1};
    
    Ordem ordem;
    ordem.id = id;
    ordem.qtd = qtd;
    strcpy(ordem.name, name);

    return ordem;
}

void *funcao_do_produtor(void *path) {
    
    int i = 0;

    Ordem ordem;
    FILE *arq = fopen((char*)path, "r"); 

    if(arq == NULL) 
        exit(printf("Erro na abertura do arquivo: %s\n", (char*)path));
    

    while(1) {
        ordem = ordem_leitura_e_retorno(arq);

        sem_wait(&EMPTY); 
        sem_wait(&LOCK); 

        buffer[writepos] = ordem; 
        writepos = (writepos + 1) % 4; 

        sem_post(&LOCK); 
        sem_post(&FULL); 

        if (ordem.id == -1)
            break;

        i++; 
    }

    fclose(arq); 

    printf("Produtor terminou após produzir %d itens\n", i); 

    return NULL;
}

void *funcao_do_consumidor() {
    int i = 0;
    int readpos = 0; 
    int prodCount = 4; // qtd de threads produtoras em execução
    Ordem item;
    while(1) {
        sem_wait(&FULL); // Espera por um item

        item = buffer[readpos]; // Lê o item do buffer
        readpos = (readpos + 1) % 4; // Atualiza a posição de leitura

        sem_post(&EMPTY); // Sinaliza que uma posição foi liberada
        //Verifica se o item recebido foi um sinal de saida de um dos produtores
        if (item.id == -1) {
            //Diminui a qtd de produtores e verifica se não existe algum produtor em execução
            if(!(--prodCount))
                break;
        }
        else {
            printf("Processando ordem: %s, %d, %d\n", item.name, item.id, item.qtd);
            i++;
        }
    }

    printf("Consumidor terminou após processar %d itens\n", i);

    return NULL;
}

int main() {
    
    pthread_t produtor[4]; 
    pthread_t consumidor;

    sem_init(&EMPTY, 0, 4);
    sem_init(&FULL, 0, 0);
    sem_init(&LOCK, 0, 1);

    pthread_create(&produtor[0], NULL, funcao_do_produtor, "Ordens/1_Ordem.txt");
    pthread_create(&produtor[1], NULL, funcao_do_produtor, "Ordens/2_Ordem.txt");
    pthread_create(&produtor[2], NULL, funcao_do_produtor, "Ordens/3_Ordem.txt");
    pthread_create(&produtor[3], NULL, funcao_do_produtor, "Ordens/4_Ordem.txt");
    
    pthread_create(&consumidor, NULL, funcao_do_consumidor, NULL);

    pthread_join(produtor[0], NULL);
    pthread_join(produtor[1], NULL);
    pthread_join(produtor[2], NULL);
    pthread_join(produtor[3], NULL);

    pthread_join(consumidor, NULL);

    return 0;
}
