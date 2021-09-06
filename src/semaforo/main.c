#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../file_manager/manager.h"
#include <stdbool.h>


int my_pid;
int parent_id;
int interval;
int cambios = 0;
int my_index;
char archivo[32];

void handle_sigabrt(int sig)
{
  FILE *f = fopen(archivo, "w");
  fprintf(f, "%d\n", cambios);
  fclose(f);
  exit(0);
}

  void handle_sigalrm(int sig)
  {
    printf("Semaforo %d have changed the light\n",my_index);
    send_signal_with_int(parent_id,my_index);
    cambios++;
    alarm(interval);
  }

void handle_sigint(int sig)
{

}

int main(int argc, char const *argv[])
{
  my_pid = getpid();
  interval = atoi(argv[0]);
  my_index = atoi(argv[1]);
  parent_id = atoi(argv[2]);

  printf("I'm the SEMAFORO process and my PID is: %i and my interval is %d\n", my_pid,interval);
  snprintf(archivo, sizeof archivo, "semaforo_%d.txt", my_index);

  // Conectamos SIGALRM a handle_sigalrm
  signal(SIGALRM, handle_sigalrm);
  signal(SIGABRT, handle_sigabrt);
  signal(SIGINT, handle_sigint);
  alarm(interval);
  while(true){
  }


}