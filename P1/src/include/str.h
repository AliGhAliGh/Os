#ifndef STR_H
#define STR_H

#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define INPUT_BUF_SIZE 200
#define RED 91
#define GREEN 92
#define YELLOW 93
#define BLUE 94
#define CYAN 96
#define PINK 95

void open_log(char *file);

char *split(char **message, const char *splitter);

char *str_make(int n, ...);

char *to_str(int data);

char *set_color(int color, const char *message);

void writer_fd(const int fd, const char *message);

void writer(const char *message);

void die(const char *message);

void set_ready(int is_ready);

char *reader_fd(int fd);

char *reader();

#endif