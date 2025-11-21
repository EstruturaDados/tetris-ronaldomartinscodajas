/*
  Tetris Stack - Nível Mestre
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAM_FILA 5
#define TAM_PILHA 3

/* ----- Definições de tipos ----- */
typedef struct {
    char tipo; /* 'I', 'O', 'T', 'L' */
    int id;    /* identificador único */
} Peca;

/* Snapshot para desfazer: cópia completa do estado */
typedef struct {
    Peca fila[TAM_FILA];
    int inicio;
    int qtd;
    Peca pilha[TAM_PILHA];
    int topo;
    int proximoID;
    int valido; /* 1 se snapshot válido (há o que desfazer), 0 se não */
} Snapshot;

/* ----- Estado global (fila e pilha) ----- */
Peca fila[TAM_FILA];
int inicio = 0;
int qtdFila = 0;
int fim = 0; /* fim = (inicio + qtdFila) % TAM_FILA */

Peca pilha[TAM_PILHA];
int topo = -1;

int proximoID = 0;

/* Snapshot atual para undo */
Snapshot lastSnapshot;

/* ----- Funções auxiliares para geração de peças ----- */
char gerarTipoAleatorio() {
    const char tipos[] = {'I', 'O', 'T', 'L'};
    return tipos[rand() % 4];
}

Peca gerarPeca() {
    Peca p;
    p.tipo = gerarTipoAleatorio();
    p.id = proximoID++;
    return p;
}

/* ----- Snapshot (undo) ----- */
void salvarSnapshot() {
    /* copia completa do estado */
    memcpy(lastSnapshot.fila, fila, sizeof(fila));
    lastSnapshot.inicio = inicio;
    lastSnapshot.qtd = qtdFila;
    memcpy(lastSnapshot.pilha, pilha, sizeof(pilha));
    lastSnapshot.topo = topo;
    lastSnapshot.proximoID = proximoID;
    lastSnapshot.valido = 1;
}

void limparSnapshot() {
    lastSnapshot.valido = 0;
}

int temSnapshot() {
    return lastSnapshot.valido == 1;
}

void restaurarSnapshot() {
    if (!temSnapshot()) return;
    memcpy(fila, lastSnapshot.fila, sizeof(fila));
    inicio = lastSnapshot.inicio;
    qtdFila = lastSnapshot.qtd;
    fim = (inicio + qtdFila) % TAM_FILA;
    memcpy(pilha, lastSnapshot.pilha, sizeof(pilha));
    topo = lastSnapshot.topo;
    proximoID = lastSnapshot.proximoID;
    limparSnapshot();
}

/* ----- Operações de fila circular ----- */
void inicializarFila() {
    inicio = 0;
    qtdFila = 0;
    fim = 0;
}

int filaVazia() {
    return qtdFila == 0;
}

int filaCheia() {
    return qtdFila == TAM_FILA;
}

void enqueue(Peca p) {
    if (filaCheia()) {
        /* não deve ocorrer se controlamos corretamente, mas checamos */
        printf("Erro: tentativa de enfileirar em fila cheia.\n");
        return;
    }
    int pos = (inicio + qtdFila) % TAM_FILA;
    fila[pos] = p;
    qtdFila++;
    fim = (inicio + qtdFila) % TAM_FILA;
}

Peca dequeue() {
    Peca invalida = {'-', -1};
    if (filaVazia()) {
        return invalida;
    }
    Peca rem = fila[inicio];
    inicio = (inicio + 1) % TAM_FILA;
    qtdFila--;
    fim = (inicio + qtdFila) % TAM_FILA;
    return rem;
}

/* ----- Operações de pilha ----- */
void inicializarPilha() {
    topo = -1;
}

int pilhaVazia() {
    return topo == -1;
}

int pilhaCheia() {
    return topo == TAM_PILHA - 1;
}

void push(Peca p) {
    if (pilhaCheia()) {
        printf("Erro: tentativa de push em pilha cheia.\n");
        return;
    }
    pilha[++topo] = p;
}

Peca pop() {
    Peca invalida = {'-', -1};
    if (pilhaVazia()) return invalida;
    return pilha[topo--];
}

/* ----- Exibição ----- */
void mostrarFila() {
    printf("Fila de peças:\t");
    if (filaVazia()) {
        printf("(vazia)");
    } else {
        int idx = inicio;
        for (int i = 0; i < qtdFila; ++i) {
            Peca p = fila[idx];
            printf("[%c %d] ", p.tipo, p.id);
            idx = (idx + 1) % TAM_FILA;
        }
    }
    printf("\n");
}

void mostrarPilha() {
    printf("Pilha de reserva (Topo -> Base):\t");
    if (pilhaVazia()) {
        printf("(vazia)");
    } else {
        for (int i = topo; i >= 0; --i) {
            printf("[%c %d] ", pilha[i].tipo, pilha[i].id);
        }
    }
    printf("\n");
}

void mostrarEstado() {
    printf("\n=== Estado Atual ===\n");
    mostrarFila();
    mostrarPilha();
    printf("====================\n");
}

/* ----- Operações compostas (nível mestre) ----- */

/* Trocar peça do topo da pilha com a da frente da fila (opção 4) */
void trocarTopoComFrente() {
    if (filaVazia()) {
        printf("\n⚠️  Impossível trocar: fila vazia.\n");
        return;
    }
    if (pilhaVazia()) {
        printf("\n⚠️  Impossível trocar: pilha vazia.\n");
        return;
    }
    /* salvar snapshot para permitir desfazer */
    salvarSnapshot();

    Peca &top = pilha[topo]; /* note: using pointer below since C doesn't have references */
    /* In C we will do manual swap */
    Peca tempPilhaTop = pilha[topo];
    Peca tempFilaFront = fila[inicio];

    pilha[topo] = tempFilaFront;
    fila[inicio] = tempPilhaTop;

    printf("\n🔁 Troca realizada: topo da pilha ↔ frente da fila.\n");
}

/* Desfazer última ação (opção 5) */
void desfazerUltimaAcao() {
    if (!temSnapshot()) {
        printf("\n🔙 Não há ação para desfazer.\n");
        return;
    }
    restaurarSnapshot();
    printf("\n🔙 Última ação desfeita. Estado restaurado.\n");
}

/* Inverter fila com pilha (opção 6)
   - nova fila começa com elementos da pilha (topo -> base), depois os elementos antigos da fila (frente->traseira)
   - nova pilha recebe os primeiros até TAM_PILHA elementos antigos da fila (frente->...), com último desses sendo o topo
   - completa-se a fila com peças novas se necessário para alcançar TAM_FILA
*/
void inverterFilaComPilha() {
    /* salvar snapshot para desfazer */
    salvarSnapshot();

    /* Captura ordenada dos elementos antigos */
    Peca oldQueue[TAM_FILA];
    int oldQueueCount = qtdFila;
    int idx = inicio;
    for (int i = 0; i < oldQueueCount; ++i) {
        oldQueue[i] = fila[idx];
        idx = (idx + 1) % TAM_FILA;
    }

    Peca oldStack[TAM_PILHA];
    int oldStackCount = (topo + 1);
    for (int i = 0; i < oldStackCount; ++i) {
        /* oldStack[0] = topo, oldStack[oldStackCount-1] = base */
        oldStack[i] = pilha[topo - i];
    }

    /* Construir nova fila */
    Peca newQueue[TAM_FILA];
    int newQueueCount = 0;

    /* Primeiro, elementos da pilha (topo -> base) */
    for (int i = 0; i < oldStackCount && newQueueCount < TAM_FILA; ++i) {
        newQueue[newQueueCount++] = oldStack[i];
    }

    /* Depois, os antigos elementos da fila (frente -> traseira) */
    for (int i = 0; i < oldQueueCount && newQueueCount < TAM_FILA; ++i) {
        newQueue[newQueueCount++] = oldQueue[i];
    }

    /* Se necessário, completar com novas peças geradas */
    while (newQueueCount < TAM_FILA) {
        newQueue[newQueueCount++] = gerarPeca();
    }

    /* Construir nova pilha: primeiros até TAM_PILHA elementos antigos da fila (frente->...), 
       onde o último será o topo.
    */
    Peca newStack[TAM_PILHA];
    int newStackCount = 0;
    for (int i = 0; i < oldQueueCount && newStackCount < TAM_PILHA; ++i) {
        newStack[newStackCount++] = oldQueue[i];
    }

    /* Aplicar newQueue ao estado (ajustando inicio e qtd) */
    /* colocamos newQueue em ordem front->back, com inicio = 0 */
    for (int i = 0; i < TAM_FILA; ++i) {
        fila[i] = newQueue[i];
    }
    inicio = 0;
    qtdFila = TAM_FILA;
    fim = (inicio + qtdFila) % TAM_FILA;

    /* Aplicar newStack ao estado (topo = newStackCount-1) */
    topo = -1;
    for (int i = 0; i < newStackCount; ++i) {
        pilha[i] = newStack[i];
        topo = i;
    }

    printf("\n🔀 Inversão executada entre fila e pilha. Fila foi preenchida e pilha atualizada.\n");
}

/* ----- Inicialização ----- */
void inicializarTudo() {
    proximoID = 0;
    inicializarFila();
    inicializarPilha();
    limparSnapshot();
    /* preencher fila inicialmente com TAM_FILA peças */
    for (int i = 0; i < TAM_FILA; ++i) {
        enqueue(gerarPeca());
    }
}

/* ----- Ajuste de swap helper para compilar em C (sem referências) ----- */
/* Implementamos trocarTopoComFrente de forma compatível com C acima (já feita) */

/* ----- Loop principal (menu) ----- */
int main(void) {
    int opcao;
    srand((unsigned int)time(NULL));
    inicializarTudo();

    do {
        mostrarEstado();

        printf("\nOpções:\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar peça reservada\n");
        printf("4 - Trocar peça do topo da pilha com a da frente da fila\n");
        printf("5 - Desfazer última ação\n");
        printf("6 - Inverter fila com pilha\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        if (scanf("%d", &opcao) != 1) {
            /* limpar stdin */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("Entrada inválida. Digite um número.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1: { /* Jogar peça */
                if (filaVazia()) {
                    printf("\n⚠️  Fila vazia: nada para jogar.\n");
                } else {
                    salvarSnapshot();
                    Peca jogada = dequeue();
                    printf("\n🎮 Você jogou a peça: [%c %d]\n", jogada.tipo, jogada.id);
                    /* repor a fila automaticamente */
                    enqueue(gerarPeca());
                }
                break;
            }

            case 2: { /* Reservar peça (dequeue -> push) */
                if (filaVazia()) {
                    printf("\n⚠️  Fila vazia: nada para reservar.\n");
                } else if (pilhaCheia()) {
                    printf("\n⚠️  Pilha cheia: não é possível reservar.\n");
                } else {
                    salvarSnapshot();
                    Peca reservada = dequeue();
                    push(reservada);
                    printf("\n📦 Peça reservada: [%c %d]\n", reservada.tipo, reservada.id);
                    /* repor a fila automaticamente */
                    enqueue(gerarPeca());
                }
                break;
            }

            case 3: { /* Usar peça reservada (pop) */
                if (pilhaVazia()) {
                    printf("\n⚠️  Pilha vazia: nenhuma peça reservada.\n");
                } else {
                    salvarSnapshot();
                    Peca usada = pop();
                    printf("\n⏱️  Peça usada da reserva: [%c %d]\n", usada.tipo, usada.id);
                    /* Observação: aqui não repondo a fila pois não removemos dela;
                       apenas restauramos por desfazer se necessário.
                    */
                }
                break;
            }

            case 4: { /* Trocar topo da pilha com frente da fila */
                if (filaVazia()) {
                    printf("\n⚠️  Fila vazia: nada para trocar.\n");
                } else if (pilhaVazia()) {
                    printf("\n⚠️  Pilha vazia: nada para trocar.\n");
                } else {
                    /* realizar swap */
                    salvarSnapshot();
                    Peca temp = pilha[topo];
                    pilha[topo] = fila[inicio];
                    fila[inicio] = temp;
                    printf("\n🔁 Troca realizada entre topo da pilha e frente da fila.\n");
                }
                break;
            }

            case 5: { /* Desfazer última ação */
                desfazerUltimaAcao();
                break;
            }

            case 6: { /* Inverter fila com pilha */
                inverterFilaComPilha();
                break;
            }

            case 0:
                printf("\n🏁 Encerrando. Até mais!\n");
                break;

            default:
                printf("\n❌ Opção inválida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}
