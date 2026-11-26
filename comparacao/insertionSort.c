#include <stdio.h>

void insertionSort(int *v, int n) {
    int i, j, chave;

    // v[0..i-1] já está ordenado; encaixa v[i] nessa parte
    for (i = 1; i < n; i++) {
        chave = v[i];
        j = i - 1;

        // desloca para a direita os maiores que a chave
        while (j >= 0 && v[j] > chave) {
            v[j + 1] = v[j];
            j--;
        }

        // encaixa a chave no espaço aberto
        v[j + 1] = chave;
    }
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {7, 3, 9, 1, 5, 3, 8, 2};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    insertionSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
