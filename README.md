# Laboratório: Problema Produtor-Consumidor

## Descrição
Este projeto implementa o clássico problema produtor-consumidor usando threads POSIX e semáforos em C. O sistema simula o processamento de ordens de compra, onde múltiplos produtores (threads) leem arquivos de ordens e um único consumidor processa essas ordens.

## Estrutura do Projeto
```
produtor-consumidor/
├── src/
│   ├── main.c           # Arquivo principal com implementação
│   ├── ordem.h          # Header com definições da estrutura Ordem
│   └── ordem.c          # Implementação das funções de ordem
├── data/
│   ├── ordem1.txt       # Arquivo de ordens do produtor 1
│   ├── ordem2.txt       # Arquivo de ordens do produtor 2  
│   ├── ordem3.txt       # Arquivo de ordens do produtor 3
│   └── ordem4.txt       # Arquivo de ordens do produtor 4
├── build/               # Diretório para arquivos compilados
├── Makefile
└── README.md
```

## Como Funciona

### Componentes
- **Buffer Circular**: Array de 4 posições que armazena as ordens temporariamente
- **Produtores**: 4 threads que leem arquivos de ordens e inserem no buffer
- **Consumidor**: 1 thread que retira ordens do buffer e as processa
- **Semáforos**: Controlam o acesso sincronizado ao buffer compartilhado

### Sincronização
- `EMPTY`: Conta posições vazias no buffer (inicializado com 4)
- `FULL`: Conta posições ocupadas no buffer (inicializado com 0)
- `LOCK`: Mutex para acesso exclusivo ao buffer (inicializado com 1)

### Formato dos Arquivos de Ordem
Cada linha no arquivo deve seguir o formato:
```
{Nome, ID, Quantidade}
```
Exemplo:
```
{Miguel, 1345, 12}
{Aurea, 2345, 54}
```

## Compilação e Execução

### Compilar
```bash
make
```

### Executar
```bash
./build/main
```

### Limpar arquivos compilados
```bash
make clean
```

## Funcionamento Detalhado

1. **Inicialização**: O programa cria 4 threads produtoras e 1 consumidora
2. **Produtores**: Cada thread lê um arquivo diferente e insere ordens no buffer
3. **Consumidor**: Remove ordens do buffer e as processa em ordem FIFO
4. **Terminação**: Quando um produtor termina, insere um item especial (id = -1) para sinalizar fim
5. **Finalização**: O consumidor para quando todos os 4 produtores terminam

## Conceitos de SO Demonstrados
- Threads POSIX (pthread)
- Semáforos para sincronização
- Buffer circular compartilhado
- Problema clássico produtor-consumidor
- Exclusão mútua e condições de corrida