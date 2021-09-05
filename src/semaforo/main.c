#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../file_manager/manager.h"
#include <stdbool.h>


 int my_pid;
 int parent_pid;
 int interval;

  void handle_sigalrm(int sig)
  {
    printf("I'm the semaforo %d and I have changed the light\n",my_pid);
    send_signal_with_int(parent_pid,my_pid);
    alarm(interval);
  }

int main(int argc, char const *argv[])
{
  my_pid = getpid();
  parent_pid = getppid();
  interval = atoi(argv[0]);

  printf("I'm the SEMAFORO process and my PID is: %i and my interval is %d\n", my_pid,interval);

  // Conectamos SIGALRM a handle_sigalrm
  signal(SIGALRM, handle_sigalrm);

  alarm(interval);
  while(true){
  }


}
