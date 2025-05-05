#include <stdio.h>
#include "treasure_hub.h"

pid_t pid = 0;
int monitorStopping = 0;

int monitorStatus(){
    return pid > 0;
}

void sigchldController(int signal){
    int status;
    pid_t wPid = waitpid(pid, &status, WNOHANG);
    if(wPid > 0){
        printf("|MONITOR ENDED WITH STATUS %d|\n", status);
        pid = 0;
        monitorStopping = 0;
    }
}

void setupSigchldController(){
    struct sigaction sa;
    sa.sa_handler = sigchldController;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("!ERROR!");
        exit(1);
    }
}

void startMonitor(){
    if(pid > 0){
        printf("!The monitor is already running!");
        return;
    }

    setupSigchldController();

    pid = fork();
}
