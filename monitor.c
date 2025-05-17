#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "treasure_manager.h"


void handleSIGUSR2(int signal) {
    printf("~Shutting down~\n");
    usleep(5000000);
    exit(0);
}

int main() {
    dup2(3, STDIN_FILENO);
    dup2(4, STDOUT_FILENO);
    dup2(4, STDERR_FILENO);
    close(3);
    close(4);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    struct sigaction sa2;
    sa2.sa_handler = handleSIGUSR2;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR2, &sa2, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(1);
    }


    char buffer[512];
    ssize_t bytes;

    while (1) {
        int sig;
        if (sigwait(&mask, &sig) != 0) {
            perror("sigwait");
            exit(1);
        }
    
        if (sig == SIGUSR2) {
            continue; // se va închide doar prin handler
        } 
        else if (sig == SIGUSR1) {
            // Citiți până la newline, nu doar un bloc brut
            FILE *in = fdopen(STDIN_FILENO, "r");
            if (!in) {
                perror("fdopen");
                exit(1);
            }
    
            if (!fgets(buffer, sizeof(buffer), in)) {
                perror("fgets");
                continue;
            }
    
            buffer[strcspn(buffer, "\n")] = 0;
    
            // Parsează și execută
            char command[128] = {0}, huntId[128] = {0}, treasure[128] = {0};
            int things = sscanf(buffer, "%127s %127s %127s", command, huntId, treasure);
    
            if (strcmp(command, "listHunts") == 0) {
                printf("~Listing hunts~\n");
                listHunts();
            }
            else if (strcmp(command, "listTreasures") == 0 && things >= 2) {
                printf("~Listing treasures~\n");
                listTreasures(huntId);
            }
            else if (strcmp(command, "viewTreasure") == 0 && things >= 3) {
                int treasureId = atoi(treasure);
                printf("~Opening the treasure~\n");
                viewTreasure(huntId, treasureId);
            }
            else if (strcmp(command, "calculateScores") == 0) {
                printf("~Calculating scores~\n");
                calculateScores();
            }
            else {
                printf("!Invalid command!\n");
            }
    
            fflush(stdout);
        }
    }
    return 0;
}