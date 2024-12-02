#include "../kernel/types.h"
#include "../kernel/syscall.h"
#include "user.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: syscount <mask> <command> [args...]\n");
    exit(1);
  }

  int mask = atoi(argv[1]);
  getSysCount(mask);

  int pid_child = fork();
  if (pid_child < 0) {
    printf("Fork failed\n");
    exit(1);
  }

  if (pid_child == 0) {
    exec(argv[2], &argv[2]);
    printf("Exec failed\n");
    exit(1);
  } else {
    wait(0);

    int syscall_count = getSysCount(mask);
    printf("%d times.\n", syscall_count);
    exit(0);
  }
}