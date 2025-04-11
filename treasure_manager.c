#include "treasure_manager.h"

void actionLog(const char* huntId, const char* action){
    char log[500];
    char linkName[500];

    snprintf(log, sizeof(log), "%s/%s", huntId, "loggedHunt");

    int file = open(log, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file != -1){
        dprintf(file, "%s\n", action);
        close(file);
    }

    snprintf(linkName, sizeof(linkName), "loggedHunt - %s", huntId);
    symlink(log, linkName);
}

Treasure giveTreasureDetails(){
    Treasure treasure;

    printf("Insert the treasure's ID: ");
    scanf("%d", &treasure.id);
    printf("Insert your username: ");
    scanf("%s", treasure.username);
    printf("Insert the treasure's coordinates:\n");
    printf("\tLatitude: ");
    scanf("%f", &treasure.latitude);
    printf("\tLongitude: ");
    scanf("%f", &treasure.longitude);
    printf("Insert the clue: ");
    getchar();
    fgets(treasure.clue, sizeof(treasure.clue), stdin);
    treasure.clue[strcspn(treasure.clue, "\n")] = '\0';
    printf("Insert the treasure's value: ");
    scanf("%d", &treasure.value);

    return treasure;
}

void addTreasure(const char* huntId, Treasure treasure){
    mkdir(huntId, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    char path[500];
    snprintf(path, sizeof(path), "%s/treasures.bin", huntId);
    int file = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if(file == -1){
        perror("!!Failed to open the file!!");
        return;
    }

    if(write(file, &treasure, sizeof(Treasure)) != sizeof(Treasure))
        perror("!!Failed to write in the file!!");

    close(file);
    actionLog(huntId, "Added a treasure!");
    printf("The treasure was added\n");
}

void printTreasure(const Treasure* treasure){
    printf("Treasure ID: %d\n", treasure->id);
    printf("Username: %s\n", treasure->username);
    printf("Coordinates:\n");
    printf("\t->Latitude: %.2f\n", treasure->latitude);
    printf("\t->Longitude: %.2f\n", treasure->longitude);
    printf("Clue: %s\n", treasure->clue);
    printf("Value: %d\n", treasure->value);
    printf("------------------------------------\n");
}

void listTreasures(const char* huntId){
    char path[500];
    struct stat status;
    Treasure treasure;

    snprintf(path, sizeof(path), "%s/treasures.bin", huntId);
    int file = open(path, O_RDONLY);

    if(file == -1){
        perror("!!Failed to open the file!!");
        return;
    }

    if(stat(path, &status) == -1){
        perror("!!Status error!!");
        close(file);
        return;
    }

    printf("Hunt: %s\n", huntId);
    printf("File size: %ld bytes\n", status.st_size);
    printf("Last modified: %s", ctime(&status.st_mtime));

    while (read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure))
        printTreasure(&treasure);

    close(file);
    actionLog(huntId, "Listed the treasures!");
}

void viewTreasure(const char* huntId, int id){
    char path[500];
    Treasure treasure;

    snprintf(path, sizeof(path), "%s/treasures.bin", huntId);
    int file = open(path, O_RDONLY);

    if(file == -1){
        perror("!!Failed to open the file!!");
        return;
    }

    while (read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure)){
        if(treasure.id == id){
            printTreasure(&treasure);
            close(file);
            actionLog(huntId, "Viewed a treasure!");
            return;
        }
    }

    printf("The treasure with the ID: %d was not found in the hunt %s.\n", id, huntId);
    close(file);
}

void removeTreasure(const char *huntId, int id){
    char path[500];
    char temp[500];
    Treasure treasure;
    int found = 0;

    snprintf(path, sizeof(path), "%s/treasures.bin", huntId);
    snprintf(temp, sizeof(temp), "%s/temp.bin", huntId);
    int file = open(path, O_RDONLY);
    int tempFile = open(temp, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if((file == -1) || (tempFile == -1)){
        perror("!!Failed to open one of the file!!");
        return;
    }
    
    while (read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure)){
        if(treasure.id != id)
            write(tempFile, &treasure, sizeof(treasure));
        else
            found = 1;
    }

    close(file);
    close(tempFile);

    if(found){
        rename(temp, path);
        actionLog(huntId, "Removed a treasure!");
    }
    else{
        remove(temp);
        printf("The treasure with the ID: %d was not found\n", id);
    }
    printf("The treasure was removed\n");
}

void removeHunt(const char* huntId){
    char treasure[500];
    char log[500];
    char link[500];

    snprintf(treasure, sizeof(treasure), "%s/treasures.bin", huntId);
    snprintf(log, sizeof(log), "%s/%s", huntId, "loggedHunt");
    snprintf(link, sizeof(link), "loggedHunt - %s", huntId);

    remove(treasure);
    remove(log);
    rmdir(huntId);
    remove(link);

    printf("The hunt %s was removed\n", huntId);
}

void showActionLog(const char* huntId){
    char log[500];
    char ch;
    snprintf(log, sizeof(log), "%s/loggedHunt", huntId);

    int file = open(log, O_RDONLY);
    if(file == -1){
        perror("!!Failed to open the file!!");
        return;
    }
    
    printf("Action log:\n");
    
    while(read(file, &ch, 1) == 1)
        putchar(ch);
    
    close(file);
}