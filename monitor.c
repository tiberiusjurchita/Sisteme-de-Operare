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
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("monitor: sigprocmask");
        exit(1);
    }

    FILE *in = fdopen(STDIN_FILENO, "r");
    if (!in) {
        perror("monitor: fdopen stdin");
        exit(1);
    }

    char buffer[512];

    while (1) {
        int sig;
        if (sigwait(&mask, &sig) != 0) {
            perror("monitor: sigwait");
            exit(1);
        }

        if (sig == SIGUSR2) {
            printf("~Monitor shutting down~\n");
            usleep(3000000);
            fflush(stdout);
            fflush(stderr);
            exit(0);
        }
        else if (sig == SIGUSR1) {
            if (!fgets(buffer, sizeof(buffer), in)) {
                if (feof(in)) {
                    printf("~Monitor received EOF on command pipe, shutting down~\n");
                    fflush(stdout);
                    fflush(stderr);
                    exit(0);
                } else {
                    perror("monitor: fgets command");
                    continue;
                }
            }

            buffer[strcspn(buffer, "\n")] = 0;

            char command[128] = {0}, huntId[128] = {0}, treasure[128] = {0};
            int things = sscanf(buffer, "%127s %127s %127s", command, huntId, treasure);

            if (strcmp(command, "listHunts") == 0) {
                printf("\n~Listing hunts~\n\n");
                listHunts();
            }
            else if (strcmp(command, "listTreasures") == 0 && things >= 2) {
                printf("\n~Listing treasures for hunt %s~\n\n", huntId);
                listTreasures(huntId);
            }
            else if (strcmp(command, "viewTreasure") == 0 && things >= 3) {
                int treasureId = atoi(treasure);
                printf("\n~Opening treasure %d in hunt %s~\n\n", treasureId, huntId);
                viewTreasure(huntId, treasureId);
            }
            else if (strcmp(command, "calculateScores") == 0) {
                printf("~Calculating scores~\n\n");
                calculateScores();
            }
            else {
                printf("!Invalid command: %s!\n", buffer);
            }

            fflush(stdout);
            fflush(stderr);
        }
    }

    return 0;
}