#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Nodo_Arv {
    int info;
    struct Nodo_Arv *left;
    struct Nodo_Arv *right;
} Nodo_Arv;

Nodo_Arv* cria_nodo_arv(int dado) {
    Nodo_Arv *novo_nodo = (Nodo_Arv*)malloc(sizeof(Nodo_Arv));
    if (!novo_nodo) {
        perror("Erro de alocacao\n");
        exit(EXIT_FAILURE);
    }
    novo_nodo->info = dado;
    novo_nodo->left = NULL;
    novo_nodo->right = NULL;
    return novo_nodo;
}

Nodo_Arv* arv_criavazia() {
    return NULL;
}

Nodo_Arv* insere(Nodo_Arv* nodo, int val) {
    if (nodo == NULL) {
        return cria_nodo_arv(val);
    }
    if (val < nodo->info) {
        nodo->left = insere(nodo->left, val);
    } else {
        nodo->right = insere(nodo->right, val);
    }
    return nodo;
}

Nodo_Arv* retira(Nodo_Arv* nodo, int val) {
    if (nodo == NULL) return NULL;

    if (nodo->info > val) {
        nodo->left = retira(nodo->left, val);
    } else if (nodo->info < val) {
        nodo->right = retira(nodo->right, val);
    } else { // Achou o elemento
        if (nodo->left == NULL && nodo->right == NULL) {
            free(nodo);
            return NULL;
        } else if (nodo->left == NULL) {
            Nodo_Arv* aux = nodo->right;
            free(nodo);
            return aux;
        } else if (nodo->right == NULL) {
            Nodo_Arv* aux = nodo->left;
            free(nodo);
            return aux;
        } else {
            Nodo_Arv* filho = nodo->left;
            while (filho->right != NULL) {
                filho = filho->right;
            }
            nodo->info = filho->info;
            nodo->left = retira(nodo->left, filho->info); // Corrigido aqui
        }
    }
    return nodo;
}

void libera_subArvore(Nodo_Arv* nodo) {
    if (nodo == NULL) return;
    libera_subArvore(nodo->left);
    libera_subArvore(nodo->right);
    free(nodo);
}

Nodo_Arv* retira_com_filho(Nodo_Arv* nodo, int val) {
    if (nodo == NULL) return NULL;

    if (nodo->info > val) {
        nodo->left = retira_com_filho(nodo->left, val);
    } else if (nodo->info < val) {
        nodo->right = retira_com_filho(nodo->right, val);
    } else {
        libera_subArvore(nodo);
        return NULL; // Corrigido aqui para o pai saber que o filho sumiu
    }
    return nodo;
}

void coleta_info(Nodo_Arv* nodo, int* soma, int* folhas, int* nodos) {
    if (nodo == NULL) return;
    if (nodo->left == NULL && nodo->right == NULL) {
        (*folhas)++;
    }
    (*soma) += nodo->info;
    (*nodos)++;

    coleta_info(nodo->left, soma, folhas, nodos);
    coleta_info(nodo->right, soma, folhas, nodos);
}

Nodo_Arv* info_galho(Nodo_Arv* nodo, int val) {
    if (nodo == NULL) return NULL;

    if (nodo->info > val) {
        nodo->left = info_galho(nodo->left, val);
    } else if (nodo->info < val) {
        nodo->right = info_galho(nodo->right, val);
    } else {
        int soma = 0, folhas = 0, nodos = 0;
        coleta_info(nodo, &soma, &folhas, &nodos);
        printf("Sub-arvore %d:\n", val);
        printf("- Soma dos valores: %d\n", soma);
        printf("- Qtd de folhas: %d\n", folhas);
        printf("- Total de nos: %d\n", nodos);    
    }
    return nodo;
}

void percorre_imprimindo(Nodo_Arv *nodo, FILE* meu_arquivo) {
    if (nodo != NULL) {
        if (nodo->left != NULL)
            fprintf(meu_arquivo, "  %d -- %d;\n", nodo->info, nodo->left->info);
        if (nodo->right != NULL)
            fprintf(meu_arquivo, "  %d -- %d;\n", nodo->info, nodo->right->info);

        percorre_imprimindo(nodo->left, meu_arquivo);
        percorre_imprimindo(nodo->right, meu_arquivo);
    }
}

void graphviz(Nodo_Arv *a, FILE* meu_arquivo) {
    fprintf(meu_arquivo, "strict graph G {\n"); // Evita duplicar linhas
    percorre_imprimindo(a, meu_arquivo);
    fprintf(meu_arquivo, "}\n");
}

int* cria_vetor_aleatorio(int tamanho, int valor_min) {
    int *vetor = (int*)malloc(tamanho * sizeof(int));
    if (vetor == NULL) return NULL;

    for (int i = 0; i < tamanho; i++) {
        vetor[i] = valor_min + i;
    }

    srand(time(NULL)); 
    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
    return vetor;
}

int main() {
    Nodo_Arv* Arvore = arv_criavazia();
    int tamanho = 100;
    int* vec = cria_vetor_aleatorio(tamanho, 1);
    
    for (int i = 0; i < tamanho; i++) {
        Arvore = insere(Arvore, vec[i]);
    }
    
    // Teste de info da subárvore
    info_galho(Arvore, vec[5]); // Usa um valor que com certeza existe na árvore

    // Exportando para o Graphviz antes de remover
    FILE* meu_arquivo = fopen("grafo_antes.dot", "w");
    if(meu_arquivo) {
        graphviz(Arvore, meu_arquivo);
        fclose(meu_arquivo);
    }

    int num_elementos = 24;
    while (num_elementos > 0) {
        int sorteado = rand() % 100 + 1;
        Arvore = retira(Arvore, sorteado);
        num_elementos--;
    }

    // Exportando para o Graphviz depois de remover
    FILE* meu_arquivo_depois = fopen("grafo_depois.dot", "w");
    if(meu_arquivo_depois) {
        graphviz(Arvore, meu_arquivo_depois);
        fclose(meu_arquivo_depois);
    }

    // Limpeza de memória final
    libera_subArvore(Arvore);
    free(vec);

    return 0;
}
