#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

void handle_sigalrm(int sig){
    printf("i received an alarm\n");
}

int main(int argc, char const *argv[]) {
    
    signal(SIGALRM,handle_sigalrm);

    while(true){
        alarm(2);
        sleep(1);
    }


    return 0;
}