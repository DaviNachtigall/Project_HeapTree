# Max-Heap Binário & Exportador Graphviz em C
Uma implementação da estrutura de dados **Max-Heap Binário** em C, utilizando indexação de array baseada em 1 para simplificar a navegação em árvores binárias completas.
Inclui sub-rotinas para construção do heap, extração do elemento do topo e exportação dinâmica de visualização em `.dot` via **Graphviz**.

---

## Funcionalidades
- **Indexação de Array Baseada em 1:** O índice `0` é ignorado para simplificar as relações matemáticas entre pai e filho:
  - **Filho à Esquerda:** `2 * i`
  - **Filho à Direita:** `2 * i + 1`
  - **Pai:** `i / 2`
- **Propriedade do Max-Heap:** Os nós pais permanecem estritamente maiores ou iguais aos seus filhos.
- **Reestruturação Dinâmica:** Inclui `max_heapify` e `build_maxheap` para construção do heap em $O(n)$.
- **Exportação Graphviz:** Exporta automaticamente os estados da árvore para arquivos de grafo `.dot`, permitindo renderização visual.

---

## Visualização Graphviz
<img width="659" height="347" alt="HeapArvore" src="https://github.com/user-attachments/assets/b8602a77-6504-4f83-9c22-6e19ce50c753" />

O programa gera dois arquivos de saída `.dot` separados (`heap1.dot` e `heap2.dot`) representando:
1. **`heap1.dot`:** Array inicial após o `build_maxheap`.
2. **`heap2.dot`:** Estrutura da árvore após a extração da raiz e o rebalanceamento do heap.

---

## Compilação e Execução
### Pré-requisitos
Certifique-se de que `gcc` e `graphviz` estão instalados no seu ambiente Linux:
```bash
sudo apt update
sudo apt install build-essential graphviz
```
