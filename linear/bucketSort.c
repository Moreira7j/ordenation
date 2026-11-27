#include <stdio.h>
#include <stdlib.h>

// Bucket Sort: distribui os valores em n baldes por faixa de valor,
// ordena cada balde com Insertion Sort e concatena.
// Eficiente quando os valores são distribuídos de forma uniforme.
//
// Recebe os vetores de trabalho já alocados (balde, pos e aux com n
// posições; inicio com n + 1). O benchmark usa esta versão para não
// medir a alocação.
void bucketSortAux(int *v, int n, int *balde, int *inicio, int *pos, int *aux) {
    int i, j, b, chave, min, max;
    long long faixa;

    if (n <= 1) return;

    // menor e maior valor definem a faixa coberta pelos baldes
    min = max = v[0];
    for (i = 1; i < n; i++) {
        if (v[i] < min) min = v[i];
        if (v[i] > max) max = v[i];
    }
    faixa = (long long)max - min + 1;

    // zera o tamanho dos baldes
    for (b = 0; b <= n; b++) {
        inicio[b] = 0;
    }

    // 1) calcula o balde de cada valor e conta o tamanho dos baldes
    for (i = 0; i < n; i++) {
        balde[i] = (int)(((long long)v[i] - min) * n / faixa);
        inicio[balde[i] + 1]++;
    }

    // 2) soma acumulada: baldes ficam lado a lado em aux
    for (b = 0; b < n; b++) {
        inicio[b + 1] += inicio[b];
        pos[b] = inicio[b];
    }

    // 3) distribui os elementos nos baldes
    for (i = 0; i < n; i++) {
        aux[pos[balde[i]]++] = v[i];
    }

    // 4) ordena cada balde com Insertion Sort (baldes são pequenos)
    for (b = 0; b < n; b++) {
        for (i = inicio[b] + 1; i < inicio[b + 1]; i++) {
            chave = aux[i];
            j = i - 1;
            while (j >= inicio[b] && aux[j] > chave) {
                aux[j + 1] = aux[j];
                j--;
            }
            aux[j + 1] = chave;
        }
    }

    // 5) baldes em ordem = vetor ordenado
    for (i = 0; i < n; i++) {
        v[i] = aux[i];
    }
}

// Versão de uso geral: aloca os vetores de trabalho sozinha
void bucketSort(int *v, int n) {
    int *balde  = malloc(n * sizeof(int));          // balde de cada elemento
    int *inicio = malloc((n + 1) * sizeof(int));    // onde cada balde começa
    int *pos    = malloc(n * sizeof(int));          // próxima posição livre
    int *aux    = malloc(n * sizeof(int));

    if (balde && inicio && pos && aux) {
        bucketSortAux(v, n, balde, inicio, pos, aux);
    }

    free(balde); free(inicio); free(pos); free(aux);
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {42, 32, 33, 52, 37, 47, 51, 3};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    bucketSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
