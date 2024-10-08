#define UNIT_TEST_ENV

#include <assert.h>

#include "scheduler.h"

void assert_result(scheduler_result_t got, scheduler_result_t expected)
{
    assert(got.current_cycle == expected.current_cycle);
    assert(got.total_created_processes == expected.total_created_processes);
    assert(got.total_started_processes == expected.total_started_processes);
    assert(got.total_finished_processes == expected.total_finished_processes);
}

/**
 IN: 1 ( 0 1 5 1)

 OUT:
 Summary Data:
    Finishing time: 9
    CPU Utilisation: 0.555556
    I/O Utilisation: 0.444444
    Throughput: 11.111111 processes per hundred cycles
    Average turnaround time: 9.000000
    Average waiting time: 0.000000

**/
void test_fcfs_input1()
{
    // Arrange
    process_t processes[] = {
        {
            .A = 0,
            .B = 1,
            .C = 5,
            .M = 1,
        },
    };

    // Act
    scheduler_result_t result = fcfs(processes, 1);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 10,
        .total_created_processes = 1,
        .total_started_processes = 1,
        .total_finished_processes = 1,
    };

    assert_result(result, expected);
}

/**
IN: 2 ( 0 1 5 1) ( 0 1 5 1)
OUT: Finishing time: 10
    CPU Utilisation: 1.000000
    I/O Utilisation: 0.800000
    Throughput: 20.000000 processes per hundred cycles
    Average turnaround time: 9.500000
    Average waiting time: 0.500000
**/
void test_fcfs_input2()
{
    // Arrange
    process_t processes[] = {
        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},
    };

    // Act
    scheduler_result_t result = fcfs(processes, 2);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 11,
        .total_created_processes = 2,
        .total_started_processes = 2,
        .total_finished_processes = 2,
    };

    assert_result(result, expected);
}

/**
IN: 3 ( 0 1 5 1) ( 0 1 5 1) ( 3 1 5 1)
OUT: Finishing time: 16
    CPU Utilisation: 0.937500
    I/O Utilisation: 0.750000
    Throughput: 18.750000 processes per hundred cycles
    Average turnaround time: 12.666667
    Average waiting time: 3.666667
**/
void test_fcfs_input3()
{
    // Arrange
    process_t processes[] = {
        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 3,
         .B = 1,
         .C = 5,
         .M = 1},
    };

    // Act
    scheduler_result_t result = fcfs(processes, 3);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 17,
        .total_created_processes = 3,
        .total_started_processes = 3,
        .total_finished_processes = 3,
    };

    assert_result(result, expected);
}

/**
 IN: 1 ( 0 1 5 1)

 OUT:
 Summary Data:
    Finishing time: 9
    CPU Utilisation: 0.555556
    I/O Utilisation: 0.444444
    Throughput: 11.111111 processes per hundred cycles
    Average turnaround time: 9.000000
    Average waiting time: 0.000000

**/
void test_sjf_input1()
{
    // Arrange
    process_t processes[] = {
        {
            .A = 0,
            .B = 1,
            .C = 5,
            .M = 1,
        },
    };

    // Act
    scheduler_result_t result = sjf(processes, 1);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 10,
        .total_created_processes = 1,
        .total_started_processes = 1,
        .total_finished_processes = 1,

    };

    assert_result(result, expected);
}

/**
IN: 2 ( 0 1 5 1) ( 0 1 5 1)
OUT: Finishing time: 10
    CPU Utilisation: 1.000000
    I/O Utilisation: 0.800000
    Throughput: 20.000000 processes per hundred cycles
    Average turnaround time: 9.500000
    Average waiting time: 0.500000
**/
void test_sjf_input2()
{
    // Arrange
    process_t processes[] = {
        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},
    };

    // Act
    scheduler_result_t result = sjf(processes, 2);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 11,
        .total_created_processes = 2,
        .total_started_processes = 2,
        .total_finished_processes = 2,

    };

    assert_result(result, expected);
}

/**
IN: 3 ( 0 1 5 1) ( 0 1 5 1) ( 3 1 5 1)
OUT: Finishing time: 19
    CPU Utilisation: 0.789474
    I/O Utilisation: 0.631579
    Throughput: 15.789474 processes per hundred cycles
    Average turnaround time: 11.666667
    Average waiting time: 2.666667
**/
void test_sjf_input3()
{
    // Arrange
    process_t processes[] = {
        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 3,
         .B = 1,
         .C = 5,
         .M = 1},
    };

    // Act
    scheduler_result_t result = sjf(processes, 3);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 20,
        .total_created_processes = 3,
        .total_started_processes = 3,
        .total_finished_processes = 3,

    };

    assert_result(result, expected);
}

/**
IN: 1 ( 0 1 5 1)
OUT: Finishing time: 9
    CPU Utilisation: 0.555556
    I/O Utilisation: 0.444444
    Throughput: 11.111111 processes per hundred cycles
    Average turnaround time: 9.000000
    Average waiting time: 0.000000

**/
void test_rr_input1()
{
    // Arrange
    process_t processes[] = {
        {
            .A = 0,
            .B = 1,
            .C = 5,
            .M = 1,
        },
    };

    // Act
    scheduler_result_t result = rr(processes, 1, 2);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 10,
        .total_created_processes = 1,
        .total_started_processes = 1,
        .total_finished_processes = 1,

    };

    assert_result(result, expected);
}

/**
IN: 2 ( 0 1 5 1) ( 0 1 5 1)
OUT: Finishing time: 10
    CPU Utilisation: 1.000000
    I/O Utilisation: 0.800000
    Throughput: 20.000000 processes per hundred cycles
    Average turnaround time: 9.500000
    Average waiting time: 0.500000
*/
void test_rr_input2()
{
    // Arrange
    process_t processes[] = {
        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},
    };

    // Act
    scheduler_result_t result = rr(processes, 2, 2);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 11,
        .total_created_processes = 2,
        .total_started_processes = 2,
        .total_finished_processes = 2,

    };

    assert_result(result, expected);
}

/**
IN: 3 ( 0 1 5 1) ( 0 1 5 1) ( 3 1 5 1)
OUT: Finishing time: 16
    CPU Utilisation: 0.937500
    I/O Utilisation: 0.750000
    Throughput: 18.750000 processes per hundred cycles
    Average turnaround time: 12.666667
    Average waiting time: 3.666667
**/
void test_rr_input3()
{
    // Arrange
    process_t processes[] = {
        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 0,
         .B = 1,
         .C = 5,
         .M = 1},

        {.A = 3,
         .B = 1,
         .C = 5,
         .M = 1},
    };

    // Act
    scheduler_result_t result = rr(processes, 3, 2);

    // Assert
    scheduler_result_t expected = {
        .current_cycle = 17,
        .total_created_processes = 3,
        .total_started_processes = 3,
        .total_finished_processes = 3,

    };

    assert_result(result, expected);
}

int main()
{
    test_fcfs_input1();
    test_fcfs_input2();
    test_fcfs_input3();

    test_sjf_input1();
    test_sjf_input2();
    test_sjf_input3();

    test_rr_input1();
    test_rr_input2();
    test_rr_input3();

    return 0;
}