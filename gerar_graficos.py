"""
Gera os gráficos do relatório a partir dos CSVs do benchmark.

Entrada:  resultados.csv   (tempo x N)
          resultados_k.csv (tempo x amplitude K)
Saída:    charts/*.png

Uso:      python gerar_graficos.py
"""

from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # gera arquivos sem abrir janela
import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import FuncFormatter, LogLocator, NullFormatter

PASTA = Path(__file__).parent
SAIDA = PASTA / "charts"

# ---------------- Estilo (paleta validada, modo claro) ----------------
# uma cor por família (3 primeiras cores da paleta, seguras para daltonismo);
# dentro da família, marcador + tipo de linha identificam o algoritmo
AZUL, LARANJA, VERDE_AGUA = "#257de0", "#f47230", "#00b894"
TINTA, TINTA_SEC, GRADE, EIXO = "#191919", "#666666", "#e7e7e7", "#c2c2c2"

FAMILIAS = {
    "Comparacao O(n^2)": ("Comparação O(n²)", LARANJA),
    "Comparacao O(n log n)": ("Comparação O(n log n)", AZUL),
    "Linear": ("Linear (não comparativo)", VERDE_AGUA),
}

# dentro de cada família, marcador + tipo de linha diferenciam os algoritmos
ESTILO_ALG = {
    "Bubble Sort": ("o", "-"),
    "Selection Sort": ("s", "--"),
    "Insertion Sort": ("^", ":"),
    "Merge Sort": ("o", "-"),
    "Quick Sort": ("s", "--"),
    "Heap Sort": ("^", ":"),
    "Counting Sort": ("o", "-"),
    "Radix Sort": ("s", "--"),
    "Bucket Sort": ("^", ":"),
}

plt.rcParams.update({
    "figure.dpi": 150,
    "savefig.dpi": 150,
    "savefig.bbox": "tight",
    "font.family": ["Segoe UI", "DejaVu Sans", "sans-serif"],
    "font.size": 10,
    "text.color": TINTA,
    "axes.labelcolor": TINTA_SEC,
    "axes.edgecolor": EIXO,
    "axes.titlecolor": TINTA,
    "axes.titlesize": 12,
    "axes.titleweight": "bold",
    "axes.titlelocation": "left",
    "axes.spines.top": False,
    "axes.spines.right": False,
    "axes.grid": True,
    "grid.color": GRADE,
    "grid.linewidth": 0.8,
    "xtick.color": TINTA_SEC,
    "ytick.color": TINTA_SEC,
    "legend.frameon": False,
    "lines.linewidth": 2,
    "lines.markersize": 6,
})


# ---------------- Formatação numérica em português ----------------
def fmt_num(x, _pos=None):
    """1000 -> '1.000'; 1.5 -> '1,5'; 0.001 -> '0,001'."""
    if x >= 1000:
        return f"{x:,.0f}".replace(",", ".")
    casas = 2 if x >= 1 else 6
    texto = f"{x:.{casas}f}".rstrip("0").rstrip(".")
    return texto.replace(".", ",")


def fmt_ms(x):
    """Rótulo de tempo com casas decimais proporcionais ao valor."""
    casas = 0 if x >= 100 else 2 if x >= 1 else 4
    texto = f"{x:,.{casas}f}".replace(",", "X").replace(".", ",").replace("X", ".")
    return f"{texto} ms"


def estilo(alg, categoria):
    """Cor da família + marcador/linha do algoritmo (igual em todos os gráficos)."""
    marcador, linha = ESTILO_ALG[alg]
    return dict(color=FAMILIAS[familia(categoria)][1], marker=marcador, linestyle=linha)


def eixo_log(ax, eixos="xy"):
    for nome in eixos:
        eixo = ax.xaxis if nome == "x" else ax.yaxis
        (ax.set_xscale if nome == "x" else ax.set_yscale)("log")
        eixo.set_major_locator(LogLocator(base=10))
        eixo.set_major_formatter(FuncFormatter(fmt_num))
        eixo.set_minor_formatter(NullFormatter())
    ax.grid(True, which="major")
    ax.grid(False, which="minor")


def familia(categoria):
    return "Linear" if categoria.startswith("Linear") else categoria


def salvar(fig, nome):
    SAIDA.mkdir(exist_ok=True)
    caminho = SAIDA / nome
    fig.savefig(caminho, facecolor="white")
    plt.close(fig)
    print(f"  {caminho.relative_to(PASTA)}")


# ---------------- Gráficos ----------------
def grafico_todos_loglog(df):
    """Os 9 algoritmos em escala log-log: compara as famílias."""
    fig, ax = plt.subplots(figsize=(9, 5.5))
    for alg, g in df.groupby("algoritmo", sort=False):
        ax.plot(g["n"], g["media_ms"], label=alg, **estilo(alg, g["categoria"].iloc[0]))

    eixo_log(ax)
    ax.set_title("Tempo de ordenação x tamanho do vetor (escala log-log)")
    ax.set_xlabel("Tamanho do vetor (N)")
    ax.set_ylabel("Tempo médio (ms)")

    # legenda agrupada por família
    handles, labels = ax.get_legend_handles_labels()
    ax.legend(handles, labels, loc="upper left", ncol=3, fontsize=9,
              title="laranja: O(n²) · azul: O(n log n) · verde: linear",
              title_fontsize=9)
    salvar(fig, "tempo_x_n_loglog.png")


def grafico_familias(df):
    """Uma área por família, escala linear: mostra o formato de cada curva."""
    fams = list(FAMILIAS)
    fig, axs = plt.subplots(1, 3, figsize=(13, 4.2))
    for ax, fam in zip(axs, fams):
        sub = df[df["categoria"].map(familia) == fam]
        sub = sub[sub["n"] >= 1000]
        for alg, g in sub.groupby("algoritmo", sort=False):
            ax.plot(g["n"], g["media_ms"], label=alg, **estilo(alg, g["categoria"].iloc[0]))
        ax.set_title(FAMILIAS[fam][0])
        ax.set_xlabel("Tamanho do vetor (N)")
        ax.xaxis.set_major_formatter(FuncFormatter(fmt_num))
        ax.yaxis.set_major_formatter(FuncFormatter(fmt_num))
        ax.tick_params(axis="x", labelrotation=30)
        ax.set_ylim(bottom=0)
        ax.legend(fontsize=9)
    axs[0].set_ylabel("Tempo médio (ms)")
    fig.suptitle("Crescimento do tempo por família (escala linear)",
                 x=0.01, ha="left", fontweight="bold", color=TINTA)
    fig.tight_layout()
    salvar(fig, "tempo_x_n_familias.png")


def grafico_ponto_de_virada(df):
    """Insertion x Merge x Quick com N pequeno: onde o simples deixa de vencer."""
    algs = ["Insertion Sort", "Quick Sort", "Merge Sort"]
    sub = df[df["algoritmo"].isin(algs) & (df["n"] <= 1000)]
    if sub["n"].nunique() < 2:
        return

    fig, ax = plt.subplots(figsize=(8, 5))
    for alg in algs:
        g = sub[sub["algoritmo"] == alg]
        ax.plot(g["n"], g["media_ms"], label=alg, **estilo(alg, g["categoria"].iloc[0]))
        # rótulo direto no fim da linha
        ult = g.iloc[-1]
        ax.annotate(alg, (ult["n"], ult["media_ms"]), xytext=(6, 0),
                    textcoords="offset points", va="center", fontsize=9,
                    color=TINTA)

    eixo_log(ax)
    ax.set_xlim(right=sub["n"].max() * 2.2)
    ax.set_title("Ponto de virada: algoritmo simples x complexo com N pequeno")
    ax.set_xlabel("Tamanho do vetor (N)")
    ax.set_ylabel("Tempo médio por ordenação (ms)")
    ax.legend(loc="upper left", fontsize=9)
    salvar(fig, "ponto_de_virada.png")


def grafico_barras(df, n_alvo=100000):
    """Ranking dos 9 algoritmos em um N fixo."""
    sub = df[df["n"] == n_alvo].sort_values("media_ms")
    if sub.empty:
        print(f"  (sem dados para N = {fmt_num(n_alvo)}; gráfico de barras pulado)")
        return

    fig, ax = plt.subplots(figsize=(8.5, 4.8))
    cores = [FAMILIAS[familia(c)][1] for c in sub["categoria"]]
    barras = ax.barh(sub["algoritmo"], sub["media_ms"], color=cores, height=0.65,
                     edgecolor="white", linewidth=2)
    ax.invert_yaxis()  # mais rápido no topo
    eixo_log(ax, "x")
    ax.grid(False, axis="y")
    ax.set_title(f"Tempo médio com N = {fmt_num(n_alvo)} (escala log)")
    ax.set_xlabel("Tempo médio (ms)")

    for barra, valor in zip(barras, sub["media_ms"]):
        ax.annotate(fmt_ms(valor), (barra.get_width(), barra.get_y() + barra.get_height() / 2),
                    xytext=(4, 0), textcoords="offset points", va="center",
                    fontsize=9, color=TINTA)
    ax.set_xlim(sub["media_ms"].min() / 3, sub["media_ms"].max() * 8)

    # legenda das famílias
    from matplotlib.patches import Patch
    presentes = dict.fromkeys(familia(c) for c in sub["categoria"])
    ax.legend([Patch(color=FAMILIAS[f][1]) for f in presentes],
              [FAMILIAS[f][0] for f in presentes], loc="upper right", fontsize=9)
    salvar(fig, f"barras_n_{n_alvo}.png")


def grafico_amplitude_k(dk):
    """Tempo x amplitude K com N fixo: lineares dependem de K, comparativos não."""
    fig, ax = plt.subplots(figsize=(8.5, 5))
    for alg, g in dk.groupby("algoritmo", sort=False):
        ax.plot(g["k"], g["media_ms"], label=alg, **estilo(alg, g["categoria"].iloc[0]))

    eixo_log(ax)
    n = int(dk["n"].iloc[0])
    ax.set_title(f"Tempo x amplitude dos valores K (N = {fmt_num(n)})")
    ax.set_xlabel("Amplitude dos valores (K = maior valor possível + 1)")
    ax.set_ylabel("Tempo médio (ms)")
    # legenda fora da área do gráfico, para não cobrir as linhas
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1), fontsize=9,
              title="azul: comparação\nverde: linear", title_fontsize=9,
              alignment="left")
    salvar(fig, "tempo_x_k.png")


def main():
    print("Gerando gráficos em charts/:")
    df = pd.read_csv(PASTA / "resultados.csv")
    grafico_todos_loglog(df)
    grafico_familias(df)
    grafico_ponto_de_virada(df)
    grafico_barras(df)

    caminho_k = PASTA / "resultados_k.csv"
    if caminho_k.exists():
        grafico_amplitude_k(pd.read_csv(caminho_k))


if __name__ == "__main__":
    main()
