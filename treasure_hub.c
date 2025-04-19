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