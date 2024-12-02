# System Call Enhancements and Scheduling Algorithms,Copy-on-Write fork Implementation

## Overview

This project implements enhancements to system calls and scheduling algorithms in a custom operating system kernel. The primary features include counting system calls, implementing a signal alarm mechanism, and introducing two scheduling algorithms: Lottery-Based Scheduling (LBS) and Multi-Level Feedback Queue (MLFQ).

## Features

### System Call Counting

- A global array `syscall_counts` is used to track the number of times each system call is invoked.
- A function `getSysCount` retrieves the count of a specific system call based on a bitmask provided by the user.

### Signal Alarm Mechanism

- Implemented a signal alarm feature that allows processes to set an alarm that triggers after a specified number of ticks.
- Two functions, `sys_sigalarm` and `sys_sigreturn`, manage the setting and returning of signals.

### Scheduling Algorithms

1. **Lottery-Based Scheduling (LBS)**
   - Each process is assigned a number of tickets.
   - The scheduler randomly selects a "winning ticket" to determine which process runs next.
   - Processes that arrive earlier are prioritized in case of ticket ties.

2. **Multi-Level Feedback Queue (MLFQ)**
   - Processes are managed in multiple queues with different priority levels.
   - Processes can be promoted or demoted between queues based on their behavior and waiting time.
   - A boost mechanism promotes all processes to the highest priority queue after a defined period.

## Implementation Details
### how to run:
1. **For Multi-Level Feedback Queue (MLFQ):**
   make clean &&
   make qemu CPUS=1 SCHEDULER=MLFQ
   
2. **For Lottery-Based Scheduling (LBS):**
   make clean && 
   make qemu CPUS=2 SCHEDULER=LBS
   
3. **For Default:**
   make clean &&
   make qemu

### System Call Counting

- The `syscall_counts` array is defined in `sys_names.h`.
- The `getSysCount` function is declared in `defs.h` and `user.h`.
- The `sys_getSysCount` function in `sysproc.c` retrieves the count for a specific system call.

### Signal Alarm

- The `proc.h` file includes new fields to manage the alarm state and handler.
- The `sys_sigalarm` function sets the alarm interval and handler.
- The `sys_sigreturn` function restores the process's state upon returning from the signal handler.

### Scheduling

- The `proc.h` file has been extended to include fields for ticket counts and MLFQ management.
- The scheduler in `proc.c` has been modified to implement both LBS and MLFQ.

### Performance Comparison

- Performance metrics such as response time (`rtime`) and waiting time (`wtime`) were collected to compare the effectiveness of the scheduling algorithms.

| Scheduling Policy | Response Time | Waiting Time |
|--------------------|---------------|--------------|
| MLFQ               | 12            | 148          |
| Default            | 13            | 149          |
|(here Round Robin)  |               |              |
| LBS                | 13            | 149          |

## Usage

1. **Building the Kernel**
   - Follow the instructions provided in the documentation to build the kernel with the new features.
   
2. **Testing System Calls**
   - Use the provided test programs to invoke system calls and observe the counting mechanism.
   
3. **Setting Alarms**
   - Implement test cases that use the `sigalarm` and `sigreturn` system calls to verify the alarm functionality.

4. **Running Scheduling Tests**
   - Utilize test programs to create processes with different ticket counts and observe the behavior of the LBS and MLFQ scheduling.

## Implications and Considerations

- The arrival time is used to break ties in ticket counts, ensuring fairness in process scheduling.
- Care must be taken to avoid starvation of lower-priority processes and ensure that high-priority processes do not block others.

## Pitfalls

- Incorrect implementation of arrival time handling can lead to starvation.
- All processes having the same number of tickets may lead to a first-come, first-served scenario.

## Conclusion

This project demonstrates the implementation of advanced system call handling and scheduling algorithms in a custom operating system. The enhancements aim to improve process management and responsiveness.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

## Copy-On-Write Fork Implementation

### Overview

To enhance memory efficiency, a **Copy-On-Write (COW)** mechanism has been implemented for the `fork` system call. This approach allows multiple processes to share the same physical memory pages until one of them modifies a page. By deferring the actual copying of memory pages until a write operation occurs, COW significantly reduces memory usage and improves performance, especially in scenarios where processes are created frequently.

### How It Works

1. **Shared Memory Pages**: When a process calls `fork`, instead of creating a complete copy of the parent's memory space, the child process is given a reference to the same memory pages as the parent. Both processes initially share these pages.

2. **Page Fault Handling**: If either the parent or the child attempts to write to a shared page, a page fault occurs. The operating system's page fault handler then creates a private copy of the page for the process that initiated the write. This ensures that changes made by one process do not affect the other.

3. **Memory Management**: The kernel keeps track of the reference counts for each memory page. When the reference count drops to zero (i.e., no process is using the page), the memory can be safely deallocated.

### Benefits

- **Reduced Memory Usage**: By sharing pages until a modification is necessary, the overall memory footprint of processes is minimized.
- **Improved Performance**: The overhead of copying large memory spaces is eliminated, leading to faster process creation.
- **Efficient Memory Management**: The system dynamically allocates memory only when necessary, optimizing resource utilization.

### Implementation Details

- Modifications were made in the `fork` system call implementation to support COW.
- The page fault handler was updated to handle COW scenarios by checking the access type and managing page copying when a write operation is attempted.
- The reference counting mechanism ensures that memory is only freed when it is no longer in use.

### Conclusion

The implementation of Copy-On-Write for the `fork` system call is a significant enhancement that optimizes memory usage and improves overall system performance. This feature is particularly beneficial in environments where processes are frequently created and destroyed.
