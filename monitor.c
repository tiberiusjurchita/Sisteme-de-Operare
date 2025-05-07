#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "treasure_manager.h"

void handleSIGUSR1(int signal){
    int file = open("command.txt", O_RDONLY);
    if(file == -1){
        perror("!Failed to open the file!");
        exit(1);
    }

    char buffer[500];
    ssize_t bytes = read(file, buffer, sizeof(buffer) - 1);
    if(bytes <= 0){
        perror("!Failed to read the command!");
        exit(1);
    }

    buffer[bytes] = '\0';
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "\r")] = 0;

    char command[500], huntId[500], treasure[500];
    int things = sscanf(buffer, "%s %s %s", command, huntId, treasure);

    if(strcmp(command, "listHunts") == 0){
        printf("~Listing hunts~\n");
        listHunts();
    }
    else if(strcmp(command, "listTreasures") == 0 && things >= 2){
        printf("~Listing treasures~\n");
        listTreasures(huntId);
    }
    else if(strcmp(command, "viewTreasure") == 0 && things >= 3){
        printf("~Opening the treasure~\n");
        int treasureId = atoi(treasure);
        viewTreasure(huntId, treasureId);
    }
    else {
        printf("!Invalid command!\n");
    }

    close(file);
}

void handleSIGUSR2(int signal) {
    printf("~Shutting down~\n");
    usleep(5000000);
    exit(0);
}

int main() {
    struct sigaction sa1, sa2;
    sa1.sa_handler = handleSIGUSR1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = SA_RESTART;

    sa2.sa_handler = handleSIGUSR2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa1, NULL) == -1 || sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("~Monitor started~\n~Waiting for commands~\n");

    while (1) {
        pause();
    }

    return 0;
}
