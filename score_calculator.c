#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

typedef struct {
    int id;
    char username[100];
    float latitude;
    float longitude;
    char clue[500];
    int value;
} Treasure;

typedef struct UserScore {
    char username[100];
    int total;
    struct UserScore* next;
} UserScore;

UserScore* addOrUpdate(UserScore* head, const char* username, int value) {
    UserScore* current = head;
    while (current) { //daca userul e deja in lista
        if (strcmp(current->username, username) == 0) {
            current->total += value; //adaugam la total valoarea comorii userului
            return head;
        }
        current = current->next; //mergem mai departe
    }

    UserScore* new = (UserScore*)malloc(sizeof(UserScore)); //daca userul nu e in lista, il creeaza
    strcpy(new->username, username);
    new->total = value;
    new->next = head;
    return new;
}

void freeScores(UserScore* head) {
    while (head) {
        UserScore* tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <treasures.bin>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("!!Failed to open treasures file!!");
        return 1;
    }

    UserScore* scores = NULL;
    Treasure t;
    ssize_t bytes;
    while ((bytes = read(fd, &t, sizeof(Treasure))) == sizeof(Treasure)) {
        scores = addOrUpdate(scores, t.username, t.value); //citim din fisierul cu hunt-uri si comori si ori adaugam un user ori dam update la un user
    }

    if (bytes == -1) {
        perror("!!Error reading file!!");
        close(fd);
        freeScores(scores);
        return 1;
    }

    close(fd);

    UserScore* current = scores;
    while (current) {
        printf("%s: %d\n", current->username, current->total);
        current = current->next;
    }

    freeScores(scores);
    return 0;
}

