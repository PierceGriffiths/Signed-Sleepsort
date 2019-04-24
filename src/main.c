#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "thread_args.h"
#include "sort_functions.h"

int main(const int argc, char *argv[]){
    if(argc == 1){
	printf("Usage: %s <space separated list of signed integers>", argv[0]);
	return EXIT_SUCCESS;
    }
    int num_negatives = 0, num_positives = 0;
    int *negatives = calloc(argc - 1, sizeof(*negatives));
    int *positives = calloc(argc - 1, sizeof(*positives));
    for(int i = 1; i < argc; ++i){
	int tempval = atoi(argv[i]);
	if(tempval < 0)
	    negatives[num_negatives++] = tempval;
	else
	    positives[num_positives++] = tempval;
    }

    if(num_positives > 0 && num_positives < argc - 1){
	int* temp_ptr = realloc(positives, num_positives * sizeof(*positives));
	if(temp_ptr != NULL)
	    positives = temp_ptr;
    }
    else if(num_positives == 0){
	free(positives);
	positives = NULL;
    }

    struct thread_args negative_args, positive_args;

    negative_args.array = negatives;
    negative_args.array_size = num_negatives;

    positive_args.array = positives;
    positive_args.array_size = num_positives;
    

    pthread_t negative_sorter, positive_sorter;
    pthread_create(&negative_sorter, NULL, sort_negatives, &negative_args);
    pthread_create(&positive_sorter, NULL, sort_positives, &positive_args);
    
    pthread_join(negative_sorter, NULL);

    pthread_join(positive_sorter, NULL);
    if(positives != NULL)
	memcpy(negatives + num_negatives, positives, num_positives * sizeof(*positives));
    free(positives);

    printf("[ ");
    const size_t total_iterations = num_negatives + num_positives - 1;
    size_t i = 0;
    for(; i < total_iterations; ++i)
	printf("%d, ", negatives[i]);
    printf("%d ]\n", negatives[i]);

    free(negatives);
    return EXIT_SUCCESS;
}
