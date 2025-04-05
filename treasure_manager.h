#ifndef TREASURE_MANAGER_H
#define TREASURE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>

#define LOG_FILE "logged_hunt"

typedef struct {
    int id;
    char username[50];
    float latitude;
    float longitude;
    char clue[256];
    int value;
} Treasure;

void logAction(const char *huntId, const char *action);
void addTreasure(const char *huntId, Treasure treasure);

#endif
