# xv6
I added a system call get_page_fault_stats to display the no. of page faults and the cow page faults.I took a global variable cow_cnt to count the no. of cow page faults.

I added fau in struct proc to display the no.of page faults only when p->fau = 1 which happens when get_page_fault_stats is called.
I added 2 user programs readtest and writetest to check the no.of page faults when read-only processes and processes that modify memory are executed.

