#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <signal.h> 
#include "treasure_hub.h"
#include "treasure_manager.h"

pid_t pid = 0;
int monitorStopping = 0;

int monitorStatus(){
    return pid > 0;
}

void sigchldController(int signal){
    int status;
    pid_t wPid = waitpid(pid, &status, WNOHANG);
    if(wPid > 0){
        printf("!MONITOR ENDED WITH STATUS %d!\n", status);
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
        printf("~The monitor is already running~");
        return;
    }

    setupSigchldController();

    pid = fork();
    if(pid == 0){
        execl("./treasure", "treasure", NULL);
        perror("execl failed");
        exit(1);
    }
}

void handleCommands(const char* command){
    if(!monitorStatus()){
        printf("!The monitor is not running!\n~Please use the command --startMonitor~\n");
        return;
    }

    int file = open("command.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if(file == -1){
        perror("!!Failed to open the file!!");
        exit(1);
    }

    if(write(file, command, strlen(command)) == -1){
        perror("!!Failed to write in command.txt!!");
        close(file);
        exit(1);
    }

    if(write(file, "\n", 1) == -1){
        perror("!!Failed to go to the next line!!");
        close(file);
        exit(1);
    }

    close(file);

    kill(pid, SIGUSR1);
}

void stopMonitor(){
    if(!monitorStatus()){
        printf("!Couldn't stop the monitor!\n!The monitor is not running!");
        return;
    }

    monitorStopping = 1;
    kill(pid, SIGUSR2);
    printf("~The monitor is shutting down~");
}

void waitForMonitor(){
    while(monitorStatus()){
        sleep(1);
    }
}