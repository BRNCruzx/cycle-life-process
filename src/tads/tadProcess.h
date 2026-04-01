// ---------------------TAD--------------------- //

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
