#include "../kernel/types.h"
#include "../kernel/syscall.h"
#include "user.h"

int main() {
  int tickets = 10;  // Number of tickets to set
  
  int result = settickets(tickets);  // Call system call
  if(result < 0) {
    printf("Failed to set tickets\n");
  } else {
    printf("Successfully set %d tickets\n", result);
  }
  
  exit(0);
}
