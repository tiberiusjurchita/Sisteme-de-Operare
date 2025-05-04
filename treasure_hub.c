#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

pid_t monitor_pid = -1;
int monitor_stopping = 0;

void sendSignalToMonitor(int sig) {
    if (monitor_pid > 0) {
        kill(monitor_pid, sig);
    } else {
        printf("Monitor is not running.\n");
    }
}

void writeCommandInFile(const char *cmd) {
    int fd = open(int fd = open(CMD_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd == -1) {
        perror("Failed to open command file");
        return;
    }
    write(fd, cmd, strlen(cmd));
    close(fd);
}
