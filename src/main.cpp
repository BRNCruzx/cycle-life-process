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
    fila *fila_execucao = criar_fila(1);

    // temos um vetor de filas para representar fila para tipo de recurso que um proc pode precisar
    //fila_recurso[0] -> HD
    //fila_recurso[1] -> TECLADO
    //fila_recurso[2] -> MOUSE
    fila *fila_espera_recurso[NUM_RECURSOS];
    for(int i = 0; i < NUM_RECURSOS; i++)
        fila_espera_recurso[i] = criar_fila(i + 3);

    int pid = 1;

    // Criar alguns processos iniciais
    for(int i = 0; i < 5; i++)
    {
        int tempo = rand() % 50 + 10; // tempo total aleatório
        processo *p = criar_processo(pid++, tempo);
        inserir_na_fila(fila_pronto, p);
    }

    printf("---Inicio da simulacao---\n");
    while(!fila_vazia(fila_pronto->qtde) || !fila_vazia(fila_execucao->qtde) || !recursos_vazios(fila_espera_recurso))
    {
        if(!fila_vazia(fila_pronto->qtde))
            exibir_fila(fila_pronto);
            
        // se não tiver nada executando pega da fila de pronto
        if(fila_vazia(fila_execucao->qtde) && !fila_vazia(fila_pronto->qtde))
        {
            processo *p = remover_da_fila(fila_pronto);
            inserir_na_fila(fila_execucao, p);
        }

        if(!fila_vazia(fila_execucao->qtde))
            executar_processo(fila_pronto,fila_execucao, fila_espera_recurso);

        verificar_desbloqueio(fila_espera_recurso, fila_pronto);
      
    }    
    return 0;
}
