#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int cmpstr(const void* var1, const void* var2) {
	return strcmp(*(char* const*) var1, *(char* const*) var2);
}

int main() {
 	char** words_lib = (char**)calloc(64, sizeof(char*));
 	char* word;
 	size_t max_size_words_lib = 64, max_size_word = 16, index_word = 0, index_words_lib = 0;
 	size_t semicolon_counter = 0, verticalline_counter = 0, ampersand_counter = 0, 
 	except_chars_counter = 0, single_quotes_counter = 0, double_qoutes_counter = 0;
 	int ifquote = 0;
 	int c = '\0', prev_c;
 	while(1) {
 		prev_c = c;
 		c = getchar();
 		int is_space = isspace(c);

 		if (c == '\'' && !ifquote) {
 			++single_quotes_counter;
 			ifquote = 1;
 			if (prev_c == '&') {
 				char* word0 = (char*)calloc(2, sizeof(char));
		 		word0[0] = '&';
		 		word0[1] = '\0';
		 		words_lib[index_words_lib++] = word0;
		 		--ampersand_counter;
		 		--except_chars_counter;
 			}
 			else if (prev_c == '|') {
 				char* word0 = (char*)calloc(2, sizeof(char));
		 		word0[0] = '|';
		 		word0[1] = '\0';
		 		words_lib[index_words_lib++] = word0;
		 		--verticalline_counter;
		 		--except_chars_counter;
 			}
 		}
 		else if (c == '\'' && single_quotes_counter == 1) {
			single_quotes_counter = 0;
			ifquote = 0;
			if (prev_c == '\'') {
				words_lib[index_words_lib] = (char*)calloc(1, sizeof(char));
				words_lib[index_words_lib++][0] = '\0';
			}
		}
		else if (c == '"' && !ifquote) {
			++double_qoutes_counter;
			ifquote = 1;
			if (prev_c == '&') {
 				char* word0 = (char*)calloc(2, sizeof(char));
		 		word0[0] = '&';
		 		word0[1] = '\0';
		 		words_lib[index_words_lib++] = word0;
		 		--ampersand_counter;
		 		--except_chars_counter;
 			}
 			else if (prev_c == '|') {
 				char* word0 = (char*)calloc(2, sizeof(char));
		 		word0[0] = '|';
		 		word0[1] = '\0';
		 		words_lib[index_words_lib++] = word0;
		 		--verticalline_counter;
		 		--except_chars_counter;
 			}
		}
		else if (c == '"' && double_qoutes_counter == 1) {
			double_qoutes_counter = 0;
			ifquote = 0;
			if (prev_c == '"') {
				words_lib[index_words_lib] = (char*)calloc(1, sizeof(char));
				words_lib[index_words_lib++][0] = '\0';
			}
		}

		else if (ifquote) {
			if (c == EOF) {
				fprintf(stderr, "quotes error/нет баланса кавычек\n");
				printf("quotes error/нет баланса кавычек\n");
 				return 1;
			}
			//produce memory for a new word
	 		if (index_word == 0) {
	 			word = (char*)calloc(16, sizeof(char));
	 			max_size_word = 16;
	 		}
	 		//check if the size of word is out of limits
	 		else if (index_word == max_size_word - 1) /*made for ot  checking the size, while adding '\0'*/ { 
	 			word = (char*)realloc(word, max_size_word <<= 2);
	 			if (!word) {
		 			perror(NULL);
		 			return 1;
		 		}
	 		}
	 		word[index_word++] = c;
		}

		else {
	 		if (c == ';') {
	 			++semicolon_counter;
	 			++except_chars_counter;
	 		}
	 		else if (c == '|') {
	 			++verticalline_counter;
	 			++except_chars_counter;
	 		}
	 		else if (c == '&') {
	 			++ampersand_counter;
	 			++except_chars_counter;
	 		}
	 		else if (c != EOF && !is_space ) {
		 		//produce memory for a new word
		 		if (index_word == 0) {
		 			word = (char*)calloc(16, sizeof(char));
		 			max_size_word = 16;
		 		}
		 		//check if the size of word is out of limits
		 		else if (index_word == max_size_word - 1) /*made for ot  checking the size, while adding '\0'*/ { 
		 			word = (char*)realloc(word, max_size_word <<= 2);
		 			if (!word) {
		 				perror(NULL);
		 				return 1;
		 			}

		 		}
		 		word[index_word++] = c;
		 	}

	 		if (c == EOF || is_space || except_chars_counter){
	 			//check the size of words_lib
		 		if (index_words_lib == max_size_words_lib - 1) {
		 			words_lib = (char**)realloc(words_lib, sizeof(*words_lib) * (max_size_words_lib <<= 2));
		 			if (!words_lib) {
		 				perror(NULL);
		 				return 1;
		 			}
		 		}
		 		if ((index_word > 0 || c == EOF || is_space || c == ';' || c == '&') && index_words_lib && prev_c == '|' && verticalline_counter == 1) {
		 			char* word0 = (char*)calloc(2, sizeof(char));
		 			word0[0] = '|';
		 			word0[1] = '\0';
		 			words_lib[index_words_lib++] = word0;
		 			--verticalline_counter;
		 			--except_chars_counter;
		 		}
		 		else if ((index_word > 0 || c == EOF || is_space || c == ';' || c == '|') && index_words_lib && prev_c == '&' && ampersand_counter == 1) {
		 			char* word0 = (char*)calloc(2, sizeof(char));
		 			word0[0] = '&';
		 			word0[1] = '\0';
		 			words_lib[index_words_lib++] = word0;
		 			--ampersand_counter;
		 			--except_chars_counter;
		 		}
	 			else if (index_word > 0) /*store any not special word*/ {
		 			word[index_word] = '\0';
		 			words_lib[index_words_lib++] = word;
		 			index_word = 0;
	 			}
	 			if (except_chars_counter > 0) {
	 				if (semicolon_counter == 1) {
	 					word = (char*)calloc(2, sizeof(char));
	 					word[0] = ';';
	 					word[1] = '\0';
	 					words_lib[index_words_lib++] = word;
	 					--semicolon_counter;
	 					--except_chars_counter;
	 				}
	 				else if (verticalline_counter == 1 && except_chars_counter > 1) {
	 					word = (char*)calloc(2, sizeof(char));
	 					word[0] = '|';
	 					word[1] = '\0';
	 					words_lib[index_words_lib++] = word;
	 					--verticalline_counter;
	 					--except_chars_counter;
	 				}
	 				else if (verticalline_counter == 2) {
	 					word = (char*)calloc(3, sizeof(char));
	 					word[0] = '|';
	 					word[1] = '|';
	 					word[2] = '\0';
	 					words_lib[index_words_lib++] = word;
	 					verticalline_counter -= 2;
	 					except_chars_counter -= 2;
	 				}
	 				else if (ampersand_counter == 1 && except_chars_counter > 1) {
	 					word = (char*)calloc(2, sizeof(char));
	 					word[0] = '&';
	 					word[1] = '\0';
	 					words_lib[index_words_lib++] = word;
	 					--ampersand_counter;
	 					--except_chars_counter;
	 				}
	 				else if (ampersand_counter == 2) {
	 					word = (char*)calloc(3, sizeof(char));
	 					word[0] = '&';
	 					word[1] = '&';
	 					word[2] = '\0';
	 					words_lib[index_words_lib++] = word;
	 					ampersand_counter -= 2;
	 					except_chars_counter -= 2;
	 				}
	 			}
	 			if (c == EOF) break;
	 		}
	 	}
 	}

  	qsort(words_lib, index_words_lib, sizeof(char*), cmpstr);

 	for (int i = 0; i < index_words_lib; ++i) {
 		putchar('"');
 		int j = 0;
 		while (words_lib[i][j] != '\0') {
 			putchar(words_lib[i][j++]);
 		}
 		putchar('"');
 		putchar('\n');
 	}

 	for (int i = 0; i < index_words_lib; ++i) {
 		free(words_lib[i]);
 	}
 	free(words_lib);

 	return 0;
 }