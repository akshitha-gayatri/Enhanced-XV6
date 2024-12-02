#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PROCESSES 10
#define MEMORY_SIZE (1024 * 1024)  // 1MB

void child_process(int is_read_only) {
    // Read or modify memory
    char *data = (char *)malloc(MEMORY_SIZE);
    if (is_read_only) {
        for (int i = 0; i < 100; i++) {
            data[i % MEMORY_SIZE] = (char)i;
        }
    }

    free(data);
}

int main() {
    get_page_fault_stats();
    for (int i = 0; i < NUM_PROCESSES; i++) {
        int pid = fork();
        if (pid == 0) {
            child_process(1);  
            exit(0);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        int status;
        wait(&status);
    }

    exit(0);
}
