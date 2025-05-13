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
    int pid=0;
    for (int i = 0; i < 4; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            printf(1, UNABLE_TO_CREATE_PROCESS);
            exit();
        }
        else if (pid == 0)
        {
            set_sleep_syscall(200);
            delay();
            printf(1, "\n");
            printf(1, "\n");
            exit();
        }
    }
    for(int i=0;i<8;i++)
    { 
        print_process_info();
        wait();
        
    }
    printf(1, "overall test\n");

    
    
    

    exit();
}
