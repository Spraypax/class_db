#ifndef BTREE_H
#define BTREE_H

#include "table.h"

// Déclaration de la structure Node
typedef struct Node {
    Row row;
    struct Node* left;
    struct Node* right;
} Node;

// Fonctions pour manipuler l'arbre binaire
Node* create_node(Row row);
Node* insert(Node* root, Row row);
Node* search(Node* root, int id);
Node* find_min(Node* root);
Node* delete(Node* root, int id);
void inorder(Node* root);

void btree_demo();

#endif
