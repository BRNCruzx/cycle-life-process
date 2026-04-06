#ifndef DASHBOARD_H
#define DASHBOARD_H



// ─── Cores ANSI ───────────────────────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[97m"
#define GRAY    "\033[90m"

#define BG_DARK "\033[40m"

// ─── Limpa tela e move cursor para (0,0) ──────────────────────────────────────
static void dashboard_clear()
{
    printf("\033[2J\033[H");
}

// ─── Linha horizontal ─────────────────────────────────────────────────────────
static void dash_linha(int largura, const char *cor)
{
    printf("%s", cor);
    for (int i = 0; i < largura; i++) printf("─");
    printf(RESET "\n");
}

// ─── Cabeçalho ────────────────────────────────────────────────────────────────
static void dash_header(int tempo_sistema)
{
    printf(BOLD CYAN);
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║       ⬡  SIMULADOR DE CICLO DE VIDA DE PROCESSOS    ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    printf(RESET);
    printf(GRAY "  Tempo do sistema: " RESET BOLD WHITE "%-6d UT\n" RESET, tempo_sistema);
    printf("\n");
}

// ─── Exibe uma fila no terminal ───────────────────────────────────────────────
static void dash_fila(
    const char *titulo,
    const char *cor,
    fila *f,
    int tempo_sistema)
{
    printf("%s%s▸ %-20s" RESET GRAY " [%d proc]\n" RESET,
           BOLD, cor, titulo, f->qtde);

    if (fila_vazia(f->qtde))
    {
        printf(GRAY "    (vazia)\n" RESET);
        return;
    }

    processo *aux = f->inicio;
    while (aux != NULL)
    {
        int espera_atual = aux->tempo_espera;
        if (f->finalidade == 0) // PRONTO
            espera_atual += tempo_sistema - aux->entrou_pronto;

        printf("    %s● P%-3d" RESET, cor, aux->pid);

        if (aux->pid_pai != -1)
            printf(GRAY "(filho de P%d) " RESET, aux->pid_pai);

        printf("total:%-4d exec:%-4d rest:%-4d espera:%-4d bloq:%-4d",
               aux->tempo_total,
               aux->tempo_executado,
               aux->tempo_total - aux->tempo_executado,
               espera_atual,
               aux->tempo_bloqueado);

        if (aux->possui_filho)
            printf(MAGENTA " [tem filhos]" RESET);

        printf("\n");
        aux = aux->prox;
    }
}

// ─── Exibe filas de recurso ───────────────────────────────────────────────────
static void dash_recursos(fila *fila_recurso[], int tempo_sistema)
{
    const char *nomes[] = {"HD", "TECLADO", "MOUSE"};

    printf(BOLD RED "▸ BLOQUEADOS POR RECURSO\n" RESET);

    for (int i = 0; i < NUM_RECURSOS; i++)
    {
        printf(GRAY "  ├─ %-10s" RESET GRAY " [%d]\n" RESET,
               nomes[i], fila_recurso[i]->qtde);

        processo *aux = fila_recurso[i]->inicio;
        while (aux != NULL)
        {
            int bloq_atual = (aux->entrou_bloqueado >= 0)
                             ? tempo_sistema - aux->entrou_bloqueado
                             : 0;

            printf(RED "  │    ● P%-3d" RESET
                   " bloqueado desde T=%-4d  há %d UT\n",
                   aux->pid,
                   aux->entrou_bloqueado,
                   bloq_atual);

            aux = aux->prox;
        }

        if (fila_vazia(fila_recurso[i]->qtde))
            printf(GRAY "  │    (vazia)\n" RESET);
    }
    printf("\n");
}

// ─── Exibe fila de espera filho ───────────────────────────────────────────────
static void dash_espera_filho(fila *f_filho, int tempo_sistema)
{
    printf(BOLD MAGENTA "▸ %-20s" RESET GRAY " [%d proc]\n" RESET,
           "WAIT (filho)", f_filho->qtde);

    if (fila_vazia(f_filho->qtde))
    {
        printf(GRAY "    (vazia)\n" RESET);
        return;
    }

    processo *aux = f_filho->inicio;
    while (aux != NULL)
    {
        int bloq_atual = (aux->entrou_bloqueado >= 0)
                         ? tempo_sistema - aux->entrou_bloqueado
                         : 0;
        printf(MAGENTA "    ● P%-3d" RESET
               " aguardando filho terminar  bloqueado há %d UT\n",
               aux->pid, bloq_atual);
        aux = aux->prox;
    }
}

// ─── Barra de progresso para processo em execução ────────────────────────────
static void dash_barra_progresso(processo *p)
{
    if (p == NULL) return;

    int barra = 30;
    int feito = (p->tempo_total > 0)
                ? (p->tempo_executado * barra) / p->tempo_total
                : 0;
    if (feito > barra) feito = barra;

    printf(BOLD YELLOW "    P%-3d  [" RESET, p->pid);
    for (int i = 0; i < barra; i++)
        printf("%s", i < feito ? YELLOW "█" RESET : GRAY "░" RESET);
    printf(YELLOW BOLD "]" RESET " %d/%d UT\n" RESET,
           p->tempo_executado, p->tempo_total);
}

// ─── Métricas rápidas ─────────────────────────────────────────────────────────
static void dash_metricas(
    int proc_finalizados,
    int proc_exec_prontos,
    int qtde_bloqueados,
    float tempo_medio_bloqueio)
{
    printf(BOLD WHITE "\n── MÉTRICAS ─────────────────────────────────────────\n" RESET);
    printf(GREEN  "  Finalizados       : %d\n" RESET, proc_finalizados);
    printf(YELLOW "  Exec→Pronto       : %d\n" RESET, proc_exec_prontos);
    printf(RED    "  Total bloqueados  : %d\n" RESET, qtde_bloqueados);
    printf(CYAN   "  Tempo médio bloq  : %.1f UT\n" RESET, tempo_medio_bloqueio);
    printf(BOLD WHITE "─────────────────────────────────────────────────────\n" RESET);
}

// ─── FUNÇÃO PRINCIPAL 
static void dashboard_exibir(
    fila *pronto,
    fila *execucao,
    fila *fila_recurso[],
    fila *espera_filho,
    fila *finalizados,
    int   tempo_sistema,
    int   proc_finalizados,
    int   proc_exec_prontos,
    int   qtde_bloqueados,
    float tempo_medio_bloqueio)
{
    dashboard_clear();
    dash_header(tempo_sistema);

    // Fila de pronto
    dash_fila("PRONTO",      BLUE,    pronto,       tempo_sistema);
    printf("\n");

    // Execução com barra de progresso
    dash_fila("EXECUÇÃO",    YELLOW,  execucao,     tempo_sistema);
    if (!fila_vazia(execucao->qtde))
        dash_barra_progresso(execucao->inicio);
    printf("\n");

    // Recursos bloqueados
    dash_recursos(fila_recurso, tempo_sistema);

    // Wait filho
    dash_espera_filho(espera_filho, tempo_sistema);
    printf("\n");

    // Finalizados (só contagem pra não poluir)
    printf(BOLD GREEN "▸ FINALIZADOS" RESET GRAY "             [%d proc]\n" RESET,
           finalizados->qtde);
    printf("\n");

    // Métricas
    dash_metricas(proc_finalizados, proc_exec_prontos,
                  qtde_bloqueados, tempo_medio_bloqueio);

    printf(GRAY "\n  Pressione ESC para encerrar...\n" RESET);
    fflush(stdout);
}

#endif // DASHBOARD_H
