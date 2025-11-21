#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Desafio Tetris Stack
// Tema 3 - Integração de Fila e Pilha
// NÍVEL NOVATO — Fila Circular de Peças Futuras

#define TAM_FILA 5

// Struct da peça
typedef struct {
    char tipo; // I, O, T, L
    int id;    // identificador único
} Peca;

// Estrutura da fila circular
typedef struct {
    Peca elementos[TAM_FILA];
    int inicio;
    int qtd;
} Fila;

// ---------- Protótipos das Funções ----------
void inicializarFila(Fila *f);
int filaVazia(Fila *f);
int filaCheia(Fila *f);
void enqueue(Fila *f, Peca p);
Peca dequeue(Fila *f);
void mostrarFila(Fila *f);
Peca gerarPeca();


// ---------- MAIN ----------
int main() {
    Fila fila;
    int opcao;

    srand(time(NULL));
    inicializarFila(&fila);

    // Inicializa com 5 peças automáticas
    for (int i = 0; i < TAM_FILA; i++) {
        enqueue(&fila, gerarPeca());
    }

    do {
        mostrarFila(&fila);

        printf("\nOpções:\n");
        printf("1 - Jogar peça (dequeue)\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            if (!filaVazia(&fila)) {
                Peca jogada = dequeue(&fila);
                printf("\nPeça jogada: [%c %d]\n", jogada.tipo, jogada.id);
                enqueue(&fila, gerarPeca()); // Sempre repõe!
            } else {
                printf("\nFila vazia! Não há peça para jogar.\n");
            }
        } else if (opcao != 0) {
            printf("\n⚠ Opção inválida!\n");
        }

    } while (opcao != 0);

    printf("\nEncerrando programa...\n");
    return 0;
}


// ---------- Funções da Fila ----------

void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->qtd = 0;
}

int filaVazia(Fila *f) {
    return f->qtd == 0;
}

int filaCheia(Fila *f) {
    return f->qtd == TAM_FILA;
}

void enqueue(Fila *f, Peca p) {
    if (filaCheia(f)) {
        printf("Fila cheia! Não foi possível inserir.\n");
        return;
    }
    int pos = (f->inicio + f->qtd) % TAM_FILA;
    f->elementos[pos] = p;
    f->qtd++;
}

Peca dequeue(Fila *f) {
    Peca vazia = {' ', -1};
    if (filaVazia(f)) {
        return vazia;
    }
    Peca removida = f->elementos[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA;
    f->qtd--;
    return removida;
}

// Mostrar o estado atual da fila
void mostrarFila(Fila *f) {
    printf("\nFila de Peças:\n");
    if (filaVazia(f)) {
        printf("(vazia)\n");
        return;
    }

    int index = f->inicio;
    for (int i = 0; i < f->qtd; i++) {
        printf("[%c %d] ", f->elementos[index].tipo, f->elementos[index].id);
        index = (index + 1) % TAM_FILA;
    }
    printf("\n");
}

// Gera peça automática com tipo aleatório e id sequencial
Peca gerarPeca() {
    static int contadorID = 0;
    char tipos[] = {'I', 'O', 'T', 'L'};
    int r = rand() % 4;

    Peca p;
    p.tipo = tipos[r];
    p.id = contadorID++;
    return p;
}
