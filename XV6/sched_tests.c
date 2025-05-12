#include "types.h"
#include "user.h"
#define UNABLE_TO_CREATE_PROCESS "unable to create a process\n"
enum class_and_level {WITHOUT_PRIORITY, EARLIEST_DEADLINE_FIRST, MULTILEVEL_FEEDBACK_QUEUE_FIRST_LEVEL, MULTILEVEL_FEEDBACK_QUEUE_SECOND_LEVEL};
int REAL_TIME_PROCESS_DEADLINE = 500; 

void delay() {
    volatile int i, j;
    int slices = 50;
    for(i = 0; i < slices; i++) {
        for(j = 0; j < 5000; j++) {
            asm("nop");
        }
        if (i % 10 == 0) {
            printf(1, ".");
        }
    }
    printf(1, "\n");
}

int main(void)
{
    int pid1 = fork();
    if(pid1 < 0) {
        printf(1, UNABLE_TO_CREATE_PROCESS);
        exit();
    }
    
    if(pid1 == 0) {
        delay();
        printf(1 , "\n");
        print_process_info();
        printf(1 , "\n");
        printf(1, "PID 1 Test\n");
        exit();
    }

    int pid2 = fork();
    if(pid2 < 0) {
        printf(1, UNABLE_TO_CREATE_PROCESS);
        exit();
    }

    if(pid2 == 0) {
        delay();
        change_process_queue(getpid(), MULTILEVEL_FEEDBACK_QUEUE_FIRST_LEVEL);
        printf(1, "Process %d moved to MLFQ Level 1 (RR)\n", getpid());
        delay();
        printf(1 , "\n");
        print_process_info();
        printf(1 , "\n");
        printf(1, "PID 2 Test\n");
        exit();
    }

    int pid3 = fork();
    if(pid3 < 0) {
        printf(1, UNABLE_TO_CREATE_PROCESS);
        exit();
    }

    if(pid3 == 0) {
        delay();
        create_realtime_process(REAL_TIME_PROCESS_DEADLINE);
        printf(1, "Process %d moved to EDF\n", getpid());
        delay();
        printf(1 , "\n");
        print_process_info();
        printf(1 , "\n");
        printf(1, "PID 3 Test\n");
        exit();
    }
   
    delay();
    printf(1 , "\n");
    print_process_info();
    printf(1 , "\n");
    printf(1, "overall test\n");

    wait();
    wait();
    wait();

    exit();
}
