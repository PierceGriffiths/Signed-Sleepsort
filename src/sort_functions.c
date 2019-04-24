#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "thread_args.h"

#define USLEEP_SCALAR 2000

void* sort_negatives(struct thread_args *const args){
    if(args->array_size == 0)
	return NULL;

    int index_memID, *index = NULL;
    if((index_memID = shmget(IPC_PRIVATE, sizeof(*index), IPC_CREAT | 0666)) == -1){
	fprintf(stderr, "Unable to allocate shared memory segment for negative value index.\n");
	exit(EXIT_FAILURE);
    }
    else if(*(index = shmat(index_memID, NULL, 0)) == -1){
	fprintf(stderr, "Unable to attach shared memory segment for negative value index.\n");
	exit(EXIT_FAILURE);
    }

    int array_memID, *sorted = NULL;
    if((array_memID = shmget(IPC_PRIVATE, args->array_size * sizeof(*sorted), IPC_CREAT | 0666)) == -1){
	fprintf(stderr, "Unable to allocate shared memory segment for array of sorted negative integers.\n");
	exit(EXIT_FAILURE);
    }
    else if(*(sorted = shmat(array_memID, NULL, 0)) == -1){
	fprintf(stderr, "Unable to attach shared memory segment for array of sorted negative integers.\n");
	exit(EXIT_FAILURE);
    }
    
    *index = args->array_size;
    int i = -1;
    const pid_t original_ID = syscall(SYS_gettid);
    while (++i < args->array_size - 1 && !fork());
    usleep(-USLEEP_SCALAR * args->array[i]);
    sorted[--*index] = args->array[i];
    wait(NULL);
    if(syscall(SYS_gettid) == original_ID){//if this is the "root" negative sort process, then copy the sorted array over the unsorted array, then detach and deallocate shared memory
	wait(NULL);
	shmdt(index);
	shmctl(index_memID, IPC_RMID, NULL);
	memcpy(args->array, sorted, args->array_size * sizeof(*sorted));
	shmdt(sorted);
	shmctl(array_memID, IPC_RMID, NULL);
    }
    return NULL;
}

void* sort_positives(struct thread_args *const args){
    if(args->array_size == 0)
	return NULL;

    int index_memID, *index = NULL;
    if((index_memID = shmget(IPC_PRIVATE, sizeof(*index), IPC_CREAT | 0666)) == -1){
	fprintf(stderr, "Unable to allocate shared memory segment for positive value index.\n");
	exit(EXIT_FAILURE);
    }
    else if(*(index = shmat(index_memID, NULL, 0)) == -1){
	fprintf(stderr, "Unable to attach shared memory segment for positive value index.\n");
	exit(EXIT_FAILURE);
    }
    
    int array_memID, *sorted = NULL;
    if((array_memID = shmget(IPC_PRIVATE, args->array_size * sizeof(*sorted), IPC_CREAT | 0666)) == -1){
	fprintf(stderr, "Unable to allocate shared memory segment for array of sorted positive integers.\n");
	exit(EXIT_FAILURE);
    }
    else if(*(sorted = shmat(array_memID, NULL, 0)) == -1){
	fprintf(stderr, "Unable to attach shared memory segment for array of sorted positive integers.\n");
	exit(EXIT_FAILURE);
    }

    *index = -1;
    int i = -1;
    const pid_t original_ID = syscall(SYS_gettid);
    while (++i < args->array_size - 1 && !fork());
    usleep(USLEEP_SCALAR * args->array[i]);
    sorted[++*index] = args->array[i];
    wait(NULL);
    if(syscall(SYS_gettid) == original_ID){//if this is the "root" positive sort process, then copy the sorted array over the unsorted array, then detach and deallocate shared memory
	wait(NULL);
	shmdt(index);
	shmctl(index_memID, IPC_RMID, NULL);
	memcpy(args->array, sorted, args->array_size * sizeof(*sorted));
	shmdt(sorted);
	shmctl(array_memID, IPC_RMID, NULL);
    }
    return NULL;
}
