#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#include "tads/tadProcess.h"

int main()
{
    srand(time(NULL));

    // Criação das filas por tipo
    fila *fila_pronto = criar_fila(0);
    fila *fila_espera = criar_fila(1);
    fila *fila_execucao = criar_fila(2);

    int pid = 1;

    // Criar alguns processos iniciais
    for(int i = 0; i < 5; i++)
    {
        int tempo = rand() % 50 + 10; // tempo total aleatório
        processo *p = criar_processo(pid++, tempo);
        inserir_na_fila(fila_pronto, p);
    }

    printf("---Inicio da simulacao---\n");
    while(!fila_vazia(fila_pronto->qtde) || !fila_vazia(fila_espera->qtde) || !fila_vazia(fila_execucao->qtde))
    {

        exibir_fila(fila_pronto);
        // se não tiver nada executando pega da fila de pronto
        if(fila_vazia(fila_execucao->qtde) && !fila_vazia(fila_pronto->qtde))
        {
            processo *p = remover_da_fila(fila_pronto);
            inserir_na_fila(fila_execucao, p);
        }
        
        if(!fila_vazia(fila_execucao->qtde))
        {
            processo *p = remover_da_fila(fila_execucao);
            printf("\nExecutando processo %d\n", p->pid);
            delete(p);
        }
    }
    if(fila_vazia(fila_espera->qtde))
        printf("\nFila vazia.");
    
    return 0;
}
