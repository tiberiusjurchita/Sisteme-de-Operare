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
    while (current) {
        if (strcmp(current->username, username) == 0) {
            current->total += value;
            return head;
        }
        current = current->next;
    }

    UserScore* newNode = (UserScore*)malloc(sizeof(UserScore));
    strcpy(newNode->username, username);
    newNode->total = value;
    newNode->next = head;
    return newNode;
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
    ssize_t bytesRead;
    while ((bytesRead = read(fd, &t, sizeof(Treasure))) == sizeof(Treasure)) {
        scores = addOrUpdate(scores, t.username, t.value);
    }

    if (bytesRead == -1) {
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

