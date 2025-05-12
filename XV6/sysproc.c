#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}




int
sys_next_palindrome(void)
{
  int in_num=myproc()->tf->ebx;
  next_palindrome(in_num);
  return 0;
}

int 
sys_make_user(void) {
  int user_id;
  char* password;

  if (argint(0, &user_id) < 0)
      return -1;
  if (argstr(1, &password) < 0)
      return -1;

  return make_user(user_id, password);
}

int 
sys_login(void){
  int user_id;
  char* password;

  if (argint(0, &user_id) < 0)
      return -1;
  if (argstr(1, &password) < 0)
      return -1;

  return login(user_id , password);
}

int
sys_logout(void){
  return logout();
}

int
sys_logs(void){
  return logs();
}

int 
sys_diff(void)
{
  // get data from trap's stack
  char* file1;
  char* file2;
  if(argstr(0, &file1)<0)
    return -1;
  if(argstr(1, &file2)<0)
    return -1;

  int result = diff(file1, file2);
  return result;
}

int
sys_set_sleep_syscall(void)
{
  int input_tick;
  if(argint(0,&input_tick)<0)
    return -1;
  if(set_sleep_syscall(input_tick)==-1)
    return -1;
  return 0;
}



int
sys_get_system_time(void)
{
  struct rtcdate* current_time;
  if(argptr(0,(void*)&current_time,sizeof(*current_time))<0)
    return -1;
  cmostime(current_time);
  return 0;
}



int
sys_create_realtime_process(void) //additional
{
  int decided_deadline;
  if(argint(0,&decided_deadline)<0)
    return -1;
  return create_realtime_process(decided_deadline);
}