# Análise Comparativa de Algoritmos de Ordenação

Implementação em C de 9 algoritmos de ordenação e de um benchmark que mede o
tempo de execução de cada um para diferentes tamanhos de entrada.

## Estrutura

```
comparacao/   algoritmos baseados em comparação
  bubbleSort.c  selectionSort.c  insertionSort.c   -> O(n²)
  mergeSort.c   quickSort.c      heapSort.c        -> O(n log n)
linear/       algoritmos não baseados em comparação
  countingSort.c  radixSort.c  bucketSort.c
benchmark.c        programa que executa o experimento
gerar_graficos.py  gera os gráficos a partir dos CSVs
charts/            gráficos gerados (PNG)
```

Cada arquivo de algoritmo contém a função de ordenação e uma `main` de teste.

## Requisitos

- Compilador GCC (testado com gcc 15.2.0, MSYS2 UCRT64, Windows 11)
- Python 3 com pandas e matplotlib, apenas para os gráficos
  (`pip install pandas matplotlib`)

## Testar um algoritmo isoladamente

```
gcc -Wall -O2 comparacao/bubbleSort.c -o bubble
./bubble
```

## Executar o benchmark

```
gcc -O2 benchmark.c -o benchmark
./benchmark            # experimento completo (pode levar dezenas de minutos)
./benchmark rapido     # apenas N <= 10.000, para teste
```

No PowerShell, use `.\benchmark` em vez de `./benchmark`.

O programa imprime as tabelas de tempos (em ms) e gera:

- `resultados.csv`: tempo x tamanho do vetor (N)
- `resultados_k.csv`: tempo x amplitude dos valores (K), com N fixo

## Gerar os gráficos

Depois de rodar o benchmark, execute:

```
python gerar_graficos.py
```

O script lê `resultados.csv` e `resultados_k.csv` e salva os gráficos em
`charts/`, substituindo os anteriores. Sempre que o benchmark for executado
novamente, rode o script de novo para atualizar os gráficos.

| Arquivo | Conteúdo |
|---|---|
| `tempo_x_n_loglog.png` | os 9 algoritmos, tempo x N em escala log-log |
| `tempo_x_n_familias.png` | um painel por família, em escala linear |
| `ponto_de_virada.png` | Insertion x Quick x Merge com N pequeno |
| `barras_n_100000.png` | ranking dos algoritmos com N = 100.000 |
| `tempo_x_k.png` | tempo x amplitude dos valores K, com N fixo |

## Metodologia

- Vetores de inteiros aleatórios em [0, 1.000.000], gerados com semente fixa
  (xorshift32), garantindo reprodutibilidade.
- Para cada N é gerado um único vetor base, copiado de forma idêntica para
  cada algoritmo.
- O cronômetro (`QueryPerformanceCounter`) mede somente a chamada da
  ordenação; geração, cópia e verificação ficam fora da medição.
- Os vetores auxiliares de Merge, Counting, Radix e Bucket Sort são alocados
  uma única vez, antes de qualquer medição, e passados prontos para as
  funções (versões `...Aux`). Assim o tempo de alocação de memória não entra
  no resultado.
- Cada medição é repetida no mínimo 3 vezes (até 10 nos casos rápidos) e é
  registrada a média.
- Todo resultado é comparado com a saída do `qsort` da biblioteca padrão.
- Algoritmos O(n²) são executados até N = 500.000.

