#ifndef MAIN_H
#define MAIN_H
#include "network.h"

typedef void (*_run_command)(char *command);

typedef void (*_run_custom_command)(char *command, char *current_command);

void set_curr_command(char *command);

#endif