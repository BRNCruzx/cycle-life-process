struct fila
{
    processo *inicio;
    processo *fim;
    int qtde;
    int finalidade; // 0 = Pronto   1 = Espera   2 = Execuçao
};
