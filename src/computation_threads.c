#include <stdio.h>
#include <stdlib.h>
#include "computation_threads.h"

void run_periodic(float *output_data, pthread_mutex_t *lock, computation_func_ptr f)
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

        float temp;

        hour_calc = 0;
        for (hour = 0; hour < 24; hour++)
        {
            minute_calc = 0;
            for (minute = 0; minute < 60; minute++)
            {
                seconds_calc = 0;
                for (seconds = 0; seconds < 60; seconds += SLEEP_DURATION)
                {
                    temp = f();
                    pthread_mutex_lock(lock);
                    output_data[0] = temp;
                    seconds_calc += output_data[0];
                    pthread_mutex_unlock(lock);
                    // sleep(SLEEP_DURATION);
                }
                pthread_mutex_lock(lock);
                output_data[1] = seconds_calc / (60 / SLEEP_DURATION);
                minute_calc += output_data[1];
                pthread_mutex_unlock(lock);
            }
            pthread_mutex_lock(lock);
            output_data[2] = minute_calc / 60;
            hour_calc += output_data[2];
            pthread_mutex_unlock(lock);
        }
        pthread_mutex_lock(lock);
        output_data[3] = hour_calc / 24;
        pthread_mutex_unlock(lock);
    }
}

float current_cpu_loading(void)
{
    FILE *proc_stat_file;
    char line[256];
    char cpu_label[10];
    unsigned long
        init_user,
        init_nice,
        init_system,
        init_idle,
        init_iowait,
        init_irq,
        init_softirq,
        init_steal,
        init_guest,
        init_guest_nice;
    unsigned long
        post_user,
        post_nice,
        post_system,
        post_idle,
        post_iowait,
        post_irq,
        post_softirq,
        post_steal,
        post_guest,
        post_guest_nice;

    proc_stat_file = fopen("/proc/stat", "r");
    fgets(line, sizeof(line), proc_stat_file);
    fclose(proc_stat_file);

    sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           cpu_label,
           &init_user,
           &init_nice,
           &init_system,
           &init_idle,
           &init_iowait,
           &init_irq,
           &init_softirq,
           &init_steal,
           &init_guest,
           &init_guest_nice);

    sleep(SLEEP_DURATION);

    proc_stat_file = fopen("/proc/stat", "r");
    fgets(line, sizeof(line), proc_stat_file);
    fclose(proc_stat_file);
    sscanf(line, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           cpu_label,
           &post_user,
           &post_nice,
           &post_system,
           &post_idle,
           &post_iowait,
           &post_irq,
           &post_softirq,
           &post_steal,
           &post_guest,
           &post_guest_nice);

    unsigned long delta_idle, delta_non_idle;

    delta_idle = (post_idle + post_iowait) -
                 (init_idle + init_iowait);
    delta_non_idle = (post_user + post_nice + post_system + post_irq + post_softirq + post_steal + post_guest + post_guest_nice) -
                     (init_user + init_nice + init_system + init_irq + init_softirq + init_steal + init_guest + init_guest_nice);
    return ((float)delta_non_idle) * 100.0 / (delta_idle + delta_non_idle);
}

float current_memory_loading(void)
{
    sleep(SLEEP_DURATION);
    return 1.0;
}

float current_processes(void)
{
    sleep(SLEEP_DURATION);
    return 1.0;
}

void *cpu_computation(void *args)
{
    struct thread_args *data = (struct thread_args *)args;
    run_periodic(data->data_ptr, data->lock, current_cpu_loading);
}

void *memory_computation(void *args)
{
    struct thread_args *data = (struct thread_args *)args;
    run_periodic(data->data_ptr, data->lock, current_memory_loading);
}

void *processes_computation(void *args)
{
    struct thread_args *data = (struct thread_args *)args;
    run_periodic(data->data_ptr, data->lock, current_processes);
}