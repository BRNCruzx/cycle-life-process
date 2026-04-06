#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <time.h>
#ifdef WIN32
    #include <windows.h>
    #define sleep(x) Sleep(x * 1000);
    #include "./headers/conio/conio-dos.h"
    #include "./headers/conio/meuconio.h"
#else
    #include "./conio/conio-unix.h"
#endif
#include "tads/tadProcess.h"
#include "moldura/dashboard.h" 


int main()
{
    srand(time(NULL));

    // Criação das filas por tipo
    fila *fila_pronto = criar_fila(0);
    fila *fila_execucao = criar_fila(1);
    fila *fila_espera_filho = criar_fila(2); //pra retornar como espera
    fila *fila_finalizados = criar_fila(10);

    // temos um vetor de filas para representar fila para tipo de recurso que um proc pode precisar
    //fila_recurso[0] -> HD
    //fila_recurso[1] -> TECLADO
    //fila_recurso[2] -> MOUSE
    fila *fila_espera_recurso[NUM_RECURSOS];
    for(int i = 0; i < NUM_RECURSOS; i++)
        fila_espera_recurso[i] = criar_fila(i + 3);

    int encerrar = 0;
    int pid = 1;
    int tempo_sistema = 0;
    int tempo_executado = 0;

    int proc_finalizados = 0;
    int proc_exec_prontos = 0;
    int qtde_bloqueados = 0;
    int tempo_total_bloqueado = 0;


    // Criar alguns processos iniciais
    for(int i = 0; i < 5; i++)
    {
        int tempo = rand() % 50 + 10; // tempo total aleatório
        processo *p = criar_processo(pid++, tempo, tempo_sistema);
        inserir_na_fila(fila_pronto, p);
    }

    printf("---Inicio da simulacao---\n");
    float tmed ;
    while (!encerrar &&
      (!fila_vazia(fila_pronto->qtde) ||
       !fila_vazia(fila_execucao->qtde) ||
       !fila_vazia(fila_espera_filho->qtde) ||
       !recursos_vazios(fila_espera_recurso) || 
        !fila_vazia(fila_espera_filho->qtde)))
    {
         tmed = qtde_bloqueados > 0
             ? (float)tempo_total_bloqueado / qtde_bloqueados
             : 0;
        if (!fila_vazia(fila_pronto->qtde))
              dashboard_exibir(fila_pronto, fila_execucao, fila_espera_recurso,
                 fila_espera_filho, fila_finalizados,
                 tempo_sistema,
                 proc_finalizados, proc_exec_prontos,
                 qtde_bloqueados, tmed);
        verificar_desbloqueio(fila_espera_recurso, fila_pronto, tempo_sistema, &tempo_total_bloqueado);
 
        // Se nao tiver nada executando, pega da fila de pronto
        if (fila_vazia(fila_execucao->qtde) && !fila_vazia(fila_pronto->qtde))
        {
            processo *p = remover_da_fila(fila_pronto);
            p->tempo_espera += tempo_sistema - p->entrou_pronto;
            inserir_na_fila(fila_execucao, p);
        }
 
        if (!fila_vazia(fila_execucao->qtde))
        {
            tempo_executado = executar_processo(fila_pronto, fila_execucao,fila_espera_recurso, fila_espera_filho,tempo_sistema, pid, &proc_finalizados, &proc_exec_prontos, &qtde_bloqueados, fila_finalizados);
        }
 
        if (tempo_executado == 0)
            tempo_sistema++;
        else
            tempo_sistema += tempo_executado;
        if (kbhit())
        {
            int tecla = getch();

            if (tecla == 27) // ESC
                encerrar = 1;

            if (tecla == 13) // ENTER
                printf("Enter pressionado\n");
        }
        sleep(1);
    }

    float tempo_medio_bloqueio = 0;
    if(qtde_bloqueados > 0)
        tempo_medio_bloqueio = tempo_total_bloqueado / qtde_bloqueados;
 
    printf("\n---Fim da simulacao--- Tempo total: %d\n", tempo_sistema);
    dashboard_exibir(fila_pronto, fila_execucao, fila_espera_recurso,
                 fila_espera_filho, fila_finalizados,
                 tempo_sistema,
                 proc_finalizados, proc_exec_prontos,
                 qtde_bloqueados, tmed);

    printf("\n===== TEMPO DE EXECUCAO DOS PROCESSOS =====\n");
    imprimir_processos_restantes(
        fila_pronto,
        fila_execucao,
        fila_espera_recurso,
        fila_espera_filho,
        fila_finalizados,
        tempo_sistema);
       

    return 0;
}
