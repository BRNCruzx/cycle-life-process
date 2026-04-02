#include <stdio.h>
#include <stdlib.h>

#define MAX_TEMPO_CPU 10

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
    int tempo_bloqueado;
    int tempo_espera;
    int criou_filho; // 1 = sim e 0 = nao
    int esperando_filho; // 1 = sim e 0 = nao
    int possui_filho; // 1 = sim e 0 = nao
    int terminou;
    filho *lista_filhos;
    struct processo *prox;
};

struct fila
{
    processo *inicio;
    processo *fim;
    int qtde;
    int finalidade; // 0 = Pronto   1 = Espera   2 = Execuçao
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
processo *criar_processo(int pid, int tempo_total) 
{
    processo *novo = new processo;
    
    novo->pid = pid;
    novo->tempo_total = tempo_total;
    novo->tempo_executado = 0;
    novo->tempo_bloqueado = 0;
    novo->tempo_espera = 0;
    novo->criou_filho = 0;
    novo->esperando_filho = 0;
    novo->possui_filho = 0;
    novo->lista_filhos = NULL;
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

void exibir_fila(fila *f)
{
    processo *aux = f->inicio;

    if(fila_vazia(f->qtde))
    {
        printf("\nFila vazia.\n");
        return;
    }

    printf("\nFila (finalidade = %d):\n", f->finalidade);

    while(aux != NULL)
    {
        printf("PID: %d | Tempo Total: %d | Executado: %d\n", aux->pid, aux->tempo_total, aux->tempo_executado);
        aux = aux->prox;
    }
}
