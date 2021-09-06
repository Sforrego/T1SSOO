#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../file_manager/manager.h"
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>

// PROBLEMAS
// 1: Repartidores al ser creados dentro del handler alarm, no funciona sus propio handler alarm
// 2: Dentro de handle usr1 se modifica state_semaforos, pero el state_semaforos que se usa en handler alarm no se ve modificado

int other_id;
char **state_semaforos;
int pid_semaforos[3] = {0, 0, 0}; // id del proceso de cada semaforo
char *time_semaforos[3]; // tiempo que dura cada semaforo antes de cambiar 
char *info_semaforos[8]; // 4 posiciones + 3 estados + el indice del repartidor
char *arg_repartidor = "./repartidor";
char *arg_semaforo = "./semaforo";
char *semaforo_argv[3];
int repartidores_creados = 0;
int total_repartidores;
int time_repartidores;
int pid_repartidores[10000];
// estado de los semaforos, 1 es verde
int status = 0;
pid_t wpid;
int fabrica_id;
int crear_repartidor = 1;

// propaga el abrt a los repartidores
void handle_sigabrt_fabrica(){
  for(int j=0;j<repartidores_creados;j++){
    kill(pid_repartidores[j],SIGABRT);
  }
  }

// Aca recibe los cambios de semaforos
void handle_sigusr1(int sig, siginfo_t *siginfo, void *context)
{
  int number_received = siginfo->si_value.sival_int;
  if(strcmp(state_semaforos[number_received], "0") == 0){
      state_semaforos[number_received] = "1";}
  else{
      state_semaforos[number_received] = "0";
  }

  for(int j=0;j<repartidores_creados;j++){
  send_signal_with_int(pid_repartidores[j],number_received);
  }
}


void handle_sigalrm(int sig){
  crear_repartidor = 1;
  }

void handle_sigint(int sig){
  kill(fabrica_id, SIGABRT);
  for(int i =0;i<3;i++){
    kill(pid_semaforos[i],SIGABRT); // termino los semaforos
  }
  for( int i = 0; i < 3; i++){
    free(state_semaforos[i]);
  }
  free(state_semaforos); 
}

void fabrica(){ 

  // Conectamos SIGALRM y SIGABRT a sus handlers
  signal(SIGALRM, handle_sigalrm);
  signal(SIGABRT,handle_sigabrt_fabrica);
  connect_sigaction(SIGUSR1, handle_sigusr1);

  while(true){
    if (crear_repartidor){
        other_id = fork();
        if(!other_id){
            info_semaforos[4] = state_semaforos[0];
            info_semaforos[5] = state_semaforos[1];
            info_semaforos[6] = state_semaforos[2];
            char indice[50];
            sprintf(indice,"%d",repartidores_creados);
            info_semaforos[7] = indice;
            execvp(arg_repartidor,info_semaforos);
        }
        else{
            pid_repartidores[repartidores_creados] = other_id;
            repartidores_creados++;
            if (repartidores_creados<total_repartidores){ 
            alarm(time_repartidores);
            } else {
            break;
                }
            }
        crear_repartidor = 0;
        }
    }


  // wait for children to finish
  pid_t wpid;
  int status = 0;
  while ((wpid = wait(&status)) > 0);


  }


int main(int argc, char const *argv[])
{
  state_semaforos = malloc(3 * sizeof(char*));
  for( int i = 0; i < 3; i++){
    state_semaforos[i] = malloc(sizeof(char));
    strcpy(state_semaforos[i], "1");

  }
  printf("Semaforos: %s - %s - %s\n",state_semaforos[0], state_semaforos[1], state_semaforos[2]);
  printf("I'm the DCCUBER process and my PID is: %i\n", getpid());
  char *filename = argv[1];
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

// semaforos

 fabrica_id = fork();
 if(!fabrica_id){
 // fabrica
 fabrica();
 } else {
  
  signal(SIGINT, handle_sigint);

  // creamos los 3 semaforos
  for(int i=0;i<3;i++){
  other_id = fork();
    if(!other_id){
      char nsemaforo[50];
      char parent_id[50];
      sprintf(nsemaforo,"%d",i);
      sprintf(parent_id, "%d", fabrica_id);
      semaforo_argv[0] = time_semaforos[i];
      semaforo_argv[1] = nsemaforo;
      semaforo_argv[2] = parent_id;
    execvp(arg_semaforo,semaforo_argv);
    } else {
      pid_semaforos[i] = other_id;
    }}

  // espero que la fabrica termine
  waitpid(fabrica_id,NULL,0);
  
  for(int i =0;i<3;i++){
    kill(pid_semaforos[i],SIGABRT); // termino los semaforos
  } 

  pid_t wpid;
  int status = 0;
  while ((wpid = wait(&status)) > 0);

  printf("Liberando memoria...\n");
  input_file_destroy(data_in);
  for( int i = 0; i < 3; i++){
    free(state_semaforos[i]);
  }
  free(state_semaforos);
 }
 
}