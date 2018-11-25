#include<stdio.h>
#include <stdlib.h>

int main(){
  char * p,*s=malloc(1);
  int i=1;
  p=s;
  for(i=2;i<1000000;i++){
    s=realloc(s,i);
    if(p!=s){
      printf("%d %ld\n",i-1,s-p);
      p=s;
    }
  }
}

