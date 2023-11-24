#include "include/str.h"

#define LOG_PATH "log/"

int fd = -1;

void open_log(char *file)
{
    fd = open(str_make(3, LOG_PATH, file, ".txt"), O_CREAT | O_RDWR);
    if (fd < 0)
        die("err open file!\n");
}

char *str_make(int n, ...)
{
    if (n <= 0)
        return NULL;
    va_list ptr;
    va_start(ptr, n);
    char *new_arg = va_arg(ptr, char *);
    char *res = malloc(strlen(new_arg));
    strcat(res, new_arg);
    for (int i = 1; i < n; i++)
    {
        new_arg = va_arg(ptr, char *);
        res = realloc(res, strlen(new_arg) + strlen(res));
        strcat(res, new_arg);
    }
    return res;
}

char *to_str(int data)
{
    if (data == 0)
        return "0";
    char *res = malloc(sizeof(char) * 10);
    int isneg = 0;
    if (data < 0)
    {
        isneg = 1;
        res[0] = '-';
        data *= -1;
        res = res + sizeof(char);
    }
    int i = 0;
    while (data > 0)
    {
        res[i] = data % 10 + '0';
        i++;
        data /= 10;
    }
    int j = 0;
    while (i / 2 > j)
    {
        char tmp = res[j];
        res[j] = res[i - 1 - j];
        res[i - 1 - j] = tmp;
        j++;
    }
    res[i] = 0;
    if (isneg)
        res -= sizeof(char);
    return res;
}

char *set_color(int color, const char *message)
{
    return str_make(5, "\033[", to_str(color), "m", message, "\033[97m");
}

void writer_fd(const int fd, const char *message)
{
    write(fd, message, strlen(message));
}

void writer(const char *message)
{
    if (fd > 0)
        writer_fd(fd, message);
    writer_fd(STDOUT_FILENO, message);
}

void die(const char *message)
{
    writer(set_color(RED, message));
    exit(EXIT_FAILURE);
}

void set_ready(int is_ready)
{
    if (is_ready == 1)
        writer(">> ");
    else if (is_ready == 2)
        writer("-> ");
    else
        writer("\b\b\b");
}

char *reader_fd(int fd)
{
    char *buf = malloc(INPUT_BUF_SIZE * sizeof(char));
    int n = read(fd, buf, INPUT_BUF_SIZE * sizeof(char));
    buf[n] = 0;
    buf[strcspn(buf, "\n")] = 0;
    return buf;
}

char *reader()
{
    return reader_fd(STDIN_FILENO);
}

char *split(char **message, const char *splitter)
{
    int index = strcspn(*message, splitter);
    (*message)[index] = 0;
    char *res = *message;
    (*message) += index + 1;
    return res;
}