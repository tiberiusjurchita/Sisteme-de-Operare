#include "treasure_manager.h"


void actionLog(const char* huntId, const char* action){
    char log[500];
    char link[500];
    sprintf(log, "%s/%s", huntId, "logged_hunt");
    sprintf(link, "logged_hunt - %s", huntId);

    int file = open(log, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(file != -1){
        char buffer[500];
        sprintf(buffer, "%s\n", action);
        write(file, buffer, strlen(buffer));
        close(file);
    }

    symlink(log, link);
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
    sprintf(path, "%s/treasures.bin", huntId);
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
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void listTreasures(const char* huntId){
    char path[500];
    struct stat stats;
    Treasure treasure;

    sprintf(path, "%s/treasures.bin", huntId);
    int file = open(path, O_RDONLY);

    if(file == -1){
        perror("!!Failed to open the file!!");
        return;
    }

    if(stat(path, &stats) == -1){
        perror("!!stats error!!");
        close(file);
        return;
    }

    printf("Hunt: %s\n", huntId);
    printf("File size: %ld bytes\n", stats.st_size);
    printf("Last modified: %s", ctime(&stats.st_mtime));

    while (read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure))
        printTreasure(&treasure);

    close(file);
    actionLog(huntId, "Listed the treasures!");
}

void listTreasureIds(const char* huntId){
    char path[500];
    Treasure treasure;
    sprintf(path, "%s/treasures.bin", huntId);

    int file = open(path, O_RDONLY);
    if(file == -1){
        perror("!!Failed to open the file!!");
        return;
    }

    printf("List of treasure Id's from this hunt:\n");
    while(read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure))
        printf("->%d\n", treasure.id);
    close(file);
}

void viewTreasure(const char* huntId, int id){
    char path[500];
    Treasure treasure;

    sprintf(path, "%s/treasures.bin", huntId);
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

    printf("The treasure was not found in the hunt.\n");
    close(file);
}

void removeTreasure(const char *huntId, int id){
    char path[500];
    char temp[500];
    Treasure treasure;
    int found = 0;

    sprintf(path, "%s/treasures.bin", huntId);
    sprintf(temp, "%s/temp.bin", huntId);
    int file = open(path, O_RDONLY);
    int tempFile = open(temp, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if((file == -1) || (tempFile == -1)){
        perror("!!Failed to open one of the file!!");
        return;
    }
    
    while (read(file, &treasure, sizeof(Treasure)) == sizeof(Treasure)){
        if(treasure.id != id) //daca id-ul gasit nu este cel introds
            write(tempFile, &treasure, sizeof(treasure));//il copiem in temp
        else
            found = 1;//daca este gasit, nu il copiem si doar marcam ca a fost gasit
    }

    close(file);
    close(tempFile);

    if(found){//daca a fost gasit id-ul cautat
        rename(temp, path);//redenumim temp(unde nu se afla comoara pe care voiam sa o eliminam)cu treasures
        actionLog(huntId, "Removed a treasure!");//scriem in action log ce facem
    }
    else{
        remove(temp);//daca nu este gasit stergem temp si lasam fisierul original
        printf("The treasure with the ID: %d was not found\n", id);
    }
    printf("The treasure was removed\n");
}

void removeHunt(const char* huntId){
    char treasure[500];
    char log[500];
    char link[500];

    sprintf(treasure, "%s/treasures.bin", huntId);
    sprintf(log, "%s/%s", huntId, "logged_hunt");
    sprintf(link, "logged_hunt - %s", huntId);

    remove(treasure);
    remove(log);
    rmdir(huntId);
    remove(link);

    printf("The hunt %s was removed\n", huntId);
}

void showActionLog(const char* huntId){
    char log[500];
    char ch;

    sprintf(log, "%s/logged_hunt", huntId);

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