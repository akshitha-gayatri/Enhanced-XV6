#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sys_names.h"

const char *syscall_names[] = {"",
                               "fork",        
                               "exit",       
                               "wait",        
                               "pipe",       
                               "read",        
                               "kill",        
                               "exec",        
                               "fstat",       
                               "chdir",       
                               "dup",         
                               "getpid",      
                               "sbrk",        
                               "sleep",       
                               "uptime",     
                               "open",      
                               "write",       
                               "mknod",       
                               "unlink",     
                               "link",       
                               "mkdir",       
                               "close",      
                               "waitx",      
                               "getSysCount", 
                               "settickets",
                               "sigalarm",
                               "sigreturn"

};

uint64 sys_settickets(void) {
  int num;
  argint(0, &num);
  if (num < 1)
    return -1;
  myproc()->tickets = num;
  // printf("Setting %d tickets for process %d\n", num, myproc()->pid);
  return num;
}

uint64 sys_getSysCount(void) {
  struct proc *p = myproc();
  int mask2 = p->trapframe->a0;

  int syscall_num = -1;
  for (int i = 0; i < 64; i++) {
    if (mask2 == (1 << i)) {
      syscall_num = i;
      break;
    }
  }

  if (syscall_num == -1) {
    printf("Invalid mask: %d\n", mask2);
    return -1;
  }
  int op = syscall_counts[syscall_num];
  for (int i = 0; i < 32; i++)
    syscall_counts[i] = 0;
  if (p->flagg > 0)
    printf("PID %d called %s ", p->pid, syscall_names[syscall_num]);
  p->flagg++;
  return op;
}

uint64 sys_exit(void) {
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return fork(); }

uint64 sys_wait(void) {
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64 sys_sbrk(void) {
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64 sys_sleep(void) {
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_waitx(void) {
  uint64 addr, addr1, addr2;
  uint wtime, rtime;
  argaddr(0, &addr);
  argaddr(1, &addr1); // user virtual memory
  argaddr(2, &addr2);
  int ret = waitx(addr, &wtime, &rtime);
  struct proc *p = myproc();
  if (copyout(p->pagetable, addr1, (char *)&wtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr2, (char *)&rtime, sizeof(int)) < 0)
    return -1;
  return ret;
}

uint64 sys_sigreturn(void) {
  struct proc *p = myproc();

  if (p->backup_trapframe == 0 || p->pending_signal == 0)
    return -1;
  p->pending_signal = 0;
  p->alarm_ticks = 0;
  memmove(p->trapframe, p->backup_trapframe, PGSIZE); // to save context
  p->trapframe->epc =
      p->backup_trapframe
          ->epc; // to restore the original values of the registers
  return p->trapframe->a0;
}

uint64 sys_sigalarm(void) {
  int ticks;
  struct proc *p = myproc();

  ticks = p->trapframe->a0;
  if (ticks < 0 || p->pending_signal == 1) {
    // Either an alarm is already pending, do nothing or interval is < 0
    return -1;
  }
  p->pending_signal = 0;
  p->alarm_ticks = ticks;
  p->handler = p->trapframe->a1;
  return 0;
}