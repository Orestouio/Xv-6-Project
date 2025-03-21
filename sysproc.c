#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
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
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_setpriority(void)
{
  int pid, priority;
  if (argint(0, &pid) < 0 || argint(1, &priority) < 0)
  {
    cprintf("sys_setpriority: argint failed\n");
    return -1;
  }
  if (priority < 0 || priority > 10)
  {
    cprintf("sys_setpriority: priority %d out of range\n", priority);
    return -1; // Enforce 0-10 range
  }

  struct proc *p;
  acquire(&ptable.lock);
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->pid == pid && p->state != UNUSED)
    {
      p->priority = priority;
      // cprintf("Set pid %d to priority %d\n", pid, priority);
      release(&ptable.lock);
      return 0;
    }
  }
  cprintf("sys_setpriority: pid %d not found\n", pid);
  release(&ptable.lock);
  return -1; // Process not found
}