# System Call Enhancements and Scheduling Algorithms

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
   
   make clean
   make qemu CPUS=1 SCHEDULER=MLFQ
   
3. **For Lottery-Based Scheduling (LBS):**
   make clean
   make qemu CPUS=2 SCHEDULER=LBS
4. **For Default:**
   make clean
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

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
