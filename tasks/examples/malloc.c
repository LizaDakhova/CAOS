#include <stdlib.h>

size_t N=1000L*1000*1000*10;

int main(){
  size_t i=1;
  char * s;
  s=malloc(i);
  while (i<N){
    i<<=1;
    s=realloc(s,i);
    if(!s){
      perror(NULL);
      exit(1);
    } 
  }
}

