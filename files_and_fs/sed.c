#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#define MAX_LINE_SIZE 4096

void sed(char* pattern, char* change, FILE* fp) {
	size_t capacity_= 0;
    int len;
    char *tmp0 = NULL;
    char buf[4096];
    char *dest = buf;
    int num = 4095;
    while ((len = getline(&tmp0, &capacity_, fp)) > 0){
    	char *string = tmp0;
    	char * tmp = dest;
        num--;
        const char *isstr=NULL;
        size_t lenpattern=strlen(pattern);
        size_t lenchange = strlen(change);
        while ((isstr=strstr(string,pattern))!=NULL) {
            num-=(isstr - string) + lenchange;
            if(num<1){
              break;
            }
            strncpy(dest,string,(size_t)(isstr - string));
            dest+=isstr-string;
            strncpy(dest,change,lenchange);
            dest+=lenchange;
            string = isstr + lenpattern;
        }
      for (;(*dest=*string) && (num>0);--num) {
         ++dest;
         ++string;
    }
    printf("%s\n",buf);
    free(tmp0);
    tmp0=NULL;
    }
}

int main(int argc, char** argv) {
	if (argc < 3) {
		printf("%s\n", "Not enough argumnets");
	} else if (argc == 3) {
		sed(argv[1], argv[2], stdin);
	} else {
		for (int i = 3; i < argc; ++i) {
			FILE* f = fopen(argv[i], "r");
			if (!f) {
				printf("sed: can't read %s: No such file or directory\n", argv[i]);
				break;
			} else {
				sed(argv[1], argv[2], f);
				fclose(f);
			}
		}
	}
	return 0;
}