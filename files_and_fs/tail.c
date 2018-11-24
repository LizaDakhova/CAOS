#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tail(FILE* file) {
	char c;
	int current_size = 512, num_of_strings = 0, to_print = 0, new_line = 0;
	fseek(file, 0, SEEK_END);
	int len = ftell(file);
	char* buffer = malloc(current_size);
	if (buffer == NULL) { 
    	perror(NULL);
    	exit(1);
  	}
	fseek(file, -1, SEEK_END);
	if (fgetc(file) == '\n') {
		--num_of_strings;
	} else {
		new_line = 1;
	}
	for (int i = 0; i < len; i++) {
		if (i > current_size) {
			current_size *= 2;
            buffer = realloc(buffer, current_size);
            if (buffer == NULL) { 
    			perror(NULL);
    			exit(1);
  			}
		}
		fseek(file, -(i + 1), SEEK_END);
		c = fgetc(file);
		if (c == '\n') {
			if (++num_of_strings == 10) {
				break;
			}
		}
        buffer[i] = c;
        ++to_print;
	}
	for (int i = 0; i < to_print; ++i) {
		putchar(buffer[to_print - 1 - i]);
	}
	if (new_line) {
		putchar('\n');
	}

	free(buffer);
}

int main(int argc, char** argv) {
	if (argc == 1) {
		tail(stdin);
	} else {
		for (int i = 1; i < argc; ++i) {
			FILE* f;
			f = fopen(argv[i], "r");
			if(f == NULL) {
				printf("can not open file\n");
				return 1;
			} 
			if (argc > 2) {
				printf("==> %s <==\n", argv[i]);
			}
			tail(f);
			fclose(f);
		}
	}
	return 0;
}