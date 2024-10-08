#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "random.h"

typedef enum
{
    UNSTARTED = 0,
    READY = 1,
    RUNNING = 2,
    BLOCKED = 3,
    TERMINATED = 4
} process_status;

/* Defines a job struct */
typedef struct _process
{
    uint32_t A;  // A: Arrival time of the process
    uint32_t B;  // B: Upper Bound of CPU burst times of the given random integer list
    uint32_t C;  // C: Total CPU time required
    uint32_t M;  // M: Multiplier of CPU burst time
    uint32_t id; // The process ID given upon input read

    uint8_t status; // 0 is unstarted, 1 is ready, 2 is running, 3 is blocked, 4 is terminated

    int32_t finished_time; // The cycle when the the process finishes (initially -1)
    uint32_t cpu_time;     // The amount of time the process has already run (time in running state)
    uint32_t blocked_time; // The amount of time the process has been IO blocked (time in blocked state)
    uint32_t waiting_time; // The amount of time spent waiting to be run (time in ready state)

    uint32_t io_burst;  // The amount of time until the process finishes being blocked
    uint32_t cpu_burst; // The CPU availability of the process (has to be > 1 to move to running)

    int32_t quantum; // Used for schedulers that utilise pre-emption

    bool is_first_run; // Used to check when to calculate the CPU burst when it hits running mode
} process_t;

int cmpr_process_a(const void *a, const void *b)
{
    process_t *pa = (process_t *)a;
    process_t *pb = (process_t *)b;

    return pa->A - pb->A;
}

typedef struct
{
    uint32_t current_cycle;                        // The current cycle that each process is on
    uint32_t total_created_processes;              // The total number of processes constructed
    uint32_t total_started_processes;              // The total number of processes that have started being simulated
    uint32_t total_finished_processes;             // The total number of processes that have finished running
    uint32_t total_number_of_cycles_spent_blocked; // The total cycles in the blocked state
} scheduler_result_t;

/// @brief Calculates the CPU burst time and the IO burst time for a given process
static void set_bursts(process_t *p)
{
    p->cpu_burst = randomOS(p->B, p->id, NULL);
    p->io_burst = p->cpu_burst * p->M;
}

/// @brief Iterate through an array in a circular fashion, ie the tail overflows back to the head.
///
/// Starts from location memory_offset
#define circular_iterator(memory_head, memory_offset, i, total_memory) &memory_head[((memory_offset - memory_head) + i) % total_memory]

/// @brief  Non-premptive First-Come-First-Serve (FCFS) Scheduler
scheduler_result_t fcfs(process_t *processes, uint32_t total_num_of_process)
{
    qsort(processes, total_num_of_process, sizeof(process_t), cmpr_process_a);

    scheduler_result_t r = {0}; // Result of the scheduler
    process_t *rp = NULL;       // Running process
    process_t *p = NULL;        // Process

    while (r.total_finished_processes < total_num_of_process)
    {
        const process_t *mo = rp == NULL ? processes : rp;
        for (int i = 0; i < total_num_of_process; i++)
        {
            p = circular_iterator(processes, mo, i, total_num_of_process);

            // Terminated
            if (p->status == TERMINATED)
            {
                continue;
            }

            // Blocked
            if (p->status == BLOCKED)
            {
                p->blocked_time++;
                p->io_burst--;

                // Blocked -> Ready
                p->status = p->io_burst ? BLOCKED : READY;

                r.total_number_of_cycles_spent_blocked++;
            }

            // Unstarted -> Ready
            else if (p->status == UNSTARTED && p->A == r.current_cycle)
            {
                p->cpu_time = 0;
                p->blocked_time = 0;
                p->waiting_time = 0;
                p->is_first_run = true;

                p->status = READY;

                r.total_created_processes++;
            }

            // Ready
            if (p->status == READY)
            {
                // Ready -> Running
                if (rp == NULL)
                {
                    set_bursts(p);
                    p->status = RUNNING;
                    rp = p;
                }

                // Wait
                p->waiting_time++;
            }

            // Running
            else if (p->status == RUNNING)
            {
                p->cpu_time++;
                p->cpu_burst--;
                r.total_started_processes += p->is_first_run;
                p->is_first_run = false;

                // Running -> Terminate
                if (p->cpu_time >= p->C)
                {
                    p->status = TERMINATED;
                    p->finished_time = r.current_cycle;
                    rp = NULL;

                    r.total_finished_processes++;
                }

                // Running -> Block
                else if (p->cpu_burst <= 0)
                {
                    rp = NULL;
                    p->status = BLOCKED;
                }
            }
        }

        r.current_cycle++;
    }
    return r;
}

/// @brief Non-premptive Shortest Job First (SJF) Scheduler
scheduler_result_t sjf(process_t *processes, uint32_t total_num_of_process)
{
    qsort(processes, total_num_of_process, sizeof(process_t), cmpr_process_a);

    scheduler_result_t r = {0}; // Result of the scheduler
    process_t *p = NULL;

    while (r.total_finished_processes < total_num_of_process)
    {
        process_t *sj = NULL; // Shortest job

        for (int i = 0; i < total_num_of_process; i++)
        {
            p = &processes[i];

            // Terminated
            if (p->status == TERMINATED)
            {
                continue;
            }

            // Blocked
            if (p->status == BLOCKED)
            {
                p->blocked_time++;
                p->io_burst--;

                // Blocked -> Ready
                p->status = p->io_burst ? BLOCKED : READY;

                r.total_number_of_cycles_spent_blocked++;
            }

            // Unstarted -> Ready
            else if (p->status == UNSTARTED && p->A == r.current_cycle)
            {
                p->cpu_time = 0;
                p->blocked_time = 0;
                p->waiting_time = 0;
                p->is_first_run = true;

                p->status = READY;

                r.total_created_processes++;
            }

            // Ready
            if (p->status == READY)
            {
                sj = sj == NULL ? p : ((p->C - p->cpu_time) < (sj->C - sj->cpu_time) ? p : sj);
                p->waiting_time++;
            }

            // Running
            else if (p->status == RUNNING)
            {
                p->cpu_time++;
                p->cpu_burst--;
                r.total_started_processes += p->is_first_run;
                p->is_first_run = false;

                // Running -> Terminate
                if (p->cpu_time >= p->C)
                {
                    p->status = TERMINATED;
                    p->finished_time = r.current_cycle;

                    r.total_finished_processes++;
                }

                // Running -> Block
                else if (p->cpu_burst <= 0)
                {
                    p->status = BLOCKED;
                }
            }
        }

        // Ready -> Running for Shortest Job
        if (sj != NULL)
        {
            set_bursts(sj);
            sj->status = RUNNING;
        }

        r.current_cycle++;
    }
    return r;
}

/// @brief Round Robin (RR) Scheduler
/// @param quantum the time quantum for the scheduler
scheduler_result_t rr(process_t *processes, uint32_t total_num_of_process, uint8_t quantum)
{
    qsort(processes, total_num_of_process, sizeof(process_t), cmpr_process_a);

    scheduler_result_t r = {0}; // Result of the scheduler
    process_t *rp = NULL;       // Running process
    process_t *p = NULL;        // Process

    while (r.total_finished_processes < total_num_of_process)
    {
        const process_t *mo = rp == NULL ? processes : rp;
        for (int i = 0; i < total_num_of_process; i++)
        {
            p = circular_iterator(processes, mo, i, total_num_of_process);

            // Terminated
            if (p->status == TERMINATED)
            {
                continue;
            }

            // Blocked
            if (p->status == BLOCKED)
            {
                p->blocked_time++;
                p->io_burst--;

                // Blocked -> Ready
                p->status = p->io_burst ? BLOCKED : READY;

                r.total_number_of_cycles_spent_blocked++;
            }

            // Unstarted -> Ready
            else if (p->status == UNSTARTED && p->A == r.current_cycle)
            {
                p->cpu_time = 0;
                p->blocked_time = 0;
                p->waiting_time = 0;
                p->is_first_run = true;

                p->status = READY;

                r.total_created_processes++;
            }

            // Ready
            if (p->status == READY)
            {
                // Ready -> Running
                if (rp == NULL)
                {
                    set_bursts(p);
                    p->status = RUNNING;
                    rp = p;
                }

                // Wait
                p->waiting_time++;
            }

            // Running
            else if (p->status == RUNNING)
            {
                p->cpu_time++;
                p->cpu_burst -= quantum;
                r.total_started_processes += p->is_first_run;
                p->is_first_run = false;

                // Running -> Terminate
                if (p->cpu_time >= p->C)
                {
                    p->status = TERMINATED;
                    p->finished_time = r.current_cycle;
                    rp = NULL;

                    r.total_finished_processes++;
                }

                // Running -> Block
                else
                {
                    rp = NULL;
                    p->status = BLOCKED;
                }
            }
        }

        r.current_cycle++;
    }
    return r;
}