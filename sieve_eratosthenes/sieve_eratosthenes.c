#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

char* primes;

void* fill_array(void* args_for_fill_array) {
	long long * args = (long long *) args_for_fill_array;
	long long sieve_size = args[0];
	long long prime = args[1];
	long long i = args[2];
	long long num_threads = args[3];

	long long start = prime * (i + 2);
	for (int j = start; j < sieve_size; j += prime * num_threads) {
		primes[j] = 1;
	}
	free(args_for_fill_array);
	pthread_exit(NULL);
}

void sieve_eratosthenes(int argc, char** argv) {

	long long num_threads, sieve_size;
	num_threads = atoll(argv[1]);
	if (argc > 2) {
		sieve_size = atoll(argv[2]) + 1;
	} else {
		sieve_size = 2000000001;
	}

	primes = (char*)calloc(sieve_size, sizeof(char));
	if (primes == NULL) {
		perror("calloc");
		exit(1);
	}

	long long args_for_fill_array[4];

	long long j_end = sqrt(sieve_size);
	long long k = 2;

	for (long long j = 2; j <= j_end; ++j) {
		if (primes[j] == 0) {
			pthread_t threads[num_threads];
			for (long long i = 0; i < num_threads; ++i) {
				long long* args_for_fill_array = malloc(4 * sizeof(long long));
				args_for_fill_array[0] = sieve_size;
				args_for_fill_array[1] = j; //prime
				args_for_fill_array[2] = i;
				args_for_fill_array[3] = num_threads;
				pthread_create(threads + i, NULL, fill_array, (void*) args_for_fill_array);
			}
			for (long long i = 0; i < num_threads; ++i) {
				pthread_join(threads[i], NULL);
			}
		}
		if (j - k > 100) {
			for (; k < j; ++k) {
				if (primes[k] == 0) {
					printf("%lld\n", k);
				}
			}
		}
	}

	for (k; k < sieve_size; ++k) {
		if (primes[k] == 0) {
			printf("%lld\n", k);
		}
	}

/*	for (int t = 0;  t <sieve_size; ++t) {
		printf("%d ", primes[t]);
	}*/
	free(primes);
}

int main(int argc, char** argv) {
	if (argc < 1) {
		perror("argc");
		exit(1);
	}

	sieve_eratosthenes(argc, argv);
	
	return 0;
}
