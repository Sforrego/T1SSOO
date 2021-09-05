#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../file_manager/manager.h"
#include <stdbool.h>
#include <sys/wait.h>

int other_id;
int state_semaforos[3] = {1,1,1};
int pid_semaforos[3] = {0, 0, 0}; // id del proceso de cada semaforo
char *time_semaforos[3]; // tiempo que dura cada semaforo antes de cambiar 
char *info_semaforos[7]; // 4 posiciones + 3 estados
char *arg_repartidor = "./repartidor";
char *arg_semaforo = "./semaforo";
char *semaforo_argv[1];
int repartidores_creados;
int total_repartidores;
int time_repartidores;
int pid_repartidores[10000];
// estado de los semaforos, 1 es verde
char* state1[1] = {"1"};
char* state2[1] = {"1"};
char* state3[1] = {"1"};
int status = 0;
pid_t wpid;

// Aca recibe los cambios de semaforos
void handle_sigusr1(int sig, siginfo_t *siginfo, void *context)
{
  int number_received = siginfo->si_value.sival_int;
  int position;
  for(int i = 0; i < 3; i++){
    if(pid_semaforos[i] == number_received){
      position = i;
      break;
    }
  }
  switch(state_semaforos[position]){
    case 0:
      state_semaforos[position] = 1;
      break;
    case 1:
      state_semaforos[position] = 0;
      break;
  }

  for(int j=0;j<repartidores_creados;j++){
  send_signal_with_int(pid_repartidores[j],position);
  }
}

void handle_sigalrm(int sig){
  if(repartidores_creados<total_repartidores){

    other_id = fork();
    if(!other_id){
      info_semaforos[4] = state1[0];
      info_semaforos[5] = state2[0];
      info_semaforos[6] = state3[0];
      execvp(arg_repartidor,info_semaforos);
      exit(0);
    }
   else{
      repartidores_creados++;
      pid_repartidores[repartidores_creados] = other_id;
      alarm(time_repartidores);
       }
    };

  }


void fabrica(){ 

  printf("REPARTIDORES CREADOS: %d TOTAL REPARTIDORES: %d\n",repartidores_creados,total_repartidores);
  int repartidores[total_repartidores];

    // creo el primer repartidor
    other_id = fork();
    if(!other_id){
      info_semaforos[4] = state1[0];
      info_semaforos[5] = state2[0];
      info_semaforos[6] = state3[0];
      execvp(arg_repartidor,info_semaforos);
      exit(0);
    }
   else{
      repartidores_creados++;
      pid_repartidores[0] = other_id;
      alarm(time_repartidores); // aca entro al loop de crear repartidores
       }


  // wait for children to finish
  // pid_t wpid;
  // int status = 0;
  // while ((wpid = wait(&status)) > 0);

  while(true){
    // printf("hello?\n");
  }

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
// char *state_semaforos = {'1','1','1'}; 

info_semaforos[0] = data_in->lines[0][0];
info_semaforos[1] = data_in->lines[0][1];
info_semaforos[2] = data_in->lines[0][2];
info_semaforos[3] = data_in->lines[0][3];

time_semaforos[0] = data_in -> lines[1][2];
time_semaforos[1] = data_in -> lines[1][3];
time_semaforos[2] = data_in -> lines[1][4];

total_repartidores = atoi(data_in -> lines[1][1]);
time_repartidores = atoi(data_in->lines[1][0]);
connect_sigaction(SIGUSR1, handle_sigusr1);

// semaforos

 int fabrica_id = fork();
 if(!fabrica_id){
 // fabrica
 fabrica();
 } else {

  // creamos los 3 semaforos
  for(int i=0;i<3;i++){
  other_id = fork();
    if(!other_id){
      semaforo_argv[0] = time_semaforos[i];
    execvp(arg_semaforo,semaforo_argv);
    exit(0);
    } else {
      pid_semaforos[i] = other_id;
    }}

  // espero que la fabrica termine
  waitpid(fabrica_id,NULL,0);
  
  for(int i =0;i<3;i++){
    kill(pid_semaforos[i],SIGINT); // termino los semaforos
  } 
  printf("Liberando memoria...\n");
  input_file_destroy(data_in);
 }
}
