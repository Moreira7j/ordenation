#include <stdio.h>

// Faz v[i] "descer" no heap até que o pai seja maior que os filhos
// (heap de tamanho n; filhos de i ficam em 2i+1 e 2i+2)
void descer(int *v, int n, int i) {
    int maior, esq, dir, temp;

    while (1) {
        maior = i;
        esq = 2 * i + 1;
        dir = 2 * i + 2;

        if (esq < n && v[esq] > v[maior]) maior = esq;
        if (dir < n && v[dir] > v[maior]) maior = dir;

        if (maior == i) break;  // propriedade de heap restaurada

        temp = v[i];
        v[i] = v[maior];
        v[maior] = temp;
        i = maior;
    }
}

void heapSort(int *v, int n) {
    int i, temp;

    // 1) monta o max-heap: ajusta cada nó interno, de baixo para cima
    for (i = n / 2 - 1; i >= 0; i--) {
        descer(v, n, i);
    }

    // 2) move o maior (raiz) para o final e reajusta o heap restante
    for (i = n - 1; i > 0; i--) {
        temp = v[0];
        v[0] = v[i];
        v[i] = temp;
        descer(v, i, 0);
    }
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {7, 3, 9, 1, 5, 3, 8, 2};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    heapSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
