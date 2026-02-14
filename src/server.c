#include <rpc/rpc.h>
#include <time.h>
#include <sys/types.h>
#include "rpc_service.h"
#include "computation_threads.h"

#define MAX_LEN 100

// Return date information to remote client
char **date_1(long *option)
{
        struct tm *timeptr; /* Pointer to time structure      */
        time_t clock;       /* Clock value (in secs)          */
        static char *ptr;   /* Return string                  */
        static char err[] = "Invalid Response \0";
        static char s[MAX_LEN];

        clock = time(0);
        timeptr = localtime(&clock);

        float uptime;
        FILE *uptimeFile;

        switch (*option)
        {
        case 1:
                strftime(s, MAX_LEN, "%A, %B %d, %Y", timeptr);
                ptr = s;
                break;

        case 2:
                strftime(s, MAX_LEN, "%T", timeptr);
                ptr = s;
                break;

        case 3:
                strftime(s, MAX_LEN, "%A, %B %d, %Y - %T", timeptr);
                ptr = s;
                break;
        case 4:
                // Get uptime of the system from /proc/uptime
                uptimeFile = fopen("/proc/uptime", "r");
                fscanf(uptimeFile, "%f", &uptime);
                fclose(uptimeFile);

                sprintf(s, "Uptime\nDays:\t%d\nHours:\t%d\nMinutes:%d\nSeconds:%d", ((int)uptime / 86400), (((int)uptime / 3600) % 24), (((int)uptime / 60) % 60), ((int)uptime % 60));
                ptr = s;
                break;

        default:
                ptr = err;
                break;
        }
        return (&ptr);
}

// Return cpu loading information to remote client
float *cpu_1(long *option)
{
        static float ret = 0.0;
        static float *ptr = &ret;

        if (*option > 0 && *option <= 4)
        {
                // Read the CPU loading information from
                // the shared memory with the computation thread
                pthread_mutex_lock(cpu_lock_ptr);
                ret = cpu_loading[*option - 1];
                pthread_mutex_unlock(cpu_lock_ptr);
        }
        else
        {
                ret = -1.0;
        }

        return (ptr);
}

// Return memory usage information to remote client
float *memory_1(long *option)
{
        static float ret = 0.0;
        static float *ptr = &ret;

        if (*option > 0 && *option <= 4)
        {
                // Read the memory usage information from
                // the shared memory with the computation thread
                pthread_mutex_lock(memory_lock_ptr);
                ret = memory_loading[*option - 1];
                pthread_mutex_unlock(memory_lock_ptr);
        }
        else
        {
                ret = -1.0;
        }

        return (ptr);
}

// Return active processes information to remote client
char **processes_1(long *option)
{
        static char *ptr; /* Return string                  */
        static char err[] = "Invalid Response \0";
        static char s[MAX_LEN];

        if (*option == 1)
        {
                // Read the number of currently active processes from
                // the shared memory with the computation thread
                pthread_mutex_lock(processes_lock_ptr);
                sprintf(s, "There are %d active processes", (int)active_processes[*option - 1]);
                pthread_mutex_unlock(processes_lock_ptr);
                ptr = s;
        }
        else if (*option > 1 && *option <= 4)
        {
                // Read the average number of active processes from
                // the shared memory with the computation thread
                pthread_mutex_lock(processes_lock_ptr);
                sprintf(s, "There were an average of %.1f active processes", active_processes[*option - 1]);
                pthread_mutex_unlock(processes_lock_ptr);
                ptr = s;
        }
        else
        {
                ptr = err;
        }

        return (&ptr);
}