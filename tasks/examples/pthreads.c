#include <pthread.h>
#include <stdio.h>
int a = 0;
int threadsNum=10;

void * f( void *p ) {
    int k = *(int *)p;
    int temp = 0;
    for( int i = k ; i < threadsNum * 500000000; i += threadsNum ) {
        a++; 
    }
    
    return NULL;
}

int main(int argc, const char * argv[]) {
    int *k = (int*)malloc(threadsNum * sizeof(int));
    for( int i = 0; i < threadsNum; ++i ) {
        k[i] = i;
    }
    
    pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * threadsNum);
    for( int i = 0; i < threadsNum; ++i ) {

        pthread_create(threads + i, NULL, f, k + i);
    }
        
    void *p;
    for( int i = 0; i < threadsNum; ++i ) {
        pthread_join(threads[i], &p);
    }
    printf("%d\n", a);
    return 0;
}

