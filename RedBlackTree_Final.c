#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 1
#define BLACK 0

typedef struct Jogo {
    char titulo[50];
    int ano;
    char publisher[50];
} Jogo;

typedef struct RBNode {
    struct Jogo val;
    int color;
    struct RBNode *esq;
    struct RBNode *dir;
    struct RBNode *pai;
} RBNode;

typedef struct RBTree {
    RBNode *root;
    RBNode *NIL;
} RBTree;

RBTree *rbt_cria(void) {
    RBTree *T = (RBTree *)malloc(sizeof(RBTree));
    if (T == NULL) return NULL;
    T->NIL = (RBNode *)malloc(sizeof(RBNode));
    if (T->NIL == NULL) { free(T); return NULL; }
    T->NIL->color = BLACK;
    T->NIL->esq = T->NIL->dir = T->NIL->pai = T->NIL;
    strcpy(T->NIL->val.titulo, "0");
    strcpy(T->NIL->val.publisher, "0");
    T->NIL->val.ano = 0;
    T->root = T->NIL;
    T->root->pai = T->NIL;
    return T;
}

RBNode *cria_nodo_rbt(RBTree *T, Jogo val) {
    RBNode *n = (RBNode *)malloc(sizeof(RBNode));
    if (n == NULL) return NULL;
    n->val = val;
    n->color = RED;
    n->esq = n->dir = n->pai = T->NIL;
    return n;
}

void rbt_libera_rec(RBTree *T, RBNode *x) {
    if (x == T->NIL) return;
    rbt_libera_rec(T, x->esq);
    rbt_libera_rec(T, x->dir);
    free(x);
}

void rbt_apaga(RBTree *T) {
    if (T == NULL) return;
    rbt_libera_rec(T, T->root);
    free(T->NIL);
    free(T);
}

RBNode *busca(RBTree *T, Jogo val) {  
    RBNode *x = T->root;
    while (x != T->NIL && strcmp(val.titulo, x->val.titulo) != 0) {
        if (strcmp(val.titulo, x->val.titulo) < 0) x = x->esq;
        else x = x->dir;
    }
    return x;
}

void rot_esq(RBTree *T, RBNode *x) {
    RBNode *y = x->dir;
    x->dir = y->esq;
    if (y->esq != T->NIL) y->esq->pai = x;
    y->pai = x->pai;
    if (x->pai == T->NIL) T->root = y;
    else if (x == x->pai->esq) x->pai->esq = y;
    else x->pai->dir = y;
    y->esq = x;
    x->pai = y;
}

void rot_dir(RBTree *T, RBNode *y) {
    RBNode *x = y->esq;
    y->esq = x->dir;
    if (x->dir != T->NIL) x->dir->pai = y;
    x->pai = y->pai;
    if (y->pai == T->NIL) T->root = x;
    else if (y == y->pai->dir) y->pai->dir = x;
    else y->pai->esq = x;
    x->dir = y;
    y->pai = x;
}

void rbt_insere(RBTree *T, Jogo val) {
    RBNode *z = cria_nodo_rbt(T, val);
    if (z == NULL) return;

    RBNode *y = T->NIL;
    RBNode *x = T->root;
    while (x != T->NIL) {
        y = x;
        if (strcmp(z->val.titulo, x->val.titulo) < 0) x = x->esq;
        else x = x->dir;
    }
    z->pai = y;
    if (y == T->NIL) T->root = z;
    else if (strcmp(z->val.titulo, y->val.titulo) < 0) y->esq = z;
    else y->dir = z;

    z->esq = T->NIL;
    z->dir = T->NIL;
    z->color = RED;

    while (z->pai->color == RED) {
        if (z->pai == z->pai->pai->esq) {
            RBNode *tio = z->pai->pai->dir;
            if (tio->color == RED) {
                z->pai->color = BLACK;
                tio->color = BLACK;
                z->pai->pai->color = RED;
                z = z->pai->pai;
            } else {
                if (z == z->pai->dir) {
                    z = z->pai;
                    rot_esq(T, z);
                }
                z->pai->color = BLACK;
                z->pai->pai->color = RED;
                rot_dir(T, z->pai->pai);
            }
        } else {
            RBNode *tio = z->pai->pai->esq;
            if (tio->color == RED) {
                z->pai->color = BLACK;
                tio->color = BLACK;
                z->pai->pai->color = RED;
                z = z->pai->pai;
            } else {
                if (z == z->pai->esq) {
                    z = z->pai;
                    rot_dir(T, z);
                }
                z->pai->color = BLACK;
                z->pai->pai->color = RED;
                rot_esq(T, z->pai->pai);
            }
        }
    }
    T->root->color = BLACK;
}


static void rb_transplant(RBTree *T, RBNode *u, RBNode *v) {
    if (u->pai == T->NIL) T->root = v;
    else if (u == u->pai->esq) u->pai->esq = v;
    else u->pai->dir = v;
    v->pai = u->pai;
}

static RBNode *rbt_minimum(RBTree *T, RBNode *x) {
    while (x->esq != T->NIL) x = x->esq;
    return x;
}

static void rbt_delete_fixup(RBTree *T, RBNode *x) {
    while (x != T->root && x->color == BLACK) {
        if (x == x->pai->esq) {
            RBNode *w = x->pai->dir; // w é o irmão de x
            if (w->color == RED) {
                w->color = BLACK;
                x->pai->color = RED;
                rot_esq(T, x->pai);
                w = x->pai->dir;
            }
            if (w->esq->color == BLACK && w->dir->color == BLACK) {
                w->color = RED;
                x = x->pai;
            } else {
                if (w->dir->color == BLACK) {
                    w->esq->color = BLACK;
                    w->color = RED;
                    rot_dir(T, w);
                    w = x->pai->dir;
                }
                w->color = x->pai->color;
                x->pai->color = BLACK;
                w->dir->color = BLACK;
                rot_esq(T, x->pai);
                x = T->root;
            }
        } else { // Caso simétrico (x é filho direito)
            RBNode *w = x->pai->esq;
            if (w->color == RED) {
                w->color = BLACK;
                x->pai->color = RED;
                rot_dir(T, x->pai);
                w = x->pai->esq;
            }
            if (w->dir->color == BLACK && w->esq->color == BLACK) {
                w->color = RED;
                x = x->pai;
            } else {
                if (w->esq->color == BLACK) {
                    w->dir->color = BLACK;
                    w->color = RED;
                    rot_esq(T, w);
                    w = x->pai->esq;
                }
                w->color = x->pai->color;
                x->pai->color = BLACK;
                w->esq->color = BLACK;
                rot_dir(T, x->pai);
                x = T->root;
            }
        }
    }
    x->color = BLACK;
}

void rbt_remove(RBTree *T, Jogo val) {
    RBNode *z = busca(T, val);
    if (z == T->NIL) return; // Nó não encontrado

    RBNode *y = z;
    RBNode *x;
    int y_original_color = y->color;

    if (z->esq == T->NIL) {
        x = z->dir;
        rb_transplant(T, z, z->dir);
    } else if (z->dir == T->NIL) {
        x = z->esq;
        rb_transplant(T, z, z->esq);
    } else {
        y = rbt_minimum(T, z->dir);
        y_original_color = y->color;
        x = y->dir;
        if (y->pai == z) {
            x->pai = y;
        } else {
            rb_transplant(T, y, y->dir);
            y->dir = z->dir;
            y->dir->pai = y;
        }
        rb_transplant(T, z, y);
        y->esq = z->esq;
        y->esq->pai = y;
        y->color = z->color;
    }

    free(z);

    if (y_original_color == BLACK) {
        rbt_delete_fixup(T, x);
    }
}



void rbt_graphviz_rec(RBNode* nodo_atual, FILE* fp, RBNode* NIL) {
    if (nodo_atual == NIL) return;

    fprintf(fp, "    \"%p\" [label=\"%s\", style=filled, fillcolor=%s, fontcolor=white, shape=circle];\n", 
            (void*)nodo_atual, 
            nodo_atual->val.titulo, 
            nodo_atual->color == RED ? "red" : "black");

    if (nodo_atual->esq != NIL) {
        fprintf(fp, "    \"%p\" -> \"%p\";\n", (void*)nodo_atual, (void*)nodo_atual->esq);
    }
    
    if (nodo_atual->dir != NIL) {
        fprintf(fp, "    \"%p\" -> \"%p\";\n", (void*)nodo_atual, (void*)nodo_atual->dir);
    }
    
    rbt_graphviz_rec(nodo_atual->esq, fp, NIL);
    rbt_graphviz_rec(nodo_atual->dir, fp, NIL);
}

void rbt_graphviz(RBTree* arvore) {
    FILE* fp = fopen("rbt.dot", "w");
    if (fp == NULL) return;

    fprintf(fp, "digraph RubroNegra {\n");
    if (arvore->root != arvore->NIL) {
        rbt_graphviz_rec(arvore->root, fp, arvore->NIL);
    }
    fprintf(fp, "}\n");
    fclose(fp);
}

int main(void) {
    RBTree* Minha_Arvore = rbt_cria();
    
    Jogo lista_jogos[50] = {
        {"3-D Tic-Tac-Toe", 1978, "Atari"},
        {"Acid Drop", 1992, "Salu Ltd"},
        {"Adventure", 1979, "Atari"},
        {"Adventures of Tron", 1982, "M Network"},
        {"Air Raid", 1982, "MenAVision"},
        {"Air Raiders", 1982, "M Network"},
        {"Air-Sea Battle", 1977, "Atari"},
        {"Airlock", 1982, "Data Age"},
        {"Alien", 1982, "20th Century Fox"},
        {"Amidar", 1982, "Parker Brothers"},
        {"Armor Ambush", 1982, "M Network"},
        {"Artillery Duel", 1983, "Xonox"},
        {"Asterix", 1983, "Atari"},
        {"Asteroids", 1981, "Atari"},
        {"Atlantis", 1982, "Imagic"},
        {"Bank Heist", 1983, "20th Century Fox"},
        {"Barnstorming", 1982, "Activision"},
        {"Basic Math", 1977, "Atari"},
        {"Basketball", 1978, "Atari"},
        {"Battlezone", 1983, "Atari"},
        {"Beamrider", 1983, "Activision"},
        {"Beany Bopper", 1982, "20th Century Fox"},
        {"Berzerk", 1982, "Atari"},
        {"Blackjack", 1977, "Atari"},
        {"Blue Print", 1983, "CBS Electronics"},
        {"Bowling", 1979, "Atari"},
        {"Boxing", 1980, "Activision"},
        {"Brain Games", 1978, "Atari"},
        {"Breakout", 1978, "Atari"},
        {"Buck Rogers: Planet of Zoom", 1983, "Sega"},
        {"BurgerTime", 1982, "M Network"},
        {"California Games", 1987, "Epyx"},
        {"Canyon Bomber", 1979, "Atari"},
        {"Carnival", 1982, "Coleco"},
        {"Casino", 1978, "Atari"},
        {"Centipede", 1983, "Atari"},
        {"Chase the Chuck Wagon", 1983, "Spectravision"},
        {"Chopper Command", 1982, "Activision"},
        {"Chuck Norris Superkicks", 1983, "Xonox"},
        {"Circus Atari", 1980, "Atari"},
        {"Coconuts", 1982, "Telesys"},
        {"Codebreaker", 1978, "Atari"},
        {"Combat", 1977, "Atari"},
        {"Commando", 1988, "Activision"},
        {"Congo Bongo", 1983, "Sega"},
        {"Cosmic Ark", 1982, "Imagic"},
        {"Crackpots", 1983, "Activision"},
        {"Crazy Climber", 1983, "Atari"},
        {"Crystal Castles", 1984, "Atari"},
        {"Custer's Revenge", 1982, "Mystique"}
    };

    for(int i = 0; i < 50; i++) {
        rbt_insere(Minha_Arvore, lista_jogos[i]);
    }

    // Exemplo de teste de remoção
    rbt_remove(Minha_Arvore, lista_jogos[0]); // Remove "3-D Tic-Tac-Toe"

    rbt_graphviz(Minha_Arvore);
	
    printf("Use o comando: dot rbt.dot -Tsvg -o rbt.svg\n e depois xdg-open rbt.svg para visualização.\n");
    printf("Ou cole o conteúdo do arquivo em https://dreampuf.github.io/GraphvizOnline\n");

    rbt_apaga(Minha_Arvore);
    return 0;
}
