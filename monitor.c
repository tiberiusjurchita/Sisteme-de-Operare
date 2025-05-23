#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "treasure_manager.h"


int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2); //bucata de cod pentru a face set de semnale

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("!!Error sigprocmask!!");
        exit(1);
    }

    FILE *fin = fdopen(STDIN_FILENO, "r");
    if (!fin) {
        perror("!!Error fdopen stdin!!");
        exit(1);
    }

    char buffer[512];

    while (1) { //ruleaza la infinit pana primeste semnal sa se inchida
        int sig;
        if (sigwait(&mask, &sig) != 0) { //blocheaza aici pana primeste unul din semnale
            perror("!!Error sigwait!!");
            exit(1);
        }

        if (sig == SIGUSR2) { //daca primeste SIGUSR2 se inchide monitorul
            printf("~Monitor shutting down~\n");
            usleep(3000000);
            fflush(stdout);
            fflush(stderr);
            exit(0);
        }
        else if (sig == SIGUSR1) { //daca primeste SIGUSR1 inseamna ca a primit o comanda
            if (!fgets(buffer, sizeof(buffer), fin)) {
                if (feof(fin)) {
                    printf("~Monitor received EOF on command pipe, shutting down~\n");
                    fflush(stdout);
                    fflush(stderr);
                    exit(0);
                } else {
                    perror("!!Error fgets command!!");
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
            else if (strcmp(command, "calculateScore") == 0) {
                printf("~Calculating score~\n\n");
                calculateScore();
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