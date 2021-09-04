#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../file_manager/manager.h"
#include <stdbool.h>

int other_id;
int state_semaforos[3] = {1,1,1};
int pid_semaforos[3] = {0, 0, 0}; // id del proceso de cada semaforo
char *time_semaforos[3]; // tiempo que dura cada semaforo antes de cambiar 
char *pos_semaforos[4];
char *arg_repartidor = "./repartidor";
char *arg_semaforo = "./semaforo";

void fabrica(int time_repartidores, int total_repartidores){

  int repartidores[total_repartidores];

  // hacer un whilee true que con un alarm vaya creando los repartidores como en repartidor

  for(int i=0;i<total_repartidores;i++){
    other_id = fork();
    if(!other_id){
      execvp(arg_repartidor,pos_semaforos);
      exit(0);
    }
   else{
      sleep(time_repartidores);
       }
    };

  }


int main(int argc, char const *argv[])
{
  printf("I'm the DCCUBER process and my PID is: %i\n", getpid());
  char *filename = "input.txt";
  InputFile *data_in = read_file(filename);

  printf("Leyendo el archivo %s...\n", filename);
  
  printf("- Lineas en archivo: %i\n", data_in->len);
  printf("- Contenido del archivo:\n");

  printf("\t- ");
  for (int i = 0; i < 4; i++)
  {
    printf("%s, ", data_in->lines[0][i]);
  }
  printf("\n");

  printf("\t- ");
  for (int i = 0; i < 5; i++)
  {
    printf("%s, ", data_in->lines[1][i]);
  }
  printf("\n");


 // work


pos_semaforos[0] = data_in->lines[0][0];
pos_semaforos[1] = data_in->lines[0][1];
pos_semaforos[2] = data_in->lines[0][2];
pos_semaforos[3] = data_in->lines[0][3];
time_semaforos[0] = data_in -> lines[1][2];
time_semaforos[1] = data_in -> lines[1][3];
time_semaforos[2] = data_in -> lines[1][4];

int main_id = getpid();

// semaforos
for(int i=0;i<3;i++){
  other_id = fork();
    if(!other_id){
    char *semaforo_argv[1] = {time_semaforos[i]};
    execvp(arg_semaforo,semaforo_argv);
    exit(0);
    }}
 // fabrica

 fabrica(
   atoi(data_in->lines[1][0]),
   atoi(data_in->lines[1][1]));

pid_t wpid;
int status = 0;
while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes 




  printf("Liberando memoria...\n");
  input_file_destroy(data_in);
}
