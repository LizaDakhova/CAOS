#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

typedef long long int lli;

lli max_sieve_size;
lli num_threads;

typedef struct thread_
{
    lli p;
    lli* array;
}Primethread;

void* routine(void *thread_)
{
    Primethread *info = (Primethread*) thread_;
    lli p = info->p;
    for (lli i = p * p; i <= max_sieve_size; i = i + p){
        info->array[i] = 0;
    }
    free(info);
    pthread_exit(0);
}

lli* sieve_erath()
{
    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    if (threads==NULL) {
       perror("malloc");
       exit(EXIT_FAILURE);
    }
    lli *array = malloc(sizeof(lli) * (max_sieve_size + 1));
    if (array==NULL) {
       perror("malloc");
       free(threads);
       exit(EXIT_FAILURE);
    }
    for (lli i = 0; i <= max_sieve_size; i++)
        array[i] = 1;
    lli low_p = 3;
    int flag = 0;
    while(!flag)
    { 
        int status;
        lli number_of_thread_ = 0;
        for (lli i = 0; i < num_threads; i++)
        {
            Primethread *info= malloc(sizeof(Primethread));
            if (info==NULL) {
               perror("malloc");
               free(threads);
               free(array);
               exit(EXIT_FAILURE);
            }
            info->p = -1;
            info->array = array;
            for (;low_p < (lli) ceil(sqrt(max_sieve_size + 1)); low_p += 2)
            {
                if (array[low_p] == 1)
                {
                    info->p = low_p;
                    low_p += 2;
                    break;
                }
            }
            if(info->p == -1)
            {
                flag = 1;
                break;
            }
            number_of_thread_++;
            status = pthread_create(&threads[i], NULL, routine, info);
            if (status)
            {
                perror("pthread_create");
                free(threads);
                free(array);
                exit(EXIT_FAILURE);
            }
        }
        for (lli id=0; id < number_of_thread_; id++){
            pthread_join(threads[id], NULL);
        }
    }
    free(threads);
    return array;
}


int main(int argc, char* argv[])
{
    if (argc==1) {
       perror("input");
       exit(EXIT_FAILURE);
    }
    if (argc==2) {
       max_sieve_size=LLONG_MAX-3;
    }
    else {
       max_sieve_size=atoll(argv[2]);
    }
    num_threads = atoll(argv[1]);
    lli *array = sieve_erath();
    printf("2\n");
    for (lli number = 3; number <= max_sieve_size; number+=2)
    {
        if (array[number]){
            printf("%lld\n", number);
        }
    }
    free(array);
    return 0;
}
