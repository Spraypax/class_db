#include "../include/table.h"
#include <stdio.h>
#include <stdlib.h>

// Structure de noeud pour l'arbre binaire
typedef struct Node {
    Row row;
    struct Node* left;
    struct Node* right;
} Node;

// Fonction pour creer un nouveau noeud
Node* create_node(Row row) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Erreur d'allocation de memoire");
        exit(EXIT_FAILURE);
    }
    new_node->row = row;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Fonction d'insertion dans l'arbre binaire
Node* insert(Node* root, Row row) {
    if (root == NULL) {
        return create_node(row);
    }

    if (row.id < root->row.id) {
        root->left = insert(root->left, row);
    } else if (row.id > root->row.id) {
        root->right = insert(root->right, row);
    } else {
        printf("ID deja present : %d. Insertion ignoree.\n", row.id);
    }
    return root;
}

// Fonction de recherche dans l'arbre binaire
Node* search(Node* root, int id) {
    if (root == NULL || root->row.id == id) {
        return root;
    }

    if (id < root->row.id) {
        return search(root->left, id);
    }
    return search(root->right, id);
}

// Fonction pour trouver le noeud avec la plus petite valeur (suppression)
Node* find_min(Node* root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Fonction de suppression dans l'arbre binaire
Node* delete(Node* root, int id) {
    if (root == NULL) {
        return root;
    }

    if (id < root->row.id) {
        root->left = delete(root->left, id);
    } else if (id > root->row.id) {
        root->right = delete(root->right, id);
    } else {
        // Cas 1 : Pas d'enfant
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        // Cas 2 : Un seul enfant
        else if (root->left == NULL) {
            Node* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node* temp = root->left;
            free(root);
            return temp;
        }
        // Cas 3 : Deux enfants
        Node* temp = find_min(root->right);
        root->row = temp->row;
        root->right = delete(root->right, temp->row.id);
    }
    return root;
}

// Foncton pour afficher l'arbre
void inorder(Node* root) {
    if (root != NULL) {
        inorder(root->left);
        printf("ID: %d, Name: %s\n", root->row.id, root->row.name);
        inorder(root->right);
    }
}

void btree_demo() {
    Node* root = NULL;

    // Exemple de lignes (notations)
    Row row1 = {1, "Alice"};
    Row row2 = {2, "Bob"};
    Row row3 = {3, "Charlie"};
    Row row4 = {2, "Duplicate"};

    printf("Insertion des donnees...\n");
    root = insert(root, row1);
    root = insert(root, row2);
    root = insert(root, row3);

    printf("Arbre binaire apres insertion :\n");
    inorder(root);

    // Test d'un doublon
    printf("\nTentative d'insertion d'un ID deja existant (2):\n");
    root = insert(root, row4);

    // Suppression d'un élément
    printf("\nSuppression de l'ID 2:\n");
    root = delete(root, 2);
    printf("Arbre binaire apres suppression :\n");
    inorder(root);

    // Suppression d'un ID inexistant
    printf("\nTentative de suppression d'un ID inexistant (99):\n");
    root = delete(root, 99);
    printf("Arbre binaire apres suppression d'un ID inexistant :\n");
    inorder(root);
}

