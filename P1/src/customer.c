#include "include/network.h"
#include "include/json.h"
#include "include/str.h"
#include "include/data.h"

#define FOOD_NAME "food name"
#define PORT_REST "restaurant port"
#define TIME_OUT 120

char *username = NULL, *temp_name = NULL;

Recipes recipes;

int is_searching = 0;
char *current_command = NULL;
char *food_name;
int broadcasater;
const char Type = 'c';

fd_set master;
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
    if (strcmp(FOOD_NAME, command) == 0)
        writer(str_make(4, set_color(YELLOW, "name "), "of ", set_color(PINK, "Food"), ": "));
    else if (strcmp(PORT_REST, command) == 0)
        writer(str_make(4, set_color(YELLOW, "port "), "of ", set_color(PINK, "Restaurant"), ": "));
}

int has_food_name(char *name)
{
    for (int i = 0; i < recipes.count; i++)
    {
        if (strcmp(recipes.recipes[i].name, name) == 0)
            return 1;
    }
    return 0;
}

void run_custom_command(char *command)
{
    if (strcmp(current_command, FOOD_NAME) == 0)
    {
        if (has_food_name(command))
        {
            food_name = command;
            set_curr_command(PORT_REST);
        }
        else
        {
            writer(set_color(BLUE, "not in menu!\n"));
            set_curr_command(NULL);
        }
    }
    else if (strcmp(current_command, PORT_REST) == 0)
    {
        set_curr_command(NULL);
        writer(str_make(4, set_color(YELLOW, "waiting "), "for ", set_color(PINK, "Restaurant"), "'s response ...\n"));
        int tcp = connect_server(atoi(command));
        if (tcp < 0)
            return;
        struct sockaddr_in my_addr;
        memset(&my_addr, 0, sizeof(my_addr));
        socklen_t len = sizeof(my_addr);
        getsockname(tcp, (struct sockaddr *)&my_addr, &len);
        char *message = str_make(5, username, "|", to_str((int)ntohs(my_addr.sin_port)), "|", food_name);
        send(tcp, message, strlen(message), 0);
        char *buffer = malloc(INPUT_BUF_SIZE);
        fd_set temp_set;
        FD_ZERO(&temp_set);
        FD_SET(tcp, &temp_set);
        struct timeval timeout;
        timeout.tv_sec = TIME_OUT;
        timeout.tv_usec = 0;
        set_showing(0, &master);
        int n = select(tcp + 1, &temp_set, NULL, NULL, &timeout);
        set_showing(1, &master);
        if (n < 0)
            die("select err!\n");
        if (n == 0)
        {
            writer(set_color(BLUE, "Time out!\n"));
            close(tcp);
            return;
        }
        n = recv(tcp, buffer, INPUT_BUF_SIZE, 0);
        if (n < 0)
        {
            writer(set_color(BLUE, "restaurant is closed!\n"));
            return;
        }
        buffer[n] = 0;
        char *res = split(&buffer, "|");
        if (strcmp("yes", res) == 0)
            writer(str_make(3, set_color(CYAN, buffer), set_color(PINK, " Restaurant"), " accepted and your food is ready!\n"));
        else if (strcmp("no", res) == 0)
            writer(str_make(3, set_color(CYAN, buffer), set_color(PINK, " Restaurant"), " denied and cry about it!\n"));
        else
            writer(set_color(BLUE, "restaurant is closed!\n"));
        close(tcp);
    }
}

void run_command(char *command)
{
    if (current_command != NULL)
    {
        run_custom_command(command);
        return;
    }
    if (strcmp(command, "order food") == 0)
    {
        set_curr_command(FOOD_NAME);
    }
    else if (strcmp(command, "show menu") == 0)
    {
        writer("\n---------------------------------------\n");
        for (int i = 0; i < recipes.count; i++)
            writer(str_make(4, to_str(i + 1), "- ", set_color(PINK, recipes.recipes[i].name), "\n"));
        writer("---------------------------------------\n\n");
    }
    else if (strcmp("show restaurants", command) == 0)
    {
        writer("\n---------------------------------------\n");
        is_searching = 1;
        send_message(FINDOUT_R, username, broadcasater);
        set_timeout(FINDOUT_R);
    }
    else
        writer(set_color(YELLOW, "not valid command!\n"));
}

void read_recipes()
{
    int fd = open("recipes.json", O_RDONLY);
    if (fd < 0)
        die("recipes open fail!\n");
    char buffer[10000];
    int n = read(fd, buffer, sizeof(char) * 10000);
    buffer[n] = 0;
    json_value *jv = json_parse(buffer, strlen(buffer));
    recipes.count = jv->u.object.length;
    recipes.recipes = malloc(sizeof(Recipe) * recipes.count);
    for (int i = 0; i < recipes.count; i++)
    {
        recipes.recipes[i].name = jv->u.object.values[i].name;
        json_value *jv2 = jv->u.object.values[i].value;
        recipes.recipes[i].ingredients.count = jv2->u.object.length;
        recipes.recipes[i].ingredients.ingredients = malloc(sizeof(Ingredient) * recipes.recipes[i].ingredients.count);
        for (int j = 0; j < recipes.recipes[i].ingredients.count; j++)
        {
            recipes.recipes[i].ingredients.ingredients[j].name = jv2->u.object.values[j].name;
            recipes.recipes[i].ingredients.ingredients[j].count = jv2->u.object.values[j].value->u.integer;
        }
    }
}

void accept_username()
{
    username = temp_name;
    open_log(username);
    temp_name = NULL;
    set_ready(0);
    writer(str_make(5, "Welcome ", set_color(CYAN, username), " as ", set_color(PINK, "Customer"), "!!\n"));
}

void handle_ack_find(char *message)
{
    if (username == NULL || !is_searching)
        return;
    if (strcmp(split(&message, "|"), username) != 0)
        return;
    writer(set_color(CYAN, split(&message, "|")));
    writer(str_make(3, "\t", set_color(GREEN, message), "\n"));
}

void end_findout()
{
    is_searching = 0;
    set_ready(0);
    writer("---------------------------------------\n\n");
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
        die("please enter a port!\n");

    broadcasater = get_broadcaster(atoi(argv[1]));

    read_recipes();

    int max_fd = broadcasater;
    fd_set work;
    FD_ZERO(&master);
    FD_SET(STDIN_FILENO, &master);
    FD_SET(broadcasater, &master);

    writer(str_make(3, "Please enter your ", set_color(CYAN, "username"), ":\n"));

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
            break;
        if (n == 0)
        {
            res_timer = NULL;
            if (timeout_cause == USERNAME[0])
                accept_username();
            else if (timeout_cause == FINDOUT_R[0])
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
                else if (res == ACK_FIND_R[0])
                    handle_ack_find(message);
                else if (res == Type)
                    writer(message);
                free(message);
            }
        }
    }
}