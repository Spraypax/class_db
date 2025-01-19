#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>

#define COLUMN_SIZE 100
#define ROW_SIZE 100

typedef struct {
    int id; // Identifiant de la ligne
    char name[COLUMN_SIZE]; // Nom ou autre donnée
} Row;

typedef struct {
    Row rows[ROW_SIZE]; // Tableau de lignes
    size_t num_rows; // Nombre de lignes actuellement dans la table
} Table;

#endif
