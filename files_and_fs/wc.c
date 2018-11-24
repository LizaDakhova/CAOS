#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

void f_count(FILE* f, unsigned int* words_count, unsigned int* lines_count, unsigned int* symbols_count) {
    unsigned int c, is_new_word = 1;
    char * text = malloc(1);
    if (text == NULL) {
      perror("malloc error\n");
      exit(-1);
    }
    size_t text_size = 0;
    while ((c = fgetwc(f)) != EOF) {
      ++(*symbols_count);
      ++text_size;
      if (c == '\n') {
        ++(*lines_count);
      }
      text = realloc(text, text_size);
      if (text == NULL) {
        perror("realloc error\n");
        exit(-1);
      }
      text[text_size - 1] = c;
    }

    ++text_size;
    text = realloc(text, text_size);
    if (text == NULL) {
      perror("realloc error\n");
      exit(-1);
    }
    text[text_size - 1] = '\0';

    wchar_t wtext[text_size];
    mbstowcs(wtext, text, text_size);
    size_t i = 0;
    wchar_t wc;
    while ((wc = wtext[i]) != L'\0') {
      //wprintf(L"%lc", wc);
      if (iswspace(wc)) {
            is_new_word = 1;
        } else if (is_new_word) {
            ++(*words_count);
            is_new_word = 0;
        }
        ++i;
    }
    free(text);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "");

    FILE *f;
    unsigned int words_count = 0, lines_count = 0, symbols_count = 0;

    if (argc == 1) {
        f_count(stdin, &words_count, &lines_count, &symbols_count);
        printf("%d %d %d \n", lines_count, words_count, symbols_count);
    } else if (argc == 2) {
        f = fopen(argv[1], "r");
        if (f == NULL) {
            perror(argv[1]);
        } else {
            f_count(f, &words_count, &lines_count, &symbols_count);
            fclose(f);
            printf("%d %d %d %s\n", lines_count, words_count, symbols_count, argv[1]);
        }
    } else {
        unsigned int total_words_count = 0, total_lines_count = 0, total_symbols_count = 0;
        for (unsigned int i = 1; i < argc; ++i) {
            words_count = lines_count = symbols_count = 0;
            f = fopen(argv[i], "r");
            if (f == NULL) {
                perror(argv[i]);
            } else {
                f_count(f, &words_count, &lines_count, &symbols_count);
                fclose(f);
                printf("%d %d %d %s\n", lines_count, words_count, symbols_count, argv[i]);
                total_words_count += words_count;
                total_lines_count += lines_count;
                total_symbols_count += symbols_count;
            }
        }
        printf("Total: %d %d %d \n",total_lines_count, total_words_count, total_symbols_count);
    }

    return 0;
}
