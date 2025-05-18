#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "treasure_manager.h"

// Remove this signal handler function
// void handleSIGUSR2(int signal) {
//     printf("~Shutting down~\n");
//     usleep(5000000); // Avoid sleep here
//     exit(0);
// }

int main() {
    // Using standard FDs 0, 1, 2 is more conventional
    if (dup2(3, STDIN_FILENO) == -1) { perror("dup2 stdin"); exit(1); }
    if (dup2(4, STDOUT_FILENO) == -1) { perror("dup2 stdout"); exit(1); }
    if (dup2(4, STDERR_FILENO) == -1) { perror("dup2 stderr"); exit(1); } // Optional: redirect stderr too
    close(3);
    close(4);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2); // Keep SIGUSR2 in the mask for sigwait
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    // Remove the sigaction setup for SIGUSR2
    // struct sigaction sa2;
    // sa2.sa_handler = handleSIGUSR2;
    // sigemptyset(&sa2.sa_mask);
    // sa2.sa_flags = SA_RESTART;
    // if (sigaction(SIGUSR2, &sa2, NULL) == -1) {
    //     perror("sigaction SIGUSR2");
    //     exit(1);
    // }

    char buffer[512];
    // ssize_t bytes; // Not used

    // Use FILE* for easier line reading
    FILE *in = fdopen(STDIN_FILENO, "r");
    if (!in) {
        perror("fdopen stdin");
        exit(1);
    }

    while (1) {
        int sig;
        // Wait for SIGUSR1 or SIGUSR2
        if (sigwait(&mask, &sig) != 0) {
            perror("sigwait");
            // Consider if you should exit or continue on sigwait error
            exit(1);
        }

        if (sig == SIGUSR2) {
            // Handle shutdown directly here
            printf("~Shutting down~\n");
            // No need for usleep here, just exit
            exit(0);
        }
        else if (sig == SIGUSR1) {
            // Read the command line from the pipe (now stdin)
            // Use fgets on the FILE* stream
            if (!fgets(buffer, sizeof(buffer), in)) {
                // Handle potential EOF (pipe closed unexpectedly) or read error
                if (feof(in)) {
                    printf("~Monitor received EOF on command pipe, shutting down~\n");
                    exit(0); // Parent likely died or closed pipe
                } else {
                    perror("fgets command");
                    // Depending on requirements, might continue or exit on read error
                    continue;
                }
            }

            // Remove trailing newline
            buffer[strcspn(buffer, "\n")] = 0;

            // Parse and execute
            char command[128] = {0}, huntId[128] = {0}, treasure[128] = {0};
            int things = sscanf(buffer, "%127s %127s %127s", command, huntId, treasure);

            // Use stdout (which is now the pipe to the parent)
            if (strcmp(command, "listHunts") == 0) {
                printf("~Listing hunts~\n");
                listHunts();
            }
            else if (strcmp(command, "listTreasures") == 0 && things >= 2) {
                printf("~Listing treasures for hunt %s~\n", huntId); // Added huntId to message
                listTreasures(huntId);
            }
            else if (strcmp(command, "viewTreasure") == 0 && things >= 3) {
                int treasureId = atoi(treasure);
                printf("~Opening treasure %d in hunt %s~\n", treasureId, huntId); // Added details
                viewTreasure(huntId, treasureId);
            }
            else if (strcmp(command, "calculateScores") == 0) {
                printf("~Calculating scores~\n");
                calculateScores();
            }
            else {
                printf("!Invalid command: %s!\n", buffer); // Show invalid command
            }

            // Flush output to the pipe so the parent can read it
            fflush(stdout);
            // fflush(stderr); // Also flush stderr if redirected
        }
    }

    // fclose(in); // Should be unreachable after exit(0)
    return 0; // Should be unreachable
}