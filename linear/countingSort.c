#include <stdio.h>
#include <stdlib.h>

// Counting Sort: conta as ocorrências de cada valor e usa as contagens
// para posicionar os elementos. Tempo O(n + k), espaço O(n + k),
// onde k = max - min + 1 (amplitude dos valores).
void countingSort(int *v, int n) {
    int i, min, max, k;
    int *cont, *aux;

    if (n <= 1) return;

    // a amplitude k define o tamanho do vetor de contagem
    min = max = v[0];
    for (i = 1; i < n; i++) {
        if (v[i] < min) min = v[i];
        if (v[i] > max) max = v[i];
    }
    k = max - min + 1;

    cont = calloc(k, sizeof(int));
    aux  = malloc(n * sizeof(int));
    if (cont == NULL || aux == NULL) {
        free(cont); free(aux);
        return;
    }

    // conta quantas vezes cada valor aparece
    for (i = 0; i < n; i++) {
        cont[v[i] - min]++;
    }

    // soma acumulada: cont[x] = quantos elementos são <= x
    for (i = 1; i < k; i++) {
        cont[i] += cont[i - 1];
    }

    // posiciona de trás para frente para manter a estabilidade
    for (i = n - 1; i >= 0; i--) {
        aux[--cont[v[i] - min]] = v[i];
    }

    for (i = 0; i < n; i++) {
        v[i] = aux[i];
    }

    free(cont);
    free(aux);
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {4, 2, 2, 8, 3, 3, 1, 7};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    countingSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
