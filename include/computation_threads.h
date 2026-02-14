#include <pthread.h>
#include <unistd.h>

#define SLEEP_DURATION 1
#define CURRENT 0
#define MINUTE 1
#define HOUR 2
#define DAY 3

// Shared memory pointers for inter thread communications
extern float *cpu_loading;
extern float *memory_loading;
extern float *active_processes;

// Locks for inter thread communications
extern pthread_mutex_t *cpu_lock_ptr;
extern pthread_mutex_t *memory_lock_ptr;
extern pthread_mutex_t *processes_lock_ptr;

struct thread_args
{
    float *data_ptr;
    pthread_mutex_t *lock;
};

typedef float (*computation_func_ptr)(void);

// Generic periodic function that will execute computation function and track Current, Minute, Hour, and Day averages
void run_periodic(float *output_data, pthread_mutex_t *lock, computation_func_ptr f);

// Computation function to calculate CPU usage using /proc/stat
float current_cpu_loading(void);

// Computation function to calculate Memory usage using /proc/meminfo
float current_memory_loading(void);

// Computation function to calculate active processes in /proc/
float current_processes(void);

// CPU loading thread function
void *cpu_computation(void *args);

// Memory usage thread function
void *memory_computation(void *args);

// Active processes thread function
void *processes_computation(void *args);