#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MEMORY_SIZE (1024 * 1024)  // 1MB

void write_modifying_process() {
    // Allocate and modify memory
    char *data = (char *)malloc(MEMORY_SIZE);
    for (int i = 0; i < 1000000; i++) {
        data[i % MEMORY_SIZE] = (char)i;
    }
    free(data);
}

int main() {
    get_page_fault_stats();
    write_modifying_process();
    exit(0);
}