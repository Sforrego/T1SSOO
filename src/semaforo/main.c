#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../file_manager/manager.h"
#include <stdbool.h>


 int my_pid;
 int parent_pid;
 int interval;
 int cambios = 0;
char archivo[32];

void handle_sigabrt(int sig)
{
  printf("ABORT TIME\n");
  FILE *f = fopen(archivo, "w");
  fprintf(f, "%d\n", cambios);
  fclose(f);
}

  void handle_sigalrm(int sig)
  {
    printf("I'm the semaforo %d and I have changed the light\n",my_pid);
    send_signal_with_int(parent_pid,my_pid);
    cambios++;
    alarm(interval);
  }

int main(int argc, char const *argv[])
{
  my_pid = getpid();
  parent_pid = getppid();
  interval = atoi(argv[0]);

  printf("I'm the SEMAFORO process and my PID is: %i and my interval is %d\n", my_pid,interval);
  snprintf(archivo, sizeof archivo, "semaforo_%d.txt", my_pid);

  // Conectamos SIGALRM a handle_sigalrm
  signal(SIGALRM, handle_sigalrm);
  signal(SIGABRT, handle_sigabrt);

  alarm(interval);
  while(true){
  }


}
