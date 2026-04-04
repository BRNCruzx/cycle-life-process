#include <stdio.h>
#include <stdlib.h>

#define MAX_TEMPO_CPU 10
#define NUM_RECURSOS 3

struct filho
{
    int pid;
    int tempo_execucao_total;
    struct filho *prox;
};

struct processo
{
    int pid;
    int pid_pai; // pensei em colocar -1 se nao for um processo criado por fork
    int tempo_total;
    int tempo_executado;
    int tempo_total_execucao;

    int tempo_espera;
    int tempo_bloqueado;

    int tempo_inicio;
    int tempo_fim;

    int entrou_pronto;
    int entrou_bloqueado;

    int criou_filho;
    int esperando_filho;
    int possui_filho;
    int terminou;

    filho *lista_filhos;

    struct processo *prox;
};

struct fila
{
    processo *inicio;
    processo *fim;
    int qtde;
    int finalidade; // 0 = Pronto   1 = Execucao
};


// Função para criar uma nova fila de acordo com seu tipo
fila *criar_fila(int finalidade)
{
    fila *nova_fila = new fila;
    
    nova_fila->inicio = NULL;
    nova_fila->fim = NULL;
    nova_fila->qtde = 0;
    nova_fila->finalidade = finalidade;
    
    return nova_fila;
}

// Funcao para saber se fila vazia ou nao
char fila_vazia(int qtde) 
{
    return (qtde==0);
}

// Função para inserir processo no final da fila
void inserir_na_fila(fila *f, processo *p) 
{
    p->prox = NULL;
    if (fila_vazia(f->qtde)) 
    {
        f->inicio = p;
        f->fim = p;
    } 
    else 
    {
        f->fim->prox = p;
        f->fim = p;
    }
    
    f->qtde = f->qtde + 1;
}

// Função para remover processo do início da fila
processo *remover_da_fila(fila *f) 
{
    processo *removido = f->inicio;
    f->inicio = removido->prox;
    
    if (f->inicio == NULL) 
        f->fim = NULL;
    
    f->qtde = f->qtde - 1;
    removido->prox = NULL;
    
    return removido;
}

// Função para criar um novo processo
processo *criar_processo(int pid, int tempo_total, int tempo_sistema)
{
    processo *novo = new processo;

    novo->pid = pid;
    novo->pid_pai = -1;
    novo->tempo_total = tempo_total;
    novo->tempo_executado = 0;
    novo->tempo_total_execucao = 0;

    novo->tempo_espera = 0;
    novo->tempo_bloqueado = 0;

    novo->tempo_inicio = tempo_sistema;
    novo->tempo_fim = 0;

    novo->entrou_pronto = tempo_sistema;
    novo->entrou_bloqueado = -1;

    novo->terminou = 0;
    novo->prox = NULL;

    return novo;
}

// Função para adicionar um filho na lista de filhos do processo pai
void adicionar_filho(processo *pai, processo *filho_do_pai) 
{
    filho *novo_filho = new filho;

    novo_filho->pid = filho_do_pai->pid;
    novo_filho->tempo_execucao_total = 0;
    novo_filho->prox = pai->lista_filhos;
    pai->lista_filhos = novo_filho;
    pai->possui_filho = 1;
}

// Função para liberar filho da lista de filhos
void liberar_filhos(processo *p)
{
    filho *atual = p->lista_filhos;
    filho *ant, *temp;
    
    if(atual->pid == p->pid) // filho no inicio
    {
        p->lista_filhos = atual->prox;
        free(atual);
    }
    else
    {
        while (atual->pid != p->pid && atual != NULL) 
        {
            ant = atual;
            atual = atual->prox;
        }
            
        if(atual->pid == p->pid)
        {
            temp = atual;
            atual = atual->prox;
            ant->prox = atual;
            
            free(temp);
        }
    }
    
    if(p->lista_filhos == NULL)
        p->possui_filho = 0;
}

int recursos_vazios(fila *fila_recurso[])
{
    for(int i = 0; i < NUM_RECURSOS; i++)
    {
        if(!fila_vazia(fila_recurso[i]->qtde))
            return 0; // ainda existe processo esperando
    }
    return 1; // todas estão vazias
}

const char* qual_finalidade(fila *f)
{
    if(f->finalidade == 0)
        return "PRONTO";
    else if(f->finalidade == 1)
        return "EXECUCAO";
    else
        return "ESPERA";
}

const char* qual_recurso(int recurso)
{
    if(recurso == 0)
        return "HD";
    else if(recurso == 1)
        return "TECLADO";
    else
        return "MOUSE";
}

void exibir_fila(fila *f, int tempo_sistema)
{
    processo *aux = f->inicio;

    printf("\n===== FILA %s =====\n", qual_finalidade(f));

    if(aux == NULL)
    {
        printf("Fila vazia\n");
        return;
    }

    while(aux != NULL)
    {
        int espera_atual = aux->tempo_espera;

        // se está na fila de pronto ainda está esperando
        if(f->finalidade == 0)
            espera_atual += tempo_sistema - aux->entrou_pronto;

        printf("PID:%d | Pai:%d | Total:%d | Exec:%d | Rest:%d | Espera:%d | Bloq:%d\n",
               aux->pid,
               aux->pid_pai,
               aux->tempo_total,
               aux->tempo_executado,
               aux->tempo_total - aux->tempo_executado,
               espera_atual,
               aux->tempo_bloqueado);

        aux = aux->prox;
    }
}

void exibir_filas_recurso(fila *fila_recurso[], int tempo_sistema)
{
    for(int i = 0; i < NUM_RECURSOS; i++)
    {
        printf("\n--- FILA RECURSO %s ---\n", qual_recurso(i));
        processo *aux = fila_recurso[i]->inicio;

        if(aux == NULL)
            printf("Fila vazia\n");
        else
        {
             while(aux != NULL)
            {
                int bloqueio_atual = aux->tempo_bloqueado;

                if(fila_recurso[i]->finalidade >= 3) // filas de recurso
                    bloqueio_atual += tempo_sistema - aux->entrou_bloqueado;

                printf("PID:%d | Bloqueado desde: %d\n",
                    aux->pid,
                    aux->entrou_bloqueado);

                aux = aux->prox;
            }
        }
    }
}

void exibir_estado_sistema(fila *pronto, fila *execucao, fila *fila_recurso[], int tempo_sistema)
{
    printf("\n\n=================================\n");
    printf("TEMPO DO SISTEMA: %d\n", tempo_sistema);
    printf("=================================\n");

    exibir_fila(pronto, tempo_sistema);
    exibir_fila(execucao, tempo_sistema);
    exibir_filas_recurso(fila_recurso, tempo_sistema);

    printf("\n=================================\n");
}

// Verifica se terminou e para qual fila vai entrar
int executar_processo(fila *pronto, fila *exec, fila *fila_recurso[], int tempo_sistema)
{
    processo *p = remover_da_fila(exec);

    int restante = p->tempo_total - p->tempo_executado;

    // verifica se já executou tudo ou se nao pega o restante que falta para executar
    int tempo_exec = (restante > MAX_TEMPO_CPU) ? MAX_TEMPO_CPU : restante;
    printf("\nExecutando processo %d por %d UT\n", p->pid, tempo_exec);
    p->tempo_executado += tempo_exec;

    if(p->tempo_executado >= p->tempo_total) //verifica se já executou tudo
    {
        printf("Processo %d finalizou\n", p->pid);
        p->tempo_fim = tempo_sistema + tempo_exec;
        p->tempo_total_execucao = p->tempo_fim - p->tempo_inicio;
        delete p;
    }
    else
    {
        int bloqueia = rand() % 100; // sorteia se vai bloquear por recurso
        if(bloqueia < 30)
        {
            int recurso = rand() % NUM_RECURSOS; // sorteia qual o recurso que precisa
            printf("[BLOCK] Processo %d bloqueou esperando %s no tempo %d\n",
            p->pid, qual_recurso(recurso), tempo_sistema + tempo_exec);
            p->entrou_bloqueado = tempo_sistema + tempo_exec;
            inserir_na_fila(fila_recurso[recurso], p);
        }
        else
        {
            printf("Processo %d voltou para PRONTO\n", p->pid);
            p->entrou_pronto = tempo_sistema + tempo_exec;
            inserir_na_fila(pronto, p);
        }
    }
    return tempo_exec;
}

void verificar_desbloqueio(fila *fila_recurso[], fila *pronto, int tempo_sistema)
{
    int total_bloqueados = 0;

    for(int i = 0; i < NUM_RECURSOS; i++)
        total_bloqueados += fila_recurso[i]->qtde;

    for(int i = 0; i < NUM_RECURSOS; i++)
    {
        int tamanho = fila_recurso[i]->qtde;

        for(int j = 0; j < tamanho; j++)
        {
            processo *p = remover_da_fila(fila_recurso[i]);

            // só pode desbloquear no proximo "ciclo"
            if(tempo_sistema - p->entrou_bloqueado >= 10)
            {
                int desbloquear = 0;

                // se for o único bloqueado desbloqueia direto
                if(total_bloqueados == 1)
                    desbloquear = 1;
                else
                {
                    int sorteio = rand() % 100;
                    if(sorteio < 40)
                        desbloquear = 1;
                }

                if(desbloquear)
                {
                    printf("[UNBLOCK] Processo %d desbloqueou do recurso %s no tempo %d\n",
                           p->pid, qual_recurso(i), tempo_sistema);

                    p->tempo_bloqueado += tempo_sistema - p->entrou_bloqueado;
                    p->entrou_pronto = tempo_sistema;

                    inserir_na_fila(pronto, p);
                }
                else
                {
                    inserir_na_fila(fila_recurso[i], p);
                }
            }
            else
            {
                inserir_na_fila(fila_recurso[i], p);
            }
        }
    }
}