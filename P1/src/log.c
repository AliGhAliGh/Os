#include "include/log.h"

#define LOG_PATH "log/"

int fd = -1;

void open_log(char *file)
{
    fd = open(str_make(3, LOG_PATH, file, ".txt"), O_CREAT | O_RDWR);
    if (fd < 0)
        die("err open file!\n");
}

void log_message(char *message)
{
    if (fd > 0)
        writer_fd(fd, message);
}