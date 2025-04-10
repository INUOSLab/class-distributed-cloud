#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#ifndef NUM_TASKS
#define NUM_TASKS 1000
#endif

#define FILENAME "temp_output.txt"

pthread_mutex_t file_mutex;

void* thread_worker(void* arg) {
    usleep(100000);  // 0.1초 대기

    pthread_mutex_lock(&file_mutex);
    FILE* fp = fopen(FILENAME, "a");
    if (fp) {
        fprintf(fp, "Thread %ld wrote to the file.\n", (long)pthread_self());
        fclose(fp);
    }
    pthread_mutex_unlock(&file_mutex);

    return NULL;
}

void process_worker(int id) {
    usleep(100000);  // 0.1초 대기

    FILE* fp = fopen(FILENAME, "a");
    if (fp) {
        fprintf(fp, "Process %d wrote to the file.\n", getpid());
        fclose(fp);
    }
    exit(0);
}

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void clear_file() {
    FILE* fp = fopen(FILENAME, "w");
    if (fp) fclose(fp);
}

int main() {
    struct timespec start, end;

    clear_file();
    //printf("Creating %d processes with I/O...\n", NUM_TASKS);
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_TASKS; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            process_worker(i);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_TASKS; ++i) {
        wait(NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double process_time = get_time_diff(start, end);
    printf("Process time (with I/O): %.4f seconds\n\n", process_time);

    clear_file();
    //printf("Creating %d threads with I/O...\n", NUM_TASKS);
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t threads[NUM_TASKS];
    pthread_mutex_init(&file_mutex, NULL);
    for (int i = 0; i < NUM_TASKS; ++i) {
        if (pthread_create(&threads[i], NULL, thread_worker, NULL) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < NUM_TASKS; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&file_mutex);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double thread_time = get_time_diff(start, end);
    printf("Thread time (with I/O): %.4f seconds\n\n", thread_time);

    printf("Threads were %.2fx faster than processes (with I/O).\n", process_time / thread_time);

    return 0;
}
