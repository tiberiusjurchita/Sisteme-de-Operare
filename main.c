#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "treasure_manager.h"
#include "treasure_hub.h"

int main(int argc, char* argv[]) {
    if(argc >= 2 && strcmp(argv[1], "--commandList") == 0){
        printf("List of commands:\n");
        printf("-> --add <hunt ID> (adds a treasure to the respective hunt)\n");
        printf("-> --list <hunt ID> (lists all the treasures in the respective hunt)\n");
        printf("-> --view <hunt ID> <ID> (shows the treasure's details from the respective hunt)\n");
        printf("-> --remove <hunt ID> <ID> (removes a treasure from the respective hunt)\n");
        printf("-> --removeHunt <hunt ID> (removes the respective hunt)\n");
        printf("-> --log <hunt ID> (shows action log for the hunt)\n");
        printf("-> --treasureIds <hunt ID> (shows a list of all the treasures ID's from the respective hunt)\n");
        printf("-> --startMonitor (starts the monitor process)\n");
        printf("-> --stopMonitor (stops the monitor process)\n");
        printf("-> --listHunts (lists all hunts and treasures)\n");
        printf("-> --listTreasures (lists all treasures in a hunt)\n");
        return 0;
    }

    const char* action = argv[1];

    if(strcmp(action, "--startMonitor") == 0){
        startMonitor();
        return 0;
    } else if(strcmp(action, "--stopMonitor") == 0){
        stopMonitor();
        return 0;
    } else if(strcmp(action, "--listHunts") == 0){
        handleCommands("listHunts");
        return 0;
    }

    if(argc < 3){
        printf("!!Error!!\n");
        printf("Usage: %s <action> <hunt ID> [treasure ID]\n", argv[0]);
        exit(-1);
    }

    const char* huntId = argv[2];

    if(strcmp(action, "--add") == 0 && argc == 3){
        Treasure treasure = giveTreasureDetails();
        addTreasure(huntId, treasure);
    }
    else if(strcmp(action, "--list") == 0 && argc == 3){
        listTreasures(huntId);
    }
    else if(strcmp(action, "--remove") == 0 && argc == 4){
        int treasureId = atoi(argv[3]);
        removeTreasure(huntId, treasureId);
    }
    else if(strcmp(action, "--removeHunt") == 0 && argc == 3){
        removeHunt(huntId);
    }
    else if(strcmp(action, "--log") == 0){
        showActionLog(huntId);
    }
    else if(strcmp(action, "--treasureIds") == 0){
        listTreasureIds(huntId);
    }
    else if(strcmp(action, "--listHunts") == 0){
        handleCommands("listHunts");
    }
    else if(strcmp(action, "--listTreasures") == 0 && argc == 3){
        char cmd[500];
        snprintf(cmd, sizeof(cmd), "listTreasures %s", huntId);
        handleCommands(cmd);
    }
    else if(strcmp(action, "--view") == 0 && argc == 4){
        char cmd[500];
        snprintf(cmd, sizeof(cmd), "viewTreasure %s %s", huntId, argv[3]);
        handleCommands(cmd);
    }
    else{
        printf("Invalid command\nUse: %s --commandList to see the list of usable commands\n", argv[0]);
        return 1;
    }

    return 0;
}
