#include <assert.h>

#include "random.h"
#include "scheduler.h"

/********************* SOME PRINTING HELPERS *********************/

/**
 * Prints to standard output the original input
 * process_list is the original processes inputted (in array form)
 */
void printStart(process_t process_list[], scheduler_result_t result)
{
    printf("The original input was: %i", result.total_created_processes);

    uint32_t i = 0;
    for (; i < result.total_created_processes; ++i)
    {
        printf(" ( %i %i %i %i)", process_list[i].A, process_list[i].B,
               process_list[i].C, process_list[i].M);
    }
    printf("\n");
}

/**
 * Prints to standard output the final output
 * finished_process_list is the terminated processes (in array form) in the order they each finished in.
 */
void printFinal(process_t finished_process_list[], scheduler_result_t result)
{
    printf("The (sorted) input is: %i", result.total_created_processes);

    uint32_t i = 0;
    for (; i < result.total_finished_processes; ++i)
    {
        printf(" ( %i %i %i %i)", finished_process_list[i].A, finished_process_list[i].B,
               finished_process_list[i].C, finished_process_list[i].M);
    }
    printf("\n");
} // End of the print final function

/**
 * Prints out specifics for each process.
 * @param process_list The original processes inputted, in array form
 */
void printProcessSpecifics(process_t process_list[], scheduler_result_t result)
{
    uint32_t i = 0;
    printf("\n");
    for (; i < result.total_created_processes; ++i)
    {
        printf("Process %i:\n", process_list[i].processID);
        printf("\t(A,B,C,M) = (%i,%i,%i,%i)\n", process_list[i].A, process_list[i].B,
               process_list[i].C, process_list[i].M);
        printf("\tFinishing time: %i\n", process_list[i].finishingTime);
        printf("\tTurnaround time: %i\n", process_list[i].finishingTime - process_list[i].A);
        printf("\tI/O time: %i\n", process_list[i].currentIOBlockedTime);
        printf("\tWaiting time: %i\n", process_list[i].currentWaitingTime);
        printf("\n");
    }
} // End of the print process specifics function

/**
 * Prints out the summary data
 * process_list The original processes inputted, in array form
 */
void printSummaryData(process_t process_list[], scheduler_result_t result)
{
    uint32_t i = 0;
    double total_amount_of_time_utilizing_cpu = 0.0;
    double total_amount_of_time_io_blocked = 0.0;
    double total_amount_of_time_spent_waiting = 0.0;
    double total_turnaround_time = 0.0;
    uint32_t final_finishing_time = result.current_cycle - 1;
    for (; i < result.total_created_processes; ++i)
    {
        total_amount_of_time_utilizing_cpu += process_list[i].currentCPUTimeRun;
        total_amount_of_time_io_blocked += process_list[i].currentIOBlockedTime;
        total_amount_of_time_spent_waiting += process_list[i].currentWaitingTime;
        total_turnaround_time += (process_list[i].finishingTime - process_list[i].A);
    }

    // Calculates the CPU utilisation
    double cpu_util = total_amount_of_time_utilizing_cpu / final_finishing_time;

    // Calculates the IO utilisation
    double io_util = (double)result.total_number_of_cycles_spent_blocked / final_finishing_time;

    // Calculates the throughput (Number of processes over the final finishing time times 100)
    double throughput = 100 * ((double)result.total_created_processes / final_finishing_time);

    // Calculates the average turnaround time
    double avg_turnaround_time = total_turnaround_time / result.total_created_processes;

    // Calculates the average waiting time
    double avg_waiting_time = total_amount_of_time_spent_waiting / result.total_created_processes;

    printf("Summary Data:\n");
    printf("\tFinishing time: %i\n", result.current_cycle - 1);
    printf("\tCPU Utilisation: %6f\n", cpu_util);
    printf("\tI/O Utilisation: %6f\n", io_util);
    printf("\tThroughput: %6f processes per hundred cycles\n", throughput);
    printf("\tAverage turnaround time: %6f\n", avg_turnaround_time);
    printf("\tAverage waiting time: %6f\n", avg_waiting_time);
} // End of the print summary data function

/// The first number in the file is the total number of processes
/// Panics if the file is not formatted correctly
uint32_t read_process_amount(FILE *f)
{
    uint32_t total_num_of_process = 0;
    assert(fscanf(f, "%u", &total_num_of_process) == 1);
    return total_num_of_process;
}

/// Reads the processes from the file
/// Panics if the file is not formatted correctly
void read_processes(FILE *f, process_t *process_list, uint32_t total_num_of_process)
{
    for (uint32_t i = 0; i < total_num_of_process; ++i)
    {
        int scanned = fscanf(f, "%u (%u %u %u)",
                             &process_list[i].A,
                             &process_list[i].B,
                             &process_list[i].C,
                             &process_list[i].M);

        assert(scanned == 4);
        process_list[i].processID = i;
    }
}

int main(int argc, char *argv[])
{
    // #region OPEN_FILE
    if (argc < 2)
    {
        printf("Please provide a file name.\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Failed to open the file.\n");
        return 1;
    }
    // #endregion OPEN_FILE

    // #region READ_PROCESSES
    uint32_t total_num_of_process = read_process_amount(f);
    process_t process_list[total_num_of_process];
    read_processes(f, process_list, total_num_of_process);
    // #endregion READ_PROCESSES

    qsort(process_list, total_num_of_process, sizeof(process_t), cmpr_process);

    fcfs(process_list, total_num_of_process);

    fclose(f);
    return 0;
}