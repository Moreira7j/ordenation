#include <stdio.h>
#include <stdlib.h>

// Counting Sort estável pelo dígito (v / exp) % 10
void ordenarPorDigito(int *v, int *aux, int n, long long exp) {
    int i, cont[10] = {0};

    // conta quantos números têm cada dígito (0 a 9)
    for (i = 0; i < n; i++) {
        cont[(v[i] / exp) % 10]++;
    }

    // soma acumulada: cont[d] = posição final do último com dígito d
    for (i = 1; i < 10; i++) {
        cont[i] += cont[i - 1];
    }

    // percorre de trás para frente para manter a estabilidade
    for (i = n - 1; i >= 0; i--) {
        aux[--cont[(v[i] / exp) % 10]] = v[i];
    }

    for (i = 0; i < n; i++) {
        v[i] = aux[i];
    }
}

// Radix Sort LSD (base 10): ordena dígito a dígito, do menos para o
// mais significativo. Requer inteiros não negativos.
void radixSort(int *v, int n) {
    int i, max;
    long long exp;
    int *aux;

    if (n <= 1) return;

    // o maior valor define quantos dígitos processar
    max = v[0];
    for (i = 1; i < n; i++) {
        if (v[i] > max) max = v[i];
    }

    aux = malloc(n * sizeof(int));
    if (aux == NULL) return;

    // exp = 1, 10, 100, ... (unidade, dezena, centena, ...)
    for (exp = 1; max / exp > 0; exp *= 10) {
        ordenarPorDigito(v, aux, n, exp);
    }

    free(aux);
}

#ifndef BENCHMARK
// Teste simples
int main() {
    int v[] = {170, 45, 75, 90, 802, 24, 2, 66};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    radixSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
#endif
