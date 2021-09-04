#include <stdio.h>
#include <unistd.h>
#include "../file_manager/manager.h"
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

int pos_semaforos[4] = {0,0,0,0};
int current_pos=0;
int next_semaforo = 0;
int my_pid;
void handle_sigalrm(int sig)
{
    if (current_pos+1==pos_semaforos[next_semaforo]){
      printf("Repartidor %d se encuentra con un semaforo, currentpos: %d\n",my_pid,current_pos);
      // revisar si el semaforo esta en verde

      next_semaforo++;
      current_pos++;


    } else {
      current_pos++;
    }
    
}

int main(int argc, char const *argv[])
{
  my_pid = getpid();


  // obtenemos la posición de cada semaforo y bodega
  pos_semaforos[0] = atoi(argv[0]);
  pos_semaforos[1] = atoi(argv[1]);
  pos_semaforos[2] = atoi(argv[2]);
  pos_semaforos[3] = atoi(argv[3]);


  printf("I'm the REPARTIDOR process and my PID is: %i\n", my_pid);

  // Conectamos SIGALRM a handle_sigalrm
  signal(SIGALRM, handle_sigalrm);


 
  while(true){
      if (next_semaforo == 3 && current_pos+1 == pos_semaforos[next_semaforo]){
      printf("Repartidor %d llega a la bodega\n",my_pid);
      current_pos++;
      break;
    }

      alarm(1); // mandar sigalrm en 1 segundo 
      sleep(1); // no hacer nada por 1 segundo, sino se estarian overwriteando los alarm
  }

  kill(my_pid, SIGINT);

}
