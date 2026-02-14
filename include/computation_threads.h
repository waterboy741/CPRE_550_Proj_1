#include <pthread.h>
#include <unistd.h>

#define SLEEP_DURATION 1

// Definitions for shared pointers to background computation threads
extern float *cpu_loading;
extern float *memory_loading;
extern float *active_processes;

extern pthread_mutex_t *cpu_lock_ptr;
extern pthread_mutex_t *memory_lock_ptr;
extern pthread_mutex_t *processes_lock_ptr;

struct thread_args
{
    float *data_ptr;
    pthread_mutex_t *lock;
};

typedef float (*computation_func_ptr)(void);

void run_periodic(float *output_data, pthread_mutex_t *lock, computation_func_ptr f);

float current_cpu_loading(void);

float current_memory_loading(void);

float current_processes(void);

void *cpu_computation(void *args);

void *memory_computation(void *args);

void *processes_computation(void *args);