#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void strings(FILE *f, const int limit) {
	char c;
	char* line = malloc(32);
	size_t index = 0, size = 32;
	while (fread(&c, 1, 1, f) == 1) {
		if (index == size) {
			size *= 2;
			line = realloc(line, size);
			if (!line) {
				perror(NULL);
				exit(-1);
			}
		}
		if (c == '\t' || c == ' ' || (c > 32 && c < 127)) {
			line[index] = c;
			++index;
		} else {
			if (index >= limit) {
				line[index] = '\0';
				puts(line);
			}
			index = 0;
		}
	}
	if (index >= limit) {
		line[index] = '\0';
		puts(line);
	}

	free(line);
}

int main (int argc, char **argv) {
	const int limit = 4;
	if (argc == 1) {
		strings(stdin, limit);
	} else {
		FILE *f;
		for (int i = 1; i < argc; ++i) {
			f = fopen(argv[i], "r");
			if (f == NULL) {
				perror(NULL);
			} else {
				strings(f, limit);
				fclose(f);
			}
		}
	}
	return 0;
}