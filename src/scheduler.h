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
    uint32_t A;         // A: Arrival time of the process
    uint32_t B;         // B: Upper Bound of CPU burst times of the given random integer list
    uint32_t C;         // C: Total CPU time required
    uint32_t M;         // M: Multiplier of CPU burst time
    uint32_t processID; // The process ID given upon input read

    uint8_t status; // 0 is unstarted, 1 is ready, 2 is running, 3 is blocked, 4 is terminated

    int32_t finishingTime;         // The cycle when the the process finishes (initially -1)
    uint32_t currentCPUTimeRun;    // The amount of time the process has already run (time in running state)
    uint32_t currentIOBlockedTime; // The amount of time the process has been IO blocked (time in blocked state)
    uint32_t currentWaitingTime;   // The amount of time spent waiting to be run (time in ready state)

    uint32_t IOBurst;  // The amount of time until the process finishes being blocked
    uint32_t CPUBurst; // The CPU availability of the process (has to be > 1 to move to running)

    int32_t quantum; // Used for schedulers that utilise pre-emption

    bool isFirstTimeRunning; // Used to check when to calculate the CPU burst when it hits running mode

    struct _process *nextInBlockedList;         // A pointer to the next process available in the blocked list
    struct _process *nextInReadyQueue;          // A pointer to the next process available in the ready queue
    struct _process *nextInReadySuspendedQueue; // A pointer to the next process available in the ready suspended queue
} process_t;

int cmpr_process(process_t *a, process_t *b)
{
    if (a->A < b->A)
    {
        return -1;
    }
    else if (a->A > b->A)
    {
        return 1;
    }
    else
    {
        return 0;
    }
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
    p->CPUBurst = randomOS(p->B, p->processID, NULL);
    p->IOBurst = p->CPUBurst * p->M;
}

/// @brief  Non-premptive First-Come-First-Serve (FCFS) Scheduler
/// @param processes t A list of processes sorted by A (arrival time)
/// @param total_num_of_process The total number of processes
scheduler_result_t fcfs(process_t *processes, uint32_t total_num_of_process)
{
    scheduler_result_t r = {0}; // Result of the scheduler
    process_t *rp = NULL;       // Running process
    process_t *p = NULL;        // Process

    while (r.total_finished_processes < total_num_of_process)
    {
        const process_t *mo = rp == NULL ? processes : rp;
        for (int i = 0; i < total_num_of_process; i++)
        {
            p = &processes[((mo - processes) + i) % total_num_of_process]; // magic

            // Terminated
            if (p->status == TERMINATED)
            {
                continue;
            }

            // Blocked
            if (p->status == BLOCKED)
            {
                p->currentIOBlockedTime++;
                p->IOBurst--;

                // Blocked -> Ready
                p->status = p->IOBurst ? BLOCKED : READY;

                r.total_number_of_cycles_spent_blocked++;
            }

            // Unstarted -> Ready
            else if (p->status == UNSTARTED && p->A == r.current_cycle)
            {
                p->currentCPUTimeRun = 0;
                p->currentIOBlockedTime = 0;
                p->currentWaitingTime = 0;
                p->isFirstTimeRunning = true;

                p->status = READY;

                r.total_created_processes++;
            }

            // Ready
            if (p->status == READY)
            {
                // Ready -> Running
                if (rp == NULL)
                {
                    p->nextInReadyQueue = NULL;
                    set_bursts(p);
                    p->status = RUNNING;
                    rp = p;
                }

                // Because of the circular loop (magic) this will always be the next ready process relative to rp
                else if (rp->nextInReadyQueue == NULL)
                {
                    rp->nextInReadyQueue = p;
                }

                // Wait
                p->currentWaitingTime++;
            }

            // Running
            else if (p->status == RUNNING)
            {
                p->currentCPUTimeRun++;
                p->CPUBurst--;
                r.total_started_processes += p->isFirstTimeRunning;
                p->isFirstTimeRunning = false;

                // Running -> Terminate
                if (p->currentCPUTimeRun == p->C)
                {
                    rp = NULL;
                    p->status = TERMINATED;
                    p->finishingTime = r.current_cycle;
                    p = p->nextInReadyQueue;

                    r.total_finished_processes++;
                }

                // Running -> Block
                else if (p->CPUBurst == 0)
                {
                    rp = NULL;
                    p->status = BLOCKED;
                    p = p->nextInReadyQueue;
                }
            }
        }

        r.current_cycle++;
    }
    return r;
}