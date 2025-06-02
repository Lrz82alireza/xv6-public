#include "types.h"
#include "user.h"

#define NUM_CUSTOMERS 10
#define FIRST_SPAWN_INTERVAL 0.5
#define SECOND_SPAWN_INTERVAL 5

int
main(void)
{
  barber_init();

  int barber_pid = fork();
  if (barber_pid < 0) {
    printf(1, "Error: failed to fork barber\n");
    exit();
  } else if (barber_pid == 0) {
    char *argv[] = { "barber", 0 };
    exec("barber", argv);
    printf(1, "Error: failed to exec barber\n");
    exit();
  }

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    sleep(FIRST_SPAWN_INTERVAL);

    int pid = fork();
    if (pid < 0) {
      printf(1, "Error: failed to fork customer %d\n", i);
    } else if (pid == 0) {
      int result = customer_arrive();
      exit();
    }
  }

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    wait();
  }

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    sleep(SECOND_SPAWN_INTERVAL);
    int pid = fork();
    if (pid == 0) {
      customer_arrive();
      exit();
    }
  }

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    wait();
  }

  // kill(barber_pid);
  exit();
}
