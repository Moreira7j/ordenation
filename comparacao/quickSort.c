#include <stdio.h>

void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Particiona v[ini..fim] (Lomuto): menores que o pivô à esquerda,
// maiores à direita. Retorna a posição final do pivô.
int particionar(int *v, int ini, int fim) {
    int i, j, pivo;

    // usa o elemento do meio como pivô (evita o pior caso em vetor já ordenado)
    trocar(&v[ini + (fim - ini) / 2], &v[fim]);
    pivo = v[fim];

    i = ini - 1;                // fim da região dos menores que o pivô
    for (j = ini; j < fim; j++) {
        if (v[j] < pivo) {
            i++;
            trocar(&v[i], &v[j]);
        }
    }

    // coloca o pivô logo após os menores
    trocar(&v[i + 1], &v[fim]);
    return i + 1;
}

void quickSortRec(int *v, int ini, int fim) {
    int p;

    if (ini >= fim) return;     // 0 ou 1 elemento: já ordenado

    p = particionar(v, ini, fim);
    quickSortRec(v, ini, p - 1);    // ordena os menores
    quickSortRec(v, p + 1, fim);    // ordena os maiores
}

void quickSort(int *v, int n) {
    quickSortRec(v, 0, n - 1);
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {7, 3, 9, 1, 5, 3, 8, 2};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    quickSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
