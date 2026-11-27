/*
 * Benchmark dos algoritmos de ordenação
 *
 * Mede quanto tempo cada algoritmo leva para ordenar o mesmo vetor aleatório.
 *   Experimento 1: varia o tamanho do vetor (N)
 *   Experimento 2: varia a amplitude dos valores (K), com N fixo
 *
 * Compilar:  gcc -O2 benchmark.c -o benchmark
 * Executar:  .\benchmark          (completo, demora bastante)
 *            .\benchmark rapido   (só vetores pequenos, para testar)
 */

// Desliga a main de teste de cada arquivo, para só usarmos as funções
#define BENCHMARK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include "comparacao/bubbleSort.c"
#include "comparacao/selectionSort.c"
#include "comparacao/insertionSort.c"
#include "comparacao/mergeSort.c"
#include "comparacao/quickSort.c"
#include "comparacao/heapSort.c"
#include "linear/countingSort.c"
#include "linear/radixSort.c"
#include "linear/bucketSort.c"

/* Memória de trabalho */

// Merge, Counting, Radix e Bucket precisam de vetores auxiliares. Eles são
// alocados uma vez só, antes de qualquer medição, e passados prontos para
// as funções. Assim o tempo de alocação não entra no resultado.
static int *trabAux, *trabCont, *trabBalde, *trabInicio, *trabPos;

static void mergeSortBench(int *v, int n)    { mergeSortAux(v, n, trabAux); }
static void countingSortBench(int *v, int n) { countingSortAux(v, n, trabCont, trabAux); }
static void radixSortBench(int *v, int n)    { radixSortAux(v, n, trabAux); }
static void bucketSortBench(int *v, int n) {
    bucketSortAux(v, n, trabBalde, trabInicio, trabPos, trabAux);
}

// maxN: maior vetor testado; maxK: maior amplitude de valores
static int alocarTrabalho(int maxN, int maxK) {
    trabAux    = malloc((size_t)maxN * sizeof(int));
    trabBalde  = malloc((size_t)maxN * sizeof(int));
    trabPos    = malloc((size_t)maxN * sizeof(int));
    trabInicio = malloc(((size_t)maxN + 1) * sizeof(int));
    trabCont   = malloc((size_t)maxK * sizeof(int));
    if (!trabAux || !trabBalde || !trabPos || !trabInicio || !trabCont) return 0;

    // escreve em tudo uma vez, para o sistema já entregar a memória de
    // verdade agora, e não durante a primeira medição
    memset(trabAux,    0, (size_t)maxN * sizeof(int));
    memset(trabBalde,  0, (size_t)maxN * sizeof(int));
    memset(trabPos,    0, (size_t)maxN * sizeof(int));
    memset(trabInicio, 0, ((size_t)maxN + 1) * sizeof(int));
    memset(trabCont,   0, (size_t)maxK * sizeof(int));
    return 1;
}

static void liberarTrabalho(void) {
    free(trabAux); free(trabBalde); free(trabPos); free(trabInicio); free(trabCont);
}

/* Configuração do experimento */

#define SEMENTE            12345      // semente fixa: o vetor sai sempre igual
#define MIN_REPETICOES     3          // mínimo pedido pelo professor
#define MAX_REPETICOES     10         // os rápidos repetem mais, para a média ficar estável
#define TEMPO_ALVO_MS      500.0      // para de repetir ao somar esse tempo
#define VALOR_MAX          1000000    // números sorteados entre 0 e 1.000.000
#define LIMITE_QUADRATICO  500000     // acima disso os O(n²) levariam horas
#define ELEMENTOS_POR_LOTE 100000     // usado para medir vetores pequenos (ver medir)

// Os tamanhos pequenos (10 a 100) servem para achar o ponto de virada
static const int TAMANHOS[] = {10, 50, 100, 1000, 10000, 50000, 100000, 500000, 1000000};
#define N_TAMANHOS ((int)(sizeof(TAMANHOS) / sizeof(TAMANHOS[0])))

// Experimento 2: valores sorteados entre 0 e K-1, sempre com o mesmo N
static const int AMPLITUDES[] = {1000, 100000, 1000000, 10000000};
#define N_AMPLITUDES ((int)(sizeof(AMPLITUDES) / sizeof(AMPLITUDES[0])))
#define N_EXPERIMENTO_K 1000000

/* Lista de algoritmos testados */

typedef struct {
    const char *nome;
    const char *categoria;
    void (*ordenar)(int *, int);   // ponteiro para a função de ordenação
    int quadratico;                // 1 = O(n²), não roda nos vetores gigantes
} Algoritmo;

static const Algoritmo ALGORITMOS[] = {
    {"Bubble Sort",    "Comparacao O(n^2)",     bubbleSort,    1},
    {"Selection Sort", "Comparacao O(n^2)",     selectionSort, 1},
    {"Insertion Sort", "Comparacao O(n^2)",     insertionSort, 1},
    {"Merge Sort",     "Comparacao O(n log n)", mergeSortBench,    0},
    {"Quick Sort",     "Comparacao O(n log n)", quickSort,         0},
    {"Heap Sort",      "Comparacao O(n log n)", heapSort,          0},
    {"Counting Sort",  "Linear O(n + k)",       countingSortBench, 0},
    {"Radix Sort",     "Linear O(d(n + b))",    radixSortBench,    0},
    {"Bucket Sort",    "Linear O(n + k)",       bucketSortBench,   0},
};
#define N_ALGORITMOS ((int)(sizeof(ALGORITMOS) / sizeof(ALGORITMOS[0])))

// Valores especiais guardados no lugar do tempo
#define PULADO -2.0     // não rodou nesse N
#define ERRO   -1.0     // não ordenou direito

/* Cronômetro */

// Hora atual em milissegundos, com precisão de microssegundos
static double agoraMs(void) {
#ifdef _WIN32
    static LARGE_INTEGER freq;
    LARGE_INTEGER t;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

/* Gerador de números aleatórios */

// Não usamos rand() porque no Windows ele só vai até 32.767.
// O xorshift32 é simples e gera a mesma sequência em qualquer computador.
static unsigned int estado;

static void semear(unsigned int s) { estado = s ? s : 1; }

static unsigned int proximo(void) {
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

// Preenche v com números entre 0 e valorMax
static void gerarVetor(int *v, int n, int valorMax) {
    int i;
    for (i = 0; i < n; i++) {
        v[i] = (int)(proximo() % ((unsigned int)valorMax + 1u));
    }
}

// Comparação usada pelo qsort, que gera a resposta certa para conferência
static int comparar(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

/* Medição */

// Mede o tempo médio (em ms) que alg leva para ordenar uma cópia de base.
//
// Vetores pequenos ordenam rápido demais para o relógio medir com precisão.
// Por isso, ordenamos várias cópias seguidas (um "lote") e dividimos o tempo
// pela quantidade. Com N grande, o lote tem uma cópia só.
static double medir(const Algoritmo *alg, const int *base, const int *ref,
                    int n, int *buffer, int lote, double *tempos, int *reps) {
    int r, b;
    double ini, fim, soma = 0.0, total = 0.0;

    // repete pelo menos 3 vezes; se for rápido, continua até 10
    for (r = 0; r < MAX_REPETICOES; r++) {
        if (r >= MIN_REPETICOES && total >= TEMPO_ALVO_MS) break;

        // prepara as cópias antes de ligar o cronômetro
        for (b = 0; b < lote; b++) {
            memcpy(buffer + (size_t)b * n, base, n * sizeof(int));
        }

        // só a ordenação entra na medição
        ini = agoraMs();
        for (b = 0; b < lote; b++) {
            alg->ordenar(buffer + (size_t)b * n, n);
        }
        fim = agoraMs();

        tempos[r] = (fim - ini) / lote;
        soma += tempos[r];
        total += fim - ini;

        // confere se ficou igual à resposta do qsort
        for (b = 0; b < lote; b++) {
            if (memcmp(buffer + (size_t)b * n, ref, n * sizeof(int)) != 0) {
                return ERRO;
            }
        }
    }
    *reps = r;
    return soma / r;
}

// Valor do meio dos tempos: não é puxada por um pico isolado, como a média
static double mediana(const double *tempos, int reps) {
    double t[MAX_REPETICOES], x;
    int i, j;
    for (i = 0; i < reps; i++) {
        x = tempos[i];
        for (j = i - 1; j >= 0 && t[j] > x; j--) t[j + 1] = t[j];
        t[j + 1] = x;
    }
    return (reps % 2) ? t[reps / 2] : (t[reps / 2 - 1] + t[reps / 2]) / 2.0;
}

static double minimo(const double *tempos, int reps) {
    double m = tempos[0];
    int i;
    for (i = 1; i < reps; i++) if (tempos[i] < m) m = tempos[i];
    return m;
}

/* Saída na tela */

// Coloca os pontos de milhar: 1000000 vira "1.000.000"
static void formatarMilhar(int x, char *s) {
    char tmp[32];
    int len, i, j = 0;
    sprintf(tmp, "%d", x);
    len = (int)strlen(tmp);
    for (i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) s[j++] = '.';
        s[j++] = tmp[i];
    }
    s[j] = '\0';
}

// Quanto menor o tempo, mais casas decimais
static void formatarTempo(double ms, char *s) {
    if (ms == PULADO)    sprintf(s, "-");
    else if (ms == ERRO) sprintf(s, "ERRO");
    else if (ms < 0.01)  sprintf(s, "%.6f", ms);
    else if (ms < 1.0)   sprintf(s, "%.4f", ms);
    else if (ms < 100.0) sprintf(s, "%.2f", ms);
    else                 sprintf(s, "%.0f", ms);
}

// Imprime uma tabela: uma linha por algoritmo, uma coluna por N (ou K)
static void imprimirTabela(const char *titulo, const char *rotulo,
                           const int *colunas, int nColunas,
                           const int *linhas, int nLinhas, double *res) {
    int l, c;
    char buf[32], cab[48];

    printf("\n%s (tempo medio, em ms)\n", titulo);
    printf("%-16s", "Algoritmo");
    for (c = 0; c < nColunas; c++) {
        formatarMilhar(colunas[c], buf);
        sprintf(cab, "%s=%s", rotulo, buf);
        printf(" %12s", cab);
    }
    printf("\n");

    for (l = 0; l < nLinhas; l++) {
        printf("%-16s", ALGORITMOS[linhas[l]].nome);
        for (c = 0; c < nColunas; c++) {
            formatarTempo(res[l * nColunas + c], buf);
            printf(" %12s", buf);
        }
        printf("\n");
    }
}

/* Programa principal */

int main(int argc, char *argv[]) {
    int rapido = (argc > 1 && strcmp(argv[1], "rapido") == 0);
    // pula os O(n²): útil para medir de novo só os rápidos
    int semQuadraticos = (argc > 1 && strcmp(argv[1], "sem_quadraticos") == 0);
    int nTam = N_TAMANHOS, nK = rapido ? 100000 : N_EXPERIMENTO_K;
    int t, a, k, lote, n, nLin;
    int todos[N_ALGORITMOS], lineares[N_ALGORITMOS];
    double res1[N_ALGORITMOS * N_TAMANHOS];
    double res2[N_ALGORITMOS * N_AMPLITUDES];
    double tempos[MAX_REPETICOES], m;
    int reps;
    int *base, *ref, *buffer;
    FILE *csv;

    // no modo rápido, só os tamanhos até 10.000
    if (rapido) {
        nTam = 0;
        while (nTam < N_TAMANHOS && TAMANHOS[nTam] <= 10000) nTam++;
    }

    // tamanho da memória de trabalho: maior N e maior amplitude usados
    {
        int maxN = N_EXPERIMENTO_K, maxK = VALOR_MAX + 1;
        for (t = 0; t < N_TAMANHOS; t++) if (TAMANHOS[t] > maxN) maxN = TAMANHOS[t];
        for (k = 0; k < N_AMPLITUDES; k++) if (AMPLITUDES[k] > maxK) maxK = AMPLITUDES[k];
        if (!alocarTrabalho(maxN, maxK)) {
            fprintf(stderr, "Sem memoria para os vetores de trabalho\n");
            return 1;
        }
    }

    printf("Benchmark de algoritmos de ordenacao\n");
    printf("Compilador: gcc %s | semente: %d | repeticoes: %d a %d\n",
           __VERSION__, SEMENTE, MIN_REPETICOES, MAX_REPETICOES);

    /* Experimento 1: tempo conforme o tamanho N */

    csv = fopen("resultados.csv", "w");
    if (csv) fprintf(csv, "algoritmo,categoria,n,media_ms,mediana_ms,minimo_ms,repeticoes\n");

    // o progresso vai para stderr, a tabela final para stdout
    fprintf(stderr, "\n[Experimento 1] valores em [0, %d]\n", VALOR_MAX);
    for (t = 0; t < nTam; t++) {
        n = TAMANHOS[t];
        lote = ELEMENTOS_POR_LOTE / n;
        if (lote < 1) lote = 1;

        base   = malloc(n * sizeof(int));
        ref    = malloc(n * sizeof(int));
        buffer = malloc((size_t)lote * n * sizeof(int));
        if (!base || !ref || !buffer) {
            fprintf(stderr, "Sem memoria para N = %d\n", n);
            return 1;
        }

        // um vetor base para todos; ref é ele ordenado pelo qsort
        semear(SEMENTE);
        gerarVetor(base, n, VALOR_MAX);
        memcpy(ref, base, n * sizeof(int));
        qsort(ref, n, sizeof(int), comparar);

        for (a = 0; a < N_ALGORITMOS; a++) {
            if (ALGORITMOS[a].quadratico && (n > LIMITE_QUADRATICO || semQuadraticos)) {
                res1[a * N_TAMANHOS + t] = PULADO;
                continue;
            }
            fprintf(stderr, "  N = %-8d %-15s ... ", n, ALGORITMOS[a].nome);
            m = medir(&ALGORITMOS[a], base, ref, n, buffer, lote, tempos, &reps);
            res1[a * N_TAMANHOS + t] = m;

            if (m == ERRO) {
                fprintf(stderr, "ERRO: resultado incorreto!\n");
            } else {
                fprintf(stderr, "%.6f ms (%d exec.)\n", m, reps);
                if (csv) fprintf(csv, "%s,%s,%d,%.8f,%.8f,%.8f,%d\n",
                                 ALGORITMOS[a].nome, ALGORITMOS[a].categoria, n, m,
                                 mediana(tempos, reps), minimo(tempos, reps), reps);
            }
        }
        free(base); free(ref); free(buffer);
    }
    if (csv) fclose(csv);

    // no modo rápido sobram colunas vazias; junta só as que rodaram
    for (a = 0; a < N_ALGORITMOS; a++) {
        todos[a] = a;
        for (t = 0; t < nTam; t++) res1[a * nTam + t] = res1[a * N_TAMANHOS + t];
    }
    imprimirTabela("Experimento 1: tempo x tamanho do vetor", "N",
                   TAMANHOS, nTam, todos, N_ALGORITMOS, res1);

    /* Experimento 2: tempo conforme a amplitude K */

    csv = fopen("resultados_k.csv", "w");
    if (csv) fprintf(csv, "algoritmo,categoria,n,k,media_ms,mediana_ms,minimo_ms,repeticoes\n");

    // os O(n²) ficam de fora: com N = 1.000.000 levariam horas
    nLin = 0;
    for (a = 0; a < N_ALGORITMOS; a++) {
        if (!ALGORITMOS[a].quadratico) lineares[nLin++] = a;
    }

    base   = malloc(nK * sizeof(int));
    ref    = malloc(nK * sizeof(int));
    buffer = malloc(nK * sizeof(int));
    if (!base || !ref || !buffer) {
        fprintf(stderr, "Sem memoria para o experimento 2\n");
        return 1;
    }

    fprintf(stderr, "\n[Experimento 2] N = %d, variando a amplitude K\n", nK);
    for (k = 0; k < N_AMPLITUDES; k++) {
        // mesmo N, mas números sorteados entre 0 e K-1
        semear(SEMENTE);
        gerarVetor(base, nK, AMPLITUDES[k] - 1);
        memcpy(ref, base, nK * sizeof(int));
        qsort(ref, nK, sizeof(int), comparar);

        for (a = 0; a < nLin; a++) {
            const Algoritmo *alg = &ALGORITMOS[lineares[a]];
            fprintf(stderr, "  K = %-9d %-15s ... ", AMPLITUDES[k], alg->nome);
            m = medir(alg, base, ref, nK, buffer, 1, tempos, &reps);
            res2[a * N_AMPLITUDES + k] = m;

            if (m == ERRO) {
                fprintf(stderr, "ERRO: resultado incorreto!\n");
            } else {
                fprintf(stderr, "%.6f ms (%d exec.)\n", m, reps);
                if (csv) fprintf(csv, "%s,%s,%d,%d,%.8f,%.8f,%.8f,%d\n",
                                 alg->nome, alg->categoria, nK, AMPLITUDES[k], m,
                                 mediana(tempos, reps), minimo(tempos, reps), reps);
            }
        }
    }
    free(base); free(ref); free(buffer);
    if (csv) fclose(csv);

    printf("\n(N = %d)", nK);
    imprimirTabela("Experimento 2: tempo x amplitude dos valores", "K",
                   AMPLITUDES, N_AMPLITUDES, lineares, nLin, res2);

    liberarTrabalho();
    printf("\nResultados salvos em resultados.csv e resultados_k.csv\n");
    return 0;
}
