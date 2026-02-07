#include <pthread.h>
#include <unistd.h>

#define SLEEP_DURATION 5

// Definitions for shared pointers to background computation threads
extern float *cpu_loading;
extern float *memory_loading;
extern float *active_processes;

typedef float (*computation_func_ptr)(void);

void run_periodic(float *output_data, computation_func_ptr f)
{
    for (int i = 0; i < 4; i++)
    {
        output_data[i] = f();
    }

    int counter = 0;
    while (1)
    {
        int hour;
        int minute;
        int seconds;

        float hour_calc;
        float minute_calc;
        float seconds_calc;

        hour_calc = 0;
        for (hour = 0; hour < 24; hour++)
        {
            minute_calc = 0;
            for (minute = 0; minute < 60; minute++)
            {
                seconds_calc = 0;
                for (seconds = 0; seconds < 60; seconds += SLEEP_DURATION)
                {
                    output_data[0] = f();
                    seconds_calc += output_data[0];
                    sleep(SLEEP_DURATION);
                }
                output_data[1] = seconds_calc / (60 / SLEEP_DURATION);
                minute_calc += output_data[1];
            }
            output_data[2] = minute_calc / 60;
            hour_calc += output_data[2];
        }
        output_data[3] = hour_calc / 24;
    }
}

float current_cpu_loading(void)
{
    return 1.0;
}

float current_memory_loading(void)
{
    return 1.0;
}

float current_processes(void)
{
    return 1.0;
}

void *cpu_tracker(void *args)
{
    run_periodic(cpu_loading, current_cpu_loading);
}

void *memory_computation(void *args)
{
    run_periodic(memory_loading, current_memory_loading);
}

void *processes_computation(void *args)
{
    run_periodic(active_processes, current_processes);
}