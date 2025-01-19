#include <stdio.h>
#include <stdlib.h>
#include "../include/repl.h"
#include <stdbool.h>
#include <string.h>
#include "../include/table.h"


int main(int argc, char* argv[], char* envp[]){

    (void)argc;
    (void)argv;
    (void)envp;
  repl();
  return 0;
  
}
