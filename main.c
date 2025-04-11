#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "treasure_manager.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("!!Error!!\n");
        printf("Usage: %s <action> <hunt ID> [treasure ID]\n", argv[0]);
        exit(-1);
    }

    const char* action = argv[1];
    const char* huntId = argv[2];

    if((strcmp(action, "commandList") == 0) && (argc == 2)){
        printf("List of commands:\n");
        printf("-> --add <hunt ID> (adds a treasure to the respective hunt)\n");
        printf("-> --list <hunt ID> (lists all the treasures in the respective hunt)\n");
        printf("-> --view <hunt ID> <ID> (shows the treasure's details from the respective hunt)\n");
        printf("-> --remove <hunt ID> <ID> (removes a treasure from the respective hunt)\n");
        printf("-> --removeHunt <hunt ID> (removes the respective hunt)\n");
    }
    else if((strcmp(action, "--add") == 0) && (argc == 3)){
        Treasure treasure = giveTreasureDetails();
        addTreasure(huntId, treasure);
    }
    else if((strcmp(action, "--list") == 0) && (argc == 3))
        listTreasures(huntId);
    else if((strcmp(action, "--view") == 0) && (argc == 4)){
        int treasureId = atoi(argv[3]);
        viewTreasure(huntId, treasureId);
    }
    else if((strcmp(action, "--remove") == 0) && (argc == 4)){
        int treasureId = atoi(argv[3]);
        removeTreasure(huntId, treasureId);
    }
    else if((strcmp(action, "--removeHunt") == 0) && (argc == 3))
        removeHunt(huntId);
    else if(strcmp(action, "--log") == 0)
        showActionLog(huntId);
    else{
        printf("Invalid command\nUse commandList to see the list of usable commands\n");
        return 1;
    }
    return 0;
}
