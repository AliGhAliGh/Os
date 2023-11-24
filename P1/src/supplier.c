#include "include/network.h"
#include "include/str.h"
#include "include/data.h"

#define ANSWER "answer"
#define TIME_OUT 90

fd_set master;
char *username = NULL, *temp_name = NULL;

char *current_command = NULL;
int broadcasater, tcp;
const char Type = 's';
int client = -1;

struct timeval *res_timer, timeout;
char timeout_cause;

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

void set_curr_command(char *command)
{
    current_command = command;
    set_ready(0);
    if (command == NULL)
        return;
    set_ready(2);
    if (strcmp(ANSWER, command) == 0)
        writer(str_make(5, "your answer (", set_color(GREEN, "yes"), "/", set_color(YELLOW, "no"), "): "));
}

void closer(char *message, char *res)
{
    writer(message);
    send(client, res, strlen(res), 0);
    close(client);
    FD_CLR(client, &master);
    client = -1;
}

void time_out()
{
    set_curr_command(NULL);
    closer(set_color(BLUE, "\nrequest time out!\n"), "close");
    set_ready(0);
}

void run_custom_command(char *command)
{
    if (strcmp(current_command, ANSWER) == 0)
    {
        if (strcmp("yes", command) == 0 || strcmp("no", command) == 0)
        {
            char *res = str_make(3, command, "|", username);
            closer(set_color(GREEN, "done\n"), res);
            cancel_alarm();
        }
        else
            writer(set_color(BLUE, "not valid answer!\n"));
        set_curr_command(NULL);
    }
}

void run_command(char *command)
{
    if (current_command != NULL)
    {
        run_custom_command(command);
        return;
    }
    if (strcmp(command, "answer request") == 0)
    {
        if (client == -1)
            writer(set_color(BLUE, "no request available!\n"));
        else
            set_curr_command(ANSWER);
    }
    else
        writer(set_color(YELLOW, "not valid command!\n"));
}

void accept_username()
{
    username = temp_name;
    open_log(username);
    temp_name = NULL;
    set_ready(0);
    writer(str_make(5, "Welcome ", set_color(CYAN, username), " as ", set_color(PINK, "Supplier"), "!!\n"));
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
        die("please enter a port!\n");

    int port = atoi(argv[1]);
    broadcasater = get_broadcaster(port);
    tcp = get_tcp(&port);

    FD_ZERO(&master);
    FD_SET(STDIN_FILENO, &master);
    FD_SET(broadcasater, &master);
    FD_SET(tcp, &master);
    int max_fd = tcp;
    writer(str_make(3, "Please enter your ", set_color(CYAN, "username"), ":\n"));

    fd_set work;
    timeout.tv_usec = 0;
    timeout.tv_sec = 0;
    res_timer = NULL;
    timeout_cause = 0;

    while (1)
    {
        work = master;
        if (current_command == NULL)
            set_ready(1);
        int n = select(max_fd + 1, &work, NULL, NULL, res_timer);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            writer(to_str(errno));
            break;
        }
        if (n == 0)
        {
            res_timer = NULL;
            if (timeout_cause == USERNAME[0])
                accept_username();
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
                    if (temp_name == NULL && (temp_name = send_username(broadcasater, temp)) != NULL)
                        set_timeout(USERNAME);
                }
                else
                    run_command(temp);
            }
            else if (i == broadcasater)
            {
                set_ready(0);
                char res;
                char *message = get_type(broadcasater, &res);
                if (res == USERNAME[0])
                    handle_username(username, message, broadcasater);
                else if (res == BAD_USER[0] && username == NULL && temp_name != NULL && strcmp(temp_name, message) == 0)
                {
                    writer(set_color(BLUE, "invalid username!\n"));
                    set_timeout(NULL);
                    temp_name = NULL;
                }
                else if (username != NULL && res == FINDOUT_S[0])
                {
                    send_message(ACK_FIND, str_make(5, message, "|", username, "|", to_str(port)), broadcasater);
                }
                else if (res == Type)
                    writer(message);
                free(message);
            }
            else if (tcp == i)
            {
                set_ready(0);
                int temp_client = accept_client(tcp);
                if (client > 0)
                {
                    close(temp_client);
                    continue;
                }
                writer(str_make(3, "new request ", set_color(PINK, "Ingredient"), "!\n"));
                client = temp_client;
                FD_SET(client, &master);
                if (client > max_fd)
                    max_fd = client;
                set_alarm(TIME_OUT, time_out);
            }
            else
            {
                writer("bug!\n");
            }
        }
    }
}