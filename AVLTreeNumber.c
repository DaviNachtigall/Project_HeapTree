#include <stdio.h>
#include <stdlib.h>

typedef struct Nodo {
    int valor;
    struct Nodo* left;
    struct Nodo* right;
    int altura;
} Nodo;

typedef struct Arvore {
    Nodo* raiz;
} Arvore;

int obter_altura(Nodo* nodo) {
    if (nodo == NULL) return -1;
    return nodo->altura;
}

int maximo(int a, int b) {
    return (a > b) ? a : b;
}

void atualizar_altura(Nodo* nodo) {
    if (nodo != NULL) {
        nodo->altura = maximo(obter_altura(nodo->left), obter_altura(nodo->right)) + 1;
    }
}

int obter_fator_balanceamento(Nodo* nodo) {
    if (nodo == NULL) return 0;
    return obter_altura(nodo->left) - obter_altura(nodo->right);
}

Nodo* rotacionar_esquerda(Nodo* nodo_atual) {
    Nodo* nova_raiz_local = nodo_atual->right;
    nodo_atual->right = nova_raiz_local->left;
    nova_raiz_local->left = nodo_atual;

    atualizar_altura(nodo_atual);
    atualizar_altura(nova_raiz_local);

    return nova_raiz_local;
}

Nodo* rotacionar_direita(Nodo* nodo_atual) {
    Nodo* nova_raiz_local = nodo_atual->left;
    nodo_atual->left = nova_raiz_local->right;
    nova_raiz_local->right = nodo_atual;

    atualizar_altura(nodo_atual);
    atualizar_altura(nova_raiz_local);

    return nova_raiz_local;
}

Nodo* criar_nodo(int valor) {
    Nodo* novo_nodo = (Nodo*)malloc(sizeof(Nodo));
    if (novo_nodo == NULL) return NULL;
    novo_nodo->valor = valor;
    novo_nodo->altura = 0; 
    novo_nodo->left = NULL;
    novo_nodo->right = NULL;
    return novo_nodo;
}

Nodo* inserir_recursivo(Nodo* nodo_atual, int valor) {
    if (nodo_atual == NULL) return criar_nodo(valor);
    
    if (valor < nodo_atual->valor) { 
        nodo_atual->left = inserir_recursivo(nodo_atual->left, valor);
    }
    else if (valor > nodo_atual->valor) { 
        nodo_atual->right = inserir_recursivo(nodo_atual->right, valor);
    }
    else {
        return nodo_atual;
    }

    atualizar_altura(nodo_atual);

    int fb = obter_fator_balanceamento(nodo_atual);

    if (fb > 1 && valor < nodo_atual->left->valor) {
        return rotacionar_direita(nodo_atual);
    }

    if (fb < -1 && valor > nodo_atual->right->valor) {
        return rotacionar_esquerda(nodo_atual);
    }

    if (fb > 1 && valor > nodo_atual->left->valor) {
        nodo_atual->left = rotacionar_esquerda(nodo_atual->left);
        return rotacionar_direita(nodo_atual);
    }

    if (fb < -1 && valor < nodo_atual->right->valor) {
        nodo_atual->right = rotacionar_direita(nodo_atual->right);
        return rotacionar_esquerda(nodo_atual);
    }

    return nodo_atual;
}

int inserir_valor(Arvore* arvore, int valor) {
    if (arvore == NULL) return -1;
    
    Nodo* raiz_atualizada = inserir_recursivo(arvore->raiz, valor);
    if (raiz_atualizada != NULL) {
        arvore->raiz = raiz_atualizada;
        return 0;
    }
    return -1;
}

Arvore* criar_arvore() {
    Arvore* nova_arvore = (Arvore*)malloc(sizeof(Arvore));
    if (nova_arvore == NULL) return NULL;
    nova_arvore->raiz = NULL;
    return nova_arvore;
}

void avl_graphviz_rec(Nodo* nodo_atual, FILE* fp) {
    if (nodo_atual == NULL) return;

    fprintf(fp, "    \"%p\" [label=\"%d\", style=filled, fillcolor=\"#2980b9\", fontcolor=white, shape=circle];\n", 
            (void*)nodo_atual, 
            nodo_atual->valor);

    if (nodo_atual->left != NULL) {
        fprintf(fp, "    \"%p\" -> \"%p\";\n", (void*)nodo_atual, (void*)nodo_atual->left);
    }
    
    if (nodo_atual->right != NULL) {
        fprintf(fp, "    \"%p\" -> \"%p\";\n", (void*)nodo_atual, (void*)nodo_atual->right);
    }
    
    avl_graphviz_rec(nodo_atual->left, fp);
    avl_graphviz_rec(nodo_atual->right, fp);
}

void avl_graphviz(Arvore* arvore) {
    FILE* fp = fopen("avl.dot", "w");
    if (fp == NULL) return;

    fprintf(fp, "digraph AVL {\n");
    fprintf(fp, "    bgcolor=\"#222222\";\n"); 
    fprintf(fp, "    edge [color=white];\n");     

    if (arvore != NULL && arvore->raiz != NULL) {
        avl_graphviz_rec(arvore->raiz, fp);
    }

    fprintf(fp, "}\n");
    fclose(fp);
}

void avl_libera_rec(Nodo* x) {
    if (x == NULL) return;
    avl_libera_rec(x->left);
    avl_libera_rec(x->right);
    free(x);
}

void avl_apaga(Arvore* arvore) {
    if (arvore == NULL) return;
    avl_libera_rec(arvore->raiz);
    free(arvore);
}

int main() {
    Arvore* minha_arvore = criar_arvore();

    int lista_numeros[50] = {
        45, 12, 78, 3, 89, 23, 56, 90, 67, 34,
        1, 99, 50, 15, 7, 62, 28, 81, 39, 94,
        5, 71, 19, 43, 84, 11, 68, 31, 97, 52,
        8, 76, 25, 49, 88, 17, 60, 35, 92, 58,
        2, 73, 21, 47, 86, 14, 64, 33, 95, 54
    };

    for(int i = 0; i < 50; i++) {
        inserir_valor(minha_arvore, lista_numeros[i]);
    }

    avl_graphviz(minha_arvore);
    
    printf("Use o comando: dot avl.dot -Tsvg -o avl.svg\n e depois abra para visualização.\n");
    
    avl_apaga(minha_arvore);
    return 0;
}
