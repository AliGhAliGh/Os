#include "include/network.h"

struct termios *oldt = NULL;
struct sockaddr_in bc_address;

int accept_client(int server_fd)
{
    struct sockaddr_in client_address;
    int address_len = sizeof(struct sockaddr_in);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
    if (client_fd < 0)
        die("err accept!");

    return client_fd;
}

void set_showing(int show, fd_set *master)
{
    if (oldt == NULL)
    {
        oldt = malloc(sizeof(struct termios));
        tcgetattr(STDIN_FILENO, oldt);
    }
    struct termios new = *oldt;
    if (!show)
    {
        new.c_lflag &= ~ECHO;
        FD_CLR(STDIN_FILENO, master);
    }
    else
        FD_SET(STDIN_FILENO, master);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

char *get_type(int receiver, char *type)
{
    char *message = malloc(sizeof(char) * INPUT_BUF_SIZE);
    int n = recv(receiver, message, sizeof(char) * INPUT_BUF_SIZE, 0);
    message[n] = 0;
    if (n == -1)
        die("err recv!\n");
    *type = message[0];
    memmove(message, message + sizeof(char) * 2, sizeof(char) * (n - 1));
    return message;
}

void send_message(char *group, char *message, int broadcaster)
{
    char *sendString = str_make(3, group, "|", message);
    int len = strlen(sendString);
    int n = sendto(broadcaster, sendString, strlen(sendString), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));
    if (n < len)
        die("send err!\n");
}

void handle_username(char *username, char *othername, int broadcaster)
{
    // writer(str_make(3, "other name : ", othername, "\n"));
    if (username == NULL || strcmp(username, othername) != 0)
        return;
    send_message(BAD_USER, othername, broadcaster);
}

void add_opt_val(int fd, int opt, const void *opt_val, int len)
{
    if (setsockopt(fd, SOL_SOCKET, opt, opt_val, len) == -1)
        die("err opt val!\n");
}

void add_opt(int fd, int opt)
{
    add_opt_val(fd, opt, &(int){1}, sizeof(int));
}

void cancel_alarm()
{
    alarm(0);
}

int get_broadcaster(int port)
{
    int broadcaster;
    if ((broadcaster = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        die("err socket!\n");
    add_opt(broadcaster, SO_REUSEADDR);
    add_opt(broadcaster, SO_BROADCAST);

    memset(&bc_address, 0, sizeof(bc_address));
    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(port);
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    // struct timeval timeout;
    // timeout.tv_sec = 1;
    // timeout.tv_usec = 0;
    // add_opt_val(broadcaster, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // broadcaster->sender = socket(AF_INET, SOCK_DGRAM, 0);

    // if (broadcaster->sender < 0)
    //     die("err socket!\n");
    // add_opt(broadcaster->sender, SO_BROADCAST);

    // if (fcntl(broadcaster, F_SETFL, fcntl(broadcaster, F_GETFL) | O_NONBLOCK) < 0)
    //     die("err nonblock\n");
    // if (fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK) < 0)
    //     die("err nonblock\n");

    if (bind(broadcaster, (struct sockaddr *)&bc_address, sizeof(bc_address)) == -1)
        die("cant bind!\n");
    return broadcaster;
}

int get_tcp(int *port)
{
    int tcp;
    if ((tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("err socket!\n");
    add_opt(tcp, SO_REUSEADDR);
    add_opt(tcp, SO_BROADCAST);

    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(*port);
    recv_addr.sin_addr.s_addr = INADDR_ANY;
    while (bind(tcp, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) == -1)
        recv_addr.sin_port = htons(++(*port));
    if (listen(tcp, 1) < 0)
        die("err listen!\n");
    return tcp;
}

int connect_server(int port)
{
    struct sockaddr_in server_address;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        die("err socket!\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        writer(set_color(BLUE, "can't coonect!\n"));
        close(fd);
        return -1;
    }

    return fd;
}

char *send_username(int broadcaster, char *name)
{
    while (name[0] == 0)
    {
        writer(set_color(BLUE, "invalid username!\n"));
        return NULL;
    }
    send_message(USERNAME, name, broadcaster);
    return name;
}

_called call_back;

void _caller(int sig)
{
    // writer("before call\n");
    call_back();
    // writer("after call\n");
}

void set_alarm(int sec, _called call)
{
    alarm(0);
    call_back = call;
    signal(SIGALRM, _caller);
    alarm(sec);
}