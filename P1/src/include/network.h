#ifndef NET_H
#define NET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <termios.h>
#include "str.h"

#define RESTAURANT "r"
#define CUSTOMER "c"
#define SUPPLIER "s"
#define USERNAME "u"
#define BAD_USER "b"
#define UNKNOWN "k"
#define FINDOUT_S "f"
#define FINDOUT_R "g"
#define ACK_FIND "a"
#define ACK_FIND_R "d"

typedef struct
{
    struct sockaddr_in addr;
    int sender, receiver;
} Broadcaster;

typedef void (*_called)();

void set_showing(int show, fd_set *master);

void handle_username(char *username, char *othername, int addr);

char *get_type(int receiver, char *ype);

void set_alarm(int sec, _called call_back);

void cancel_alarm();

int get_broadcaster(int port);

int get_tcp(int *port);

void send_message(char *group, char *message, int broadcasster);

char *send_username(int broadcaster, char *temp_name);

int connect_server(int port);

int accept_client(int server_fd);

#endif