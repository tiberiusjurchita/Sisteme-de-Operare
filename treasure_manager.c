#include "treasure_manager.h"

void logAction(const char *huntId, const char *action) {
    char log_path[100];
    snprintf(log_path, sizeof(log_path), "%s/%s", huntId, LOG_FILE);
    int fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd != -1) {
        dprintf(fd, "%s\n", action);
        close(fd);
    }
}

void addTreasure(const char *huntId, Treasure treasure) {
    char path[100];
    snprintf(path, sizeof(path), "%s/treasures.dat", huntId);
    
    mkdir(huntId, 0755);
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }
    
    if (write(fd, &treasure, sizeof(Treasure)) == -1) {
        perror("Error writing to file");
    }
    close(fd);
    logAction(huntId, "Added a treasure");
}