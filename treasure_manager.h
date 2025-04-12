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

typedef struct {
    int id;
    char username[100];
    float latitude;
    float longitude;
    char clue[500];
    int value;
} Treasure;

void actionLog(const char *huntId, const char *action);
Treasure giveTreasureDetails();
void addTreasure(const char *huntId, Treasure treasure);
void printTreasure(const Treasure* treasure);
void listTreasures(const char *huntId);
void listTreasureIds(const char* huntId);
void viewTreasure(const char *huntId, int id);
void removeTreasure(const char *huntId, int id);
void removeHunt(const char *huntId);
void showActionLog(const char *huntId);

#endif
