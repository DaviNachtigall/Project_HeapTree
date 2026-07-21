#  Binary Max-Heap & Graphviz Exporter in C

A **Binary Max-Heap** data structure implementation in C using 1-based array indexing to simplify complete binary tree navigation.

Includes subroutines for heap building, top-element extraction, and dynamic **Graphviz** `.dot` visualization export.

---

##  Features

- **1-Based Array Indexing:** Index `0` is ignored to simplify child-parent mathematical relationships:
  - **Left Child:** `2 * i`
  - **Right Child:** `2 * i + 1`
  - **Parent:** `i / 2`
- **Max-Heap Property:** Parent nodes remain strictly greater than or equal to their children.
- **Dynamic Restructuring:** Includes `max_heapify` and `build_maxheap` for $O(n)$ heap construction.
- **Graphviz Export:** Automatically exports tree states to `.dot` graph files for visual rendering.

---

##  Graphviz Visualization
<img width="659" height="347" alt="HeapArvore" src="https://github.com/user-attachments/assets/b8602a77-6504-4f83-9c22-6e19ce50c753" />

The program generates two separate `.dot` output files (`heap1.dot` and `heap2.dot`) representing:
1. **`heap1.dot`:** Initial array after `build_maxheap`.
2. **`heap2.dot`:** Tree structure after root extraction and heap re-balancing.

---

##  Build and Run

### Prerequisites
Ensure `gcc` and `graphviz` are installed on your Linux environment:

```bash
sudo apt update
sudo apt install build-essential graphviz
