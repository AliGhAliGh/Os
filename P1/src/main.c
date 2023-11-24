#include "include/main.h"

char *temp_name = NULL;
char *current_command = NULL;
struct timeval *res_timer, timeout;
char timeout_cause;

void set_curr_command(char *command)
{
    current_command = command;
}

void set_showing(int show)
{
    if (oldt == NULL)
    {
        oldt = malloc(sizeof(struct termios));
        tcgetattr(STDIN_FILENO, oldt);
    }
    struct termios new = *oldt;
    if (!show)
        new.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void set_timeout(char *cause)
{
    if (cause == NULL)
    {
        res_timer = NULL;
        return;
    }
    timeout.tv_usec = 100000;
    res_timer = &timeout;
    timeout_cause = cause[0];
}

void run(_run_command run_command, _run_custom_command run_custom_command, int broadcaster, int tcp, const char type)
{
    int max_fd = broadcaster;
    fd_set master, work;
    FD_ZERO(&master);
    FD_SET(STDIN_FILENO, &master);
    FD_SET(broadcaster, &master);
    if (tcp > 0)
        FD_SET(tcp, &master);
    writer(str_make(3, "Please enter your ", set_color(CYAN, "username"), ":\n"));
    while (1)
    {
        work = master;
        if (current_command == NULL)
            set_ready(1);
        // writer("before select\n");
        int n = select(max_fd + 1, &work, NULL, NULL, res_timer);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                writer("intrupt!\n");
                continue;
            }
            break;
        }
        // writer(str_make(3, "after select", to_str(n), "\n"));
        if (n == 0)
        {
            res_timer = NULL;
            if (timeout_cause == USERNAME[0])
                accept_username();
            else if (timeout_cause == FINDOUT_S[0])
                end_findout();
            continue;
        }
        for (int i = 0; i <= max_fd; i++)
        {
            if (!FD_ISSET(i, &work))
                continue;
            if (i == STDIN_FILENO)
            {
                char *temp = reader();
                if (username == NULL)
                {
                    if (temp_name == NULL && (temp_name = send_username(broadcaster, temp)) != NULL)
                        set_timeout(USERNAME);
                }
                else if (current_command == NULL)
                    run_command(temp);
                else
                    run_custom_command(temp, current_command);
            }
            else if (i == broadcaster)
            {
                set_ready(0);
                char res;
                char *message = get_type(broadcaster, &res);
                if (res == USERNAME[0])
                    handle_username(username, message, broadcaster);
                else if (res == BAD_USER[0] && username == NULL && temp_name != NULL && strcmp(temp_name, message) == 0)
                {
                    writer(set_color(BLUE, "invalid username!\n"));
                    set_timeout(NULL);
                    temp_name = NULL;
                }
                else if (res == ACK_FIND[0])
                    handle_ack_find(message);
                else if (res == type)
                    writer(message);
                free(message);
            }
            else if (tcp == i)
            {
                set_ready(0);
                FD_SET(accept_client(tcp), &master);

                writer(str_make(3, "new ", set_color(PINK, "Order"), "!\n"));
                Request req;
                // req.socket = accept_client(tcp);
                if (req.socket > max_fd)
                    max_fd = req.socket;
                char *buffer = malloc(INPUT_BUF_SIZE);
                recv(req.socket, buffer, INPUT_BUF_SIZE, 0);
                req.username = split(&buffer, "|");
                req.port = atoi(split(&buffer, "|"));
                req.food_name = split(&buffer, "|");
                req.is_end = 0;
                add_request(req);
            }
            else
            {
                char *buffer = malloc(INPUT_BUF_SIZE);
                recv(i, buffer, INPUT_BUF_SIZE, 0);
                rem_request(buffer);
                close(i);
            }
        }
    }
}