#ifndef TREASURE_HUB_H
#define TREASURE_HUB_H

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

void handleCommands(const char* command);
void startMonitor();
void stopMonitor();
void waitForMonitor();
int monitorStatus();

#endif
