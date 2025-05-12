#include "types.h"
#include "user.h"
#define INVALIDCOMMAND "invalid command\n"
#define UNABLETOMAKEAPROCESS "unable to make a process\n"
int main(int argc,char** argv)
{
    if(argc!=2)
    {
        printf(1,INVALIDCOMMAND);
        exit();
    }
    int decided_deadline=atoi(argv[1]);
    int pid=fork();
    if(pid<0)
    {
        printf(1,UNABLETOMAKEAPROCESS);
        exit();
    }
    else if(pid==0)
    {
        create_realtime_process(decided_deadline);
        printf(1,"make real time process\n");
    }
    else
    {
        set_sleep_syscall(1000);
        wait();
        printf(1,"%d\n",pid);
    }
    exit();
}