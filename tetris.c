#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5
#define TAM_PILHA 3

// Struct que representa uma peça
typedef struct {
    char tipo;
    int id;
} Peca;

// ----- FILA CIRCULAR -----
Peca fila[TAM_FILA];
int inicio = 0, fim = 0, quantidade = 0;

// ----- PILHA -----
Peca pilha[TAM_PILHA];
int topo = -1;

// Controle de IDs das peças
int proximoID = 0;

// Gera tipo aleatório de peça
char gerarTipo() {
    char tipos[] = {'I', 'O', 'T', 'L'};
    return tipos[rand() % 4];
}

// Gera uma nova peça
Peca gerarPeca() {
    Peca nova;
    nova.tipo = gerarTipo();
    nova.id = proximoID++;
    return nova;
}

// Inicializa Fila
void inicializarFila() {
    for(int i = 0; i < TAM_FILA; i++) {
        fila[i] = gerarPeca();
        quantidade++;
        fim = (fim + 1) % TAM_FILA;
    }
}

// Fila cheia e vazia
int filaCheia() { return quantidade == TAM_FILA; }
int filaVazia() { return quantidade == 0; }

// Remove peça da frente da fila
Peca dequeue() {
    Peca removida = fila[inicio];
    inicio = (inicio + 1) % TAM_FILA;
    quantidade--;
    return removida;
}

// Insere peça no fim da fila
void enqueue(Peca p) {
    if (!filaCheia()) {
        fila[fim] = p;
        fim = (fim + 1) % TAM_FILA;
        quantidade++;
    }
}

// Inicializa Pilha
void inicializarPilha() {
    topo = -1;
}

// Pilha cheia e vazia
int pilhaCheia() { return topo == TAM_PILHA - 1; }
int pilhaVazia() { return topo == -1; }

// Adiciona peça na pilha
void push(Peca p) {
    if (!pilhaCheia()) {
        pilha[++topo] = p;
    } else {
        printf("\n⚠️ Pilha cheia! Não é possível reservar mais peças.\n");
    }
}

// Remove peça da pilha
Peca pop() {
    if (!pilhaVazia()) {
        return pilha[topo--];
    } else {
        Peca vazia = {'-', -1};
        return vazia;
    }
}

// Mostrar Fila
void mostrarFila() {
    printf("Fila de peças:\t");
    int idx = inicio;
    for(int i = 0; i < quantidade; i++) {
        printf("[%c %d] ", fila[idx].tipo, fila[idx].id);
        idx = (idx + 1) % TAM_FILA;
    }
    printf("\n");
}

// Mostrar Pilha
void mostrarPilha() {
    printf("Pilha de reserva (Topo -> Base):\t");
    for(int i = topo; i >= 0; i--) {
        printf("[%c %d] ", pilha[i].tipo, pilha[i].id);
    }
    printf("\n");
}

// Mostrar Estado Geral
void mostrarEstado() {
    printf("\n=== Estado Atual ===\n");
    mostrarFila();
    mostrarPilha();
    printf("====================\n");
}

int main() {

    srand(time(NULL));

    inicializarFila();
    inicializarPilha();

    int opc;

    do {
        mostrarEstado();

        printf("\nOpções:\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar peça reservada\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d", &opc);

        switch(opc) {

            case 1: {
                if (!filaVazia()) {
                    Peca jogada = dequeue();
                    printf("\n🎮 Peça jogada: [%c %d]\n", jogada.tipo, jogada.id);
                    enqueue(gerarPeca());
                }
                break;
            }

            case 2: {
                if (!filaVazia()) {
                    if (!pilhaCheia()) {
                        Peca reservada = dequeue();
                        push(reservada);
                        printf("\n📦 Peça reservada: [%c %d]\n", reservada.tipo, reservada.id);
                        enqueue(gerarPeca());
                    } else {
                        printf("\n⚠️ Não há espaço na reserva!\n");
                    }
                }
                break;
            }

            case 3: {
                if (!pilhaVazia()) {
                    Peca usada = pop();
                    printf("\n⏱️ Peça usada da reserva: [%c %d]\n", usada.tipo, usada.id);
                } else {
                    printf("\n⚠️ Não há peças reservadas!\n");
                }
                break;
            }

            case 0:
                printf("\n🏁 Saindo do jogo...\n");
                break;

            default:
                printf("\n❌ Opção inválida!\n");
        }

    } while(opc != 0);

    return 0;
}
