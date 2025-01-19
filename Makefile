# Chemin vers le compilateur GCC
CC = E:/bin/gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

# Fichiers source et objets
SRCS = src/main.c src/btree.c src/repl.c
OBJS = $(SRCS:.c=.o)

# Nom de l'exécutable
TARGET = class_db

# Règle par défaut
all: $(TARGET)

# Générer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compiler chaque fichier source
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers générés
clean:
	rm -f $(OBJS) $(TARGET)
