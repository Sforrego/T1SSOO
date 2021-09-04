#include <stdio.h>
#include <unistd.h>
#include <signal.h>


int main(int argc, char const *argv[])
{
  int my_pid = getpid();
  printf("I'm the SEMAFORO process and my PID is: %i\n", getpid());
  kill(my_pid,SIGINT);
}
