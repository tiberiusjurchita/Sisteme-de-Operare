#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <signal.h> 
#include "treasure_manager.h"

pid_t pid = 0;

//descriptori pt comunicare cu pipe-ul
int monitorPipeRead, monitorPipeWrite;

int monitorStatus(){
    return pid > 0; //daca e > 0 inseamna ca monitorul merge, daca e 0 inseamna ca monitorul e oprit
}

void sigchldController(int signal){
    int status;
    pid_t wPid = waitpid(pid, &status, WNOHANG); //asteapta sa se termine procesul monitorului
    if(wPid > 0){
        printf("!MONITOR ENDED WITH STATUS %d!\n", status); 
        pid = 0; //resetam pid pentru monitorStatus
    }
}

void setupSigchldController(){
    struct sigaction sa;
    sa.sa_handler = sigchldController;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; //bucata de cod pentru handler

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("!ERROR!");
        exit(1);
    }
}

void startMonitor(){
    if(pid > 0){
        printf("~The monitor is already running~\n");
        return;
    }

    setupSigchldController(); //seteaza handler pentru semnal

    int toMonitor[2], fromMonitor[2], readyPipe[2]; //pipe pentru trimis, primit si sincronizat 

    if (pipe(toMonitor) == -1 || pipe(fromMonitor) == -1 || pipe(readyPipe) == -1) {
        perror("!!The pipe process failed!!");
        exit(1);
    }

    pid = fork();
    if (pid == 0) {
        
        close(toMonitor[1]);
        close(fromMonitor[0]);
        close(readyPipe[0]);

        dup2(toMonitor[0], STDIN_FILENO); //intrarile vin din pipe
        dup2(fromMonitor[1], STDOUT_FILENO); //iesirile ies in pipe
        dup2(fromMonitor[1], STDERR_FILENO); //erorile ies in pipe

        close(toMonitor[0]);
        close(fromMonitor[1]);

        write(readyPipe[1], "R", 1);
        close(readyPipe[1]);

        execl("./monitor", "monitor", NULL); //facem procesul copil sa fie executia monitorului
        perror("!!Monitor failed to start!!");
        exit(1);
    }

    close(toMonitor[0]);
    close(fromMonitor[1]);
    close(readyPipe[1]);

    char c;
    if (read(readyPipe[0], &c, 1) != 1) {
        perror("Failed to sync with monitor");
        exit(1);
    }
    close(readyPipe[0]);

    monitorPipeWrite = toMonitor[1];
    monitorPipeRead = fromMonitor[0];

    printf("~Monitor started~\n");
}

void handleCommands(const char* command){
    if(!monitorStatus()){
        printf("!The monitor is not running!\n~Please use the command --startMonitor~\n");
        return;
    }

    if(write(monitorPipeWrite, command, strlen(command)) == -1 || write(monitorPipeWrite, "\n", 1) == -1){
        perror("Failed to write to monitor");
        return;
    } //trimite comanda data si un caracter pt new line

    kill(pid, SIGUSR1);

    char buffer[5000];
    ssize_t n;
    while ((n = read(monitorPipeRead, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
        if(n < (ssize_t)(sizeof(buffer) - 1))
            break;
    }
    if(n == -1) {
        perror("Failed to read from monitor");
    } //citeste ce a primit de la monitor
}

void waitForMonitor(){
    while(monitorStatus()){
        sleep(1);
    }
}//functie pentru a astepta terminarea monitorului

void stopMonitor(){
    if(!monitorStatus()){
        printf("!Couldn't stop the monitor!\n!The monitor is not running!");
        return;
    }

    kill(pid, SIGUSR2); //semnal sa se inchida
    waitForMonitor(); //asteapta sa se inchida

    close(monitorPipeWrite);
    close(monitorPipeRead); 

    printf("~The monitor is shutting down~");
}

int main() {
    char input[2048];
    char huntId[2048];
    char treasureId[2048];

    while (1) {
        printf("\n > ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin))
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "startMonitor") == 0) {
            startMonitor();
        } 
        else if (strcmp(input, "stopMonitor") == 0) {
            stopMonitor();
        } 
        else if (strcmp(input, "listHunts") == 0) {
            handleCommands("listHunts");
        } 
        else if (strncmp(input, "listTreasures", 13) == 0) {
            if (sscanf(input, "listTreasures %s", huntId) == 1) {
                char command[2048];
                sprintf(command, "listTreasures %s", huntId);
                handleCommands(command);
            }
            else {
                printf("!Usage: listTreasures <hunt_id>\n");
            }
        } 
        else if (strncmp(input, "viewTreasure", 12) == 0) {
            if (sscanf(input, "viewTreasure %s %s", huntId, treasureId) == 2) {
                char command[2048];
                sprintf(command, "viewTreasure %s %s", huntId, treasureId);
                handleCommands(command);
            }
            else {
                printf("!Usage: viewTreasure <hunt_id> <treasure_id>\n");
            }
        }
        else if (strcmp(input, "calculateScore") == 0) {
            handleCommands("calculateScore");
        } 
        else if (strcmp(input, "exit") == 0) {
            if (monitorStatus()) {
                printf("!Cannot exit while monitor is running!\n");
            } else {
                break;
            }
        }
        else {
            printf("!Unknown command!\n");
        }
    }

    return 0;
}
