#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "computation_threads.h"

// Generic periodic function that will execute computation function and track Current, Minute, Hour, and Day averages
void run_periodic(float *output_data, pthread_mutex_t *lock, computation_func_ptr f)
{
    // Initialize all averages to the first measurement
    for (int i = 0; i < 4; i++)
    {
        output_data[i] = f();
    }

    // Continue running computations infinitely
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
                    // Call to computation function
                    temp = f();
                    pthread_mutex_lock(lock);
                    // Save off Current value and collect for Minute calculation
                    output_data[CURRENT] = temp;
                    seconds_calc += output_data[CURRENT];
                    pthread_mutex_unlock(lock);
                }
                pthread_mutex_lock(lock);
                // Save off Minute value and collect for Hour calculation
                output_data[MINUTE] = seconds_calc / (60 / SLEEP_DURATION);
                minute_calc += output_data[MINUTE];
                pthread_mutex_unlock(lock);
            }
            pthread_mutex_lock(lock);
            // Save off Hour value and collect for Day calculation
            output_data[HOUR] = minute_calc / 60;
            hour_calc += output_data[HOUR];
            pthread_mutex_unlock(lock);
        }
        pthread_mutex_lock(lock);
        // Save off Day value
        output_data[DAY] = hour_calc / 24;
        pthread_mutex_unlock(lock);
    }
}

// Computation function to calculate CPU usage using /proc/stat
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
    // Get first line out of /proc/stat which holds all of the cpu information
    fgets(line, sizeof(line), proc_stat_file);
    fclose(proc_stat_file);

    // Parse out each of the cpu loading values for the initial measurement
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
    // Get first line out of /proc/stat which holds all of the cpu information
    fgets(line, sizeof(line), proc_stat_file);
    fclose(proc_stat_file);

    // Parse out each of the cpu loading values for the second measurement
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

    // Add up all the idle times
    delta_idle = (post_idle + post_iowait) -
                 (init_idle + init_iowait);
    // Add up all the non idle times
    delta_non_idle = (post_user + post_nice + post_system + post_irq + post_softirq + post_steal + post_guest + post_guest_nice) -
                     (init_user + init_nice + init_system + init_irq + init_softirq + init_steal + init_guest + init_guest_nice);

    // Return the percent difference between the first and last measurements
    return ((float)delta_non_idle) * 100.0 / (delta_idle + delta_non_idle);
}

// Computation function to calculate Memory usage using /proc/meminfo
float current_memory_loading(void)
{
    FILE *proc_meminfo_file;
    char line1[256];
    char line2[256];
    char mem_label[25];

    unsigned long total_memory, free_memory;

    proc_meminfo_file = fopen("/proc/meminfo", "r");

    // Get the first two lines out of /proc/meminfo for Total and Free Memory
    fgets(line1, sizeof(line1), proc_meminfo_file);
    fgets(line2, sizeof(line2), proc_meminfo_file);
    fclose(proc_meminfo_file);

    sscanf(line1, "%s %lu", mem_label, &total_memory);
    sscanf(line2, "%s %lu", mem_label, &free_memory);

    sleep(SLEEP_DURATION);

    // Return the amount of memory in use
    return (total_memory - free_memory) / 1.0;
}

// Computation function to calculate active processes in /proc/
float current_processes(void)
{

    int count = 0;
    DIR *proc_directory;
    struct dirent *dir;

    proc_directory = opendir("/proc");

    // Iterate through all of the files in the /proc/ folder
    while ((dir = readdir(proc_directory)) != NULL)
    {
        int is_pid = 1;
        for (int i = 0; i < strlen(dir->d_name); i++)
        {
            // Check the name of each file to see if it is a PID
            if (!isdigit(dir->d_name[i]))
            {
                // File is not just numbers so mark it as not a process and move on
                is_pid = 0;
                break;
            }
        }
        // If file was just a PID increment the count
        count += is_pid;
    }

    closedir(proc_directory);

    sleep(SLEEP_DURATION);
    return (float)count;
}

// CPU loading thread function
void *cpu_computation(void *args)
{
    // Upack data pointer and lock then start periodic computation
    struct thread_args *data = (struct thread_args *)args;
    run_periodic(data->data_ptr, data->lock, current_cpu_loading);
}

// Memory usage thread function
void *memory_computation(void *args)
{
    // Upack data pointer and lock then start periodic computation
    struct thread_args *data = (struct thread_args *)args;
    run_periodic(data->data_ptr, data->lock, current_memory_loading);
}

// Active processes thread function
void *processes_computation(void *args)
{
    // Upack data pointer and lock then start periodic computation
    struct thread_args *data = (struct thread_args *)args;
    run_periodic(data->data_ptr, data->lock, current_processes);
}