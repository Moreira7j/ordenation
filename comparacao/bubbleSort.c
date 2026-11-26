
#include <stdio.h>

void bubbleSort(int *v, int n) {
    int i, j, temp;

    // n - 1 passadas: o último elemento restante já fica no lugar
    for (i = 0; i < n - 1; i++) {
        // os i últimos elementos já estão ordenados
        for (j = 0; j < n - 1 - i; j++) {
            if (v[j] > v[j + 1]) {
                // troca os vizinhos
                temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
        }
    }
}

// Teste simples
int main() {
    int v[] = {9, 8, 7, 6, 5, 4, 3, 12};
    int n = sizeof(v) / sizeof(v[0]);
    int i;

    bubbleSort(v, n);

    for (i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    return 0;
}
