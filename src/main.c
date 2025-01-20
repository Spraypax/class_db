#include <stdio.h>
#include <stdlib.h>
#include "../include/repl.h"
#include "../include/btree.h"
#include <stdbool.h>
#include <string.h>
#include "../include/table.h"


int main(int argc, char* argv[], char* envp[]){
	
	(void)envp;

	if (argc > 1 && strcmp(argv[1], "demo") == 0) {
		// mode demo
		btree_demo();
		printf("\nAppuyez sur une touche pour quitter...\n");
		getchar();
	} else {
	    // Mode interactif (REPL)
	    repl();
	}

     return 0;

}
