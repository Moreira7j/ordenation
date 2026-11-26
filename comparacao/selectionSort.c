#include <stdio.h>

void selectionSort(int *v, int n) {
    int i, j, menor, temp;

    // n - 1 passadas: o último elemento restante já fica no lugar
    for (i = 0; i < n - 1; i++) {
        // procura a posição do menor elemento em v[i..n-1]
        menor = i;
        for (j = i + 1; j < n; j++) {
            if (v[j] < v[menor]) {
                menor = j;
            }
        }

        // coloca o menor na posição i (uma troca por passada)
        temp = v[i];
        v[i] = v[menor];
        v[menor] = temp;
    }
}

// Teste simples
int main() {
    int v[] = {9, 8, 7, 6, 5, 4, 3, 12};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    selectionSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
