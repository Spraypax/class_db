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

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

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

void execute_select() {
    if (root == NULL) {
        printf("Aucune donnee disponible.\n");
    } else {
        printf("Donnees actuelles dans l'arbre :\n");
        inorder(root); // Parcours infixe pour afficher les données triées
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {

  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(InputBuffer* input_buffer, Statement* statement, Table* table) {
    switch (statement->type) {
        case (STATEMENT_INSERT): {
            if (prepare_insert(input_buffer, statement) == PREPARE_SUCCESS) {
                printf("Insert completed.\n");
            } else {
                printf("Insert failed.\n");
            }
            break;
        }
        case (STATEMENT_SELECT): {
            execute_select(table);
            break;
        }
        default:
            printf("Unrecognized statement type.\n");
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
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                printf("Recognized statement.\n");
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        switch (statement.type) {
            case STATEMENT_INSERT:
                if (prepare_insert(input_buffer, &statement) == PREPARE_SUCCESS) {
                    printf("Insert completed.\n");
                } else {
                    printf("Insert failed.\n");
                }
                break;

            case STATEMENT_SELECT:
                execute_select();
                break;

            default:
                printf("Unrecognized statement type.\n");
                break;
        }
    }

    close_input_buffer(input_buffer);
}
