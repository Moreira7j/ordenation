#include <stdio.h>
#include <stdlib.h>

// Intercala as metades ordenadas v[ini..meio] e v[meio+1..fim]
void intercalar(int *v, int *aux, int ini, int meio, int fim) {
    int i = ini, j = meio + 1, k = ini;

    // pega sempre o menor entre as duas metades
    while (i <= meio && j <= fim) {
        if (v[i] <= v[j]) {     // <= mantém a ordenação estável
            aux[k++] = v[i++];
        } else {
            aux[k++] = v[j++];
        }
    }

    // copia o que sobrou de uma das metades
    while (i <= meio) aux[k++] = v[i++];
    while (j <= fim)  aux[k++] = v[j++];

    // devolve o trecho intercalado para v
    for (k = ini; k <= fim; k++) {
        v[k] = aux[k];
    }
}

// Divide ao meio, ordena cada metade e intercala
void mergeSortRec(int *v, int *aux, int ini, int fim) {
    int meio;

    if (ini >= fim) return;     // 0 ou 1 elemento: já ordenado

    meio = ini + (fim - ini) / 2;
    mergeSortRec(v, aux, ini, meio);
    mergeSortRec(v, aux, meio + 1, fim);
    intercalar(v, aux, ini, meio, fim);
}

void mergeSort(int *v, int n) {
    // vetor auxiliar alocado uma única vez: espaço extra O(n)
    int *aux = malloc(n * sizeof(int));
    if (aux == NULL) return;

    mergeSortRec(v, aux, 0, n - 1);
    free(aux);
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {7, 3, 9, 1, 5, 3, 8, 2};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    mergeSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
