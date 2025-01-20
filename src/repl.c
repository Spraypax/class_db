#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // Pour ssize_t
#include "../include/table.h"
#include "../include/btree.h"


Node* root = NULL; // Déclarer l'arbre binaire

// Implémentation de getline
ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size = 0;
    int c;

    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }

    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }

    p = bufptr;

    while (c != EOF) {
        if ((p - bufptr) > (ssize_t)(size - 1)) {
            size_t new_size = size + 128;
            char *new_buf = realloc(bufptr, new_size);
            if (new_buf == NULL) {
                return -1;
            }
            p = new_buf + (p - bufptr);
            bufptr = new_buf;
            size = new_size;
        }

        *p++ = c;

        if (c == '\n') {
            break;
        }

        c = fgetc(stream);
    }

    *p = '\0';

    *lineptr = bufptr;
    *n = size;

    return p - bufptr;
}

typedef enum {
META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
	PREPARE_SUCCESS,
	PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT,
	STATEMENT_DELETE
} StatementType;

typedef struct {
  StatementType type;
} Statement;


typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;
    

Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table));
    table->num_rows = 0;
    return table;
}

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void print_prompt() { printf("db > "); }




void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read = my_getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}


void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}


MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    close_input_buffer(input_buffer);
    exit(EXIT_SUCCESS);
  } else {
    //TODO  here implement handling of other input as .exit
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
    statement->type = STATEMENT_INSERT;

    // On suppose que l'instruction est de la forme "insert <id> <name>"
    int id;
    char name[COLUMN_SIZE];

    int args = sscanf(input_buffer->buffer, "insert %d %s", &id, name);
    if (args < 2) {
        return PREPARE_UNRECOGNIZED_STATEMENT;
    }

    // Créer une nouvelle ligne
    Row new_row;
    new_row.id = id;
    strncpy(new_row.name, name, COLUMN_SIZE);

    // Insérer la ligne dans l'arbre binaire
    root = insert(root, new_row);

    return PREPARE_SUCCESS;
}

void execute_select(InputBuffer* input_buffer) {
    int id;
    // Vérifie si un ID est fourni dans la commande `select <id>`
    if (sscanf(input_buffer->buffer, "select %d", &id) == 1) {
        Node* result = search(root, id);
        if (result != NULL) {
            printf("Resultat trouve : ID: %d, Name: %s\n", result->row.id, result->row.name);
        } else {
            printf("ID %d non trouve.\n", id);
        }
    } else {
        // Si aucun ID n'est fourni, affiche tout l'arbre
        if (root == NULL) {
            printf("Aucune donnee disponible.\n");
        } else {
            printf("Donnees actuelles dans l'arbre :\n");
            inorder(root);
        }
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {

  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strncmp(input_buffer->buffer, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  if (strncmp(input_buffer->buffer, "delete", 6) == 0) {
    statement->type = STATEMENT_DELETE;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(InputBuffer* input_buffer, Statement* statement) {
    switch (statement->type) {
        case (STATEMENT_INSERT): {
            if (prepare_insert(input_buffer, statement) == PREPARE_SUCCESS) {
                printf("Insert completer.\n");
            } else {
                printf("Insert echouer.\n");
            }
            break;
        }
        case (STATEMENT_SELECT): {
            execute_select(input_buffer);
            break;
        }
	case STATEMENT_DELETE: {
            int id;
            // Extraire l'ID à supprimer depuis l'entrée utilisateur
            if (sscanf(input_buffer->buffer, "delete %d", &id) != 1) {
                printf("Syntaxe invalide. Utilisez : delete <id>\n");
            break;
            }
            root = delete(root, id); // Suppression dans l'arbre binaire
            printf("Suppression de l'ID %d effectuee.\n", id);
            break;
        }
        default:
            printf("type de declaration non reconnue.\n");
    }
}


void repl(void) {
    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("commande non reconnue '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                printf("declaration reconnue.\n");
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("clavier non reconnu au debut de '%s'.\n", input_buffer->buffer);
                continue;
        }

        execute_statement(input_buffer, &statement);
    }

    close_input_buffer(input_buffer);
}

