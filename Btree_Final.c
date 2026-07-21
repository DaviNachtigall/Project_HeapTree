#include <stdio.h>
#include <stdlib.h>

#define ORDEM 3                     /* t: ordem mínima */
#define MAX_CHAVES (2 * ORDEM)     /* 2t: capacidade máxima de chaves por página */
#define MAX_FILHOS (MAX_CHAVES + 1) /* 2t + 1: capacidade máxima de ponteiros */
#define MIN_CHAVES (ORDEM - 1)     /* t - 1: capacidade mínima de chaves por página */

typedef struct Pagina {
    int n;                             /* número de chaves presentes */
    int chaves[MAX_CHAVES];            /* chaves ordenadas */
    struct Pagina* filhos[MAX_FILHOS]; /* ponteiros para subárvores */
    int folha;                         /* 1 se for folha, 0 caso contrário */
} Pagina;

Pagina* criar_pagina(int folha) {
    Pagina* p = (Pagina*)calloc(1, sizeof(Pagina));
    if (p == NULL) return NULL;
    p->folha = folha;
    return p;
}

void btree_liberar(Pagina* raiz) {
    if (raiz == NULL) return;
    if (!raiz->folha) {
        for (int i = 0; i <= raiz->n; i++) {
            btree_liberar(raiz->filhos[i]);
        }
    }
    free(raiz);
}


Pagina* btree_buscar(Pagina* x, int k, int* indice) {
    if (x == NULL) return NULL;
    int i = 0;
    while (i < x->n && k > x->chaves[i]) i++;
    if (i < x->n && k == x->chaves[i]) {
        if (indice) *indice = i;
        return x;
    }
    if (x->folha) return NULL;
    return btree_buscar(x->filhos[i], k, indice);
}

/* Divide o filho i de pai 'x' (que está cheio com 2t chaves) */
void btree_dividir_filho(Pagina* x, int i) {
    Pagina* y = x->filhos[i];
    Pagina* z = criar_pagina(y->folha);
    z->n = ORDEM; // Recebe m chaves do final

    // Copia as últimas 'ORDEM' chaves de y para z
    for (int j = 0; j < ORDEM; j++) {
        z->chaves[j] = y->chaves[j + ORDEM];
        y->chaves[j + ORDEM] = 0;
    }

    // Copia os ponteiros de filhos se não for folha
    if (!y->folha) {
        for (int j = 0; j <= ORDEM; j++) {
            z->filhos[j] = y->filhos[j + ORDEM];
            y->filhos[j + ORDEM] = NULL;
        }
    }

    // A chave mediana de y sobe para o pai x (índice ORDEM - 1)
    int chave_mediana = y->chaves[ORDEM - 1];
    y->chaves[ORDEM - 1] = 0;
    y->n = ORDEM - 1; // y fica com t-1 chaves

    // Desloca os filhos do pai x para abrir espaço para z
    for (int j = x->n; j >= i + 1; j--) {
        x->filhos[j + 1] = x->filhos[j];
    }
    x->filhos[i + 1] = z;

    // Desloca as chaves do pai x para abrir espaço para a chave mediana
    for (int j = x->n - 1; j >= i; j--) {
        x->chaves[j + 1] = x->chaves[j];
    }
    x->chaves[i] = chave_mediana;
    x->n++;
}


void btree_inserir_nao_cheio(Pagina* x, int k) {
    int i = x->n - 1;

    if (x->folha) {
        // Encontra a posição e insere na folha
        while (i >= 0 && k < x->chaves[i]) {
            x->chaves[i + 1] = x->chaves[i];
            i--;
        }
        x->chaves[i + 1] = k;
        x->n++;
    } else {
        // Encontra qual filho deve receber k
        while (i >= 0 && k < x->chaves[i]) i--;
        i++;

        // Divisão Preventiva: se o filho está cheio, divide ANTES de descer!
        if (x->filhos[i]->n == MAX_CHAVES) {
            btree_dividir_filho(x, i);
            if (k > x->chaves[i]) {
                i++;
            }
        }
        btree_inserir_nao_cheio(x->filhos[i], k);
    }
}

Pagina* btree_inserir(Pagina* raiz, int k) {
    if (raiz == NULL) {
        raiz = criar_pagina(1);
        raiz->chaves[0] = k;
        raiz->n = 1;
        return raiz;
    }

    // Se a raiz estiver cheia, ela se divide e a altura da árvore aumenta em 1
    if (raiz->n == MAX_CHAVES) {
        Pagina* s = criar_pagina(0);
        s->filhos[0] = raiz;
        btree_dividir_filho(s, 0);
        
        int i = 0;
        if (s->chaves[0] < k) i++;
        btree_inserir_nao_cheio(s->filhos[i], k);
        return s;
    }

    btree_inserir_nao_cheio(raiz, k);
    return raiz;
}

static int obter_predecessor(Pagina* x, int idx) {
    Pagina* cur = x->filhos[idx];
    while (!cur->folha) cur = cur->filhos[cur->n];
    return cur->chaves[cur->n - 1];
}

static int obter_sucessor(Pagina* x, int idx) {
    Pagina* cur = x->filhos[idx + 1];
    while (!cur->folha) cur = cur->filhos[0];
    return cur->chaves[0];
}

static void fundir_paginas(Pagina* x, int idx) {
    Pagina* filho = x->filhos[idx];
    Pagina* irmao = x->filhos[idx + 1];

    filho->chaves[filho->n] = x->chaves[idx];

    for (int i = 0; i < irmao->n; ++i) {
        filho->chaves[i + filho->n + 1] = irmao->chaves[i];
    }
    if (!filho->folha) {
        for (int i = 0; i <= irmao->n; ++i) {
            filho->filhos[i + filho->n + 1] = irmao->filhos[i];
        }
    }

    for (int i = idx + 1; i < x->n; ++i) {
        x->chaves[i - 1] = x->chaves[i];
    }
    for (int i = idx + 2; i <= x->n; ++i) {
        x->filhos[i - 1] = x->filhos[i];
    }

    filho->n += irmao->n + 1;
    x->n--;
    free(irmao);
}

static void preencher_filho(Pagina* x, int idx) {
    if (idx != 0 && x->filhos[idx - 1]->n >= ORDEM) {
        // Pega chave emprestada do irmão esquerdo
        Pagina* filho = x->filhos[idx];
        Pagina* irmao = x->filhos[idx - 1];

        for (int i = filho->n - 1; i >= 0; --i) filho->chaves[i + 1] = filho->chaves[i];
        if (!filho->folha) {
            for (int i = filho->n; i >= 0; --i) filho->filhos[i + 1] = filho->filhos[i];
        }

        filho->chaves[0] = x->chaves[idx - 1];
        if (!filho->folha) filho->filhos[0] = irmao->filhos[irmao->n];

        x->chaves[idx - 1] = irmao->chaves[irmao->n - 1];
        filho->n++;
        irmao->n--;
    } else if (idx != x->n && x->filhos[idx + 1]->n >= ORDEM) {
        // Pega chave emprestada do irmão direito
        Pagina* filho = x->filhos[idx];
        Pagina* irmao = x->filhos[idx + 1];

        filho->chaves[filho->n] = x->chaves[idx];
        if (!filho->folha) filho->filhos[filho->n + 1] = irmao->filhos[0];

        x->chaves[idx] = irmao->chaves[0];

        for (int i = 1; i < irmao->n; ++i) irmao->chaves[i - 1] = irmao->chaves[i];
        if (!irmao->folha) {
            for (int i = 1; i <= irmao->n; ++i) irmao->filhos[i - 1] = irmao->filhos[i];
        }

        filho->n++;
        irmao->n--;
    } else {
        // Fusão com irmão
        if (idx != x->n) fundir_paginas(x, idx);
        else fundir_paginas(x, idx - 1);
    }
}

static void btree_remover_rec(Pagina* x, int k) {
    int idx = 0;
    while (idx < x->n && x->chaves[idx] < k) idx++;

    if (idx < x->n && x->chaves[idx] == k) {
        if (x->folha) {
            // Caso 1: Chave está numa folha
            for (int i = idx + 1; i < x->n; ++i) x->chaves[i - 1] = x->chaves[i];
            x->n--;
        } else {
            // Caso 2: Chave está num nó interno
            if (x->filhos[idx]->n >= ORDEM) {
                int pred = obter_predecessor(x, idx);
                x->chaves[idx] = pred;
                btree_remover_rec(x->filhos[idx], pred);
            } else if (x->filhos[idx + 1]->n >= ORDEM) {
                int suc = obter_sucessor(x, idx);
                x->chaves[idx] = suc;
                btree_remover_rec(x->filhos[idx + 1], suc);
            } else {
                fundir_paginas(x, idx);
                btree_remover_rec(x->filhos[idx], k);
            }
        }
    } else {
        // Caso 3: Chave não está neste nó
        if (x->folha) return; // Chave não existe na árvore

        int flag = (idx == x->n);
        if (x->filhos[idx]->n < ORDEM) preencher_filho(x, idx);

        if (flag && idx > x->n) {
            btree_remover_rec(x->filhos[idx - 1], k);
        } else {
            btree_remover_rec(x->filhos[idx], k);
        }
    }
}

Pagina* btree_remover(Pagina* raiz, int k) {
    if (raiz == NULL) return NULL;

    btree_remover_rec(raiz, k);

    // Se a raiz ficou vazia após remoção/fusão
    if (raiz->n == 0) {
        Pagina* tmp = raiz;
        if (raiz->folha) raiz = NULL;
        else raiz = raiz->filhos[0];
        free(tmp);
    }
    return raiz;
}


void btree_escrever_nos_dot(Pagina* pagina, FILE* fp, int* id_atual) {
    if (pagina == NULL) return;

    int meu_id = (*id_atual)++;

    fprintf(fp, "    node%d [label=\"", meu_id);
    for (int i = 0; i < pagina->n; i++) {
        if (i == 0) fprintf(fp, "<f%d> | %d", i, pagina->chaves[i]);
        else fprintf(fp, " | <f%d> | %d", i, pagina->chaves[i]);
    }
    fprintf(fp, " | <f%d>\", shape=record];\n", pagina->n);

    if (!pagina->folha) {
        for (int i = 0; i <= pagina->n; i++) {
            if (pagina->filhos[i] != NULL) {
                int id_filho = *id_atual;
                btree_escrever_nos_dot(pagina->filhos[i], fp, id_atual);
                fprintf(fp, "    \"node%d\":f%d -> \"node%d\";\n", meu_id, i, id_filho);
            }
        }
    }
}

void btree_gerar_dot(Pagina* raiz, const char* nome_arquivo) {
    FILE* fp = fopen(nome_arquivo, "w");
    if (fp == NULL) return;

    fprintf(fp, "digraph BTree {\n");
    fprintf(fp, "    node [fontname=\"Arial\"];\n");

    int contador_id = 0;
    btree_escrever_nos_dot(raiz, fp, &contador_id);

    fprintf(fp, "}\n");
    fclose(fp);
    printf("Arquivo '%s' gerado com sucesso!\n", nome_arquivo);
}

int main() {
    Pagina* raiz = NULL;

    printf("--- Inserindo elementos na B-Tree (Ordem %d) ---\n", ORDEM);
    int valores[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150};
    int total_valores = sizeof(valores) / sizeof(valores[0]);

    for (int i = 0; i < total_valores; i++) {
        raiz = btree_inserir(raiz, valores[i]);
    }

   
    printf("Removendo 60 e 100...\n");
    raiz = btree_remover(raiz, 60);
    raiz = btree_remover(raiz, 100);

    btree_gerar_dot(raiz, "arvore_btree.dot");

    btree_liberar(raiz);
    return 0;
}
