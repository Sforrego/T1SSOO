#include <stdio.h>
#include <unistd.h>
#include "../file_manager/manager.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

int pos_semaforos[4] = {0, 0, 0, 0};
int state_semaforos[3] = {0, 0, 0};
int current_pos = 0;
int next_semaforo = 0;
int my_pid;
int indice;
int tiempo = 0;
int tiempo_sem1 = -1;
int tiempo_sem2 = -1;
int tiempo_sem3 = -1;
int tiempo_bodega = -1;
int avanzar = 0;
char archivo[32];

void handle_sigabrt_repartidor(int sig)
{
  FILE *f = fopen(archivo, "w");
  fprintf(f, "%d,%d,%d,%d\n", tiempo_sem1, tiempo_sem2, tiempo_sem3, tiempo_bodega);
  fclose(f);
  exit(0);
}

void handle_sigalrm_repartidor(int sig)
{
  avanzar = 1;
}

void handle_sigusr1(int sig, siginfo_t *siginfo, void *context)
{
  int number_received = siginfo->si_value.sival_int;
  switch (state_semaforos[number_received])
  {
  case 0:
    state_semaforos[number_received] = 1;
    break;
  case 1:
    state_semaforos[number_received] = 0;
    break;
  }
}
void handle_sigint(int sig)
{
}

int main(int argc, char const *argv[])
{
  my_pid = getpid();

  // obtenemos la posición de cada semaforo y bodega
  pos_semaforos[0] = atoi(argv[0]);
  pos_semaforos[1] = atoi(argv[1]);
  pos_semaforos[2] = atoi(argv[2]);
  pos_semaforos[3] = atoi(argv[3]);
  state_semaforos[0] = atoi(argv[4]);
  state_semaforos[1] = atoi(argv[5]);
  state_semaforos[2] = atoi(argv[6]);
  indice = atoi(argv[7]);
  snprintf(archivo, sizeof archivo, "repartidor_%d.txt", indice);
  printf("REPARTIDOR %i creado, semaforos %d-%d-%d\n", my_pid, state_semaforos[0], state_semaforos[1], state_semaforos[2]);
  // Conectamos SIGALRM a handle_sigalrm
  signal(SIGALRM, handle_sigalrm_repartidor);
  signal(SIGABRT, handle_sigabrt_repartidor);
  signal(SIGINT, handle_sigint);
  connect_sigaction(SIGUSR1, handle_sigusr1);

  alarm(1);
  while (true)
  {
    if (avanzar)
    {
      tiempo++; // paso 1 segundo, se aumento el tiempo

      if (next_semaforo == 3 && current_pos + 1 == pos_semaforos[next_semaforo])
      { // reviso si llegó a la bodega
        current_pos++;
        tiempo_bodega = tiempo;
        printf("Repartidor %d llega a la bodega\n", my_pid);
        break;
      }
      else
      { // si no ha llegado a la bodega avanzar
        if (current_pos + 1 == pos_semaforos[next_semaforo])
        { // reviso si viene un semaforo
          if (next_semaforo < 3 && state_semaforos[next_semaforo] == 1)
          { // reviso que el semaforo esté en verde
            printf("Repartidor %d se encuentra con un semaforo en verde, currentpos: %d\n", my_pid, current_pos);
            switch (next_semaforo)
            { // registro cuanto tiempo se demoro en llegar al semaforo
            case 0:
              tiempo_sem1 = tiempo;
              break;
            case 1:
              tiempo_sem2 = tiempo;
              break;
            case 2:
              tiempo_sem3 = tiempo;
              break;
            }
            current_pos++;
            next_semaforo++;
          }
          else
          { // el semaforo esta en rojo
            printf("Repartidor %d espera en un semaforo en ROJO, current pos: %d\n", my_pid, current_pos);
          }
        }
        else
        { // no hay semaforo
          current_pos++;
        }
        avanzar = 0;
        alarm(1);
      }
    }
  }

  kill(my_pid, SIGABRT);
}
