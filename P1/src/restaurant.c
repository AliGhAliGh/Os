#include "include/network.h"
#include "include/json.h"
#include "include/data.h"

#define SUPP_PORT "supplier port"
#define ING_NAME "ingredient name"
#define ING_COUNT "ingredient count"
#define ANS_REQ "answer request"
#define YES_NO "yes no"
#define ACCEPTED 0
#define DENIED 1
#define TIME_OUT 2

char *username = NULL;
char *temp_name = NULL;
int broadcasater, tcp;

const char Type = 'r';
int is_open, is_searching;
char *current_command = NULL;

fd_set master;
struct timeval *res_timer, timeout;
char timeout_cause;

typedef struct supp_con
{
    int port;
    Ingredient ingredient;
} Sup_Con;

typedef struct request
{
    char *username;
    int port, socket;
    char *food_name;
    struct request *next;
} Request;

typedef struct history
{
    char *name;
    char *food_name;
    int state;
    struct history *next;
} History;

History *histories = NULL;

Request *req_list = NULL;

Request curr_req;

Sup_Con curr_con;

Recipes recipes;

Ingredients ingredients;

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
    if (strcmp(SUPP_PORT, command) == 0)
        writer(str_make(4, set_color(YELLOW, "port "), "of ", set_color(PINK, "Supplier"), ": "));
    else if (strcmp(ING_NAME, command) == 0)
        writer(str_make(4, set_color(YELLOW, "name "), "of ", set_color(PINK, "Ingredient"), ": "));
    else if (strcmp(ING_COUNT, command) == 0)
        writer(str_make(4, set_color(YELLOW, "number "), "of ", set_color(PINK, "Ingredient"), ": "));
    else if (strcmp(ANS_REQ, command) == 0)
        writer(str_make(4, set_color(YELLOW, "port "), "of ", set_color(PINK, "Request"), ": "));
    else if (strcmp(YES_NO, command) == 0)
        writer(str_make(5, "your answer (", set_color(GREEN, "yes"), "/", set_color(YELLOW, "no"), "): "));
}

void add_ingredient(Ingredient ingredient)
{
    for (int i = 0; i < ingredients.count; i++)
    {
        if (strcmp(ingredients.ingredients[i].name, ingredient.name) == 0)
        {
            ingredients.ingredients[i].count += ingredient.count;
            return;
        }
    }
    ingredients.ingredients = realloc(ingredients.ingredients, sizeof(Ingredient) * (++ingredients.count));
    ingredients.ingredients[ingredients.count - 1].name = ingredient.name;
    ingredients.ingredients[ingredients.count - 1].count = ingredient.count;
}

Request *req_by_port(int port)
{
    Request *res = req_list;
    while (res != NULL)
    {
        if (res->port == port)
            break;
        res = res->next;
    }
    return res;
}

int has_ings(Ingredients ings)
{
    for (int j = 0; j < ings.count; j++)
    {
        int has = 0;
        for (int k = 0; k < ingredients.count; k++)
        {
            if (strcmp(ings.ingredients[j].name, ingredients.ingredients[k].name) == 0 && ingredients.ingredients[k].count >= ings.ingredients[j].count)
            {
                has = 1;
                break;
            }
        }
        if (!has)
            return 0;
    }

    return 1;
}

int rem_ings(Ingredients ings)
{
    for (int j = 0; j < ings.count; j++)
        for (int k = 0; k < ingredients.count; k++)
            if (strcmp(ings.ingredients[j].name, ingredients.ingredients[k].name) == 0)
            {
                ingredients.ingredients[k].count -= ings.ingredients[j].count;
                break;
            }

    return 1;
}

int rem_food_ing(char *food_name)
{
    for (int i = 0; i < recipes.count; i++)
        if (strcmp(recipes.recipes[i].name, food_name) == 0)
        {
            Ingredients ings = recipes.recipes[i].ingredients;
            if (has_ings(ings))
            {
                rem_ings(ings);
                return 1;
            }
            else
                return 0;
        }
    return 0;
}

void add_request(Request *req)
{
    req->next = req_list;
    req_list = req;
}

void add_history(Request *history, int state)
{
    History *his = malloc(sizeof(History));
    his->food_name = history->food_name;
    his->name = history->username;
    his->state = state;
    his->next = histories;
    histories = his;
}

void rem_request(int socket, int state)
{
    if (req_list != NULL)
    {
        Request *current = req_list;
        if (current->socket == socket)
        {
            req_list = current->next;
            add_history(current, state);
            return;
        }
        while (current->next != NULL)
        {
            if (current->next->socket == socket)
            {
                add_history(current->next, state);
                current->next = current->next->next;
                return;
            }
            current = current->next;
        }
    }
}

void run_custom_command(char *command)
{
    if (strcmp(current_command, SUPP_PORT) == 0)
    {
        curr_con.port = atoi(command);
        set_curr_command(ING_NAME);
    }
    else if (strcmp(current_command, ANS_REQ) == 0)
    {
        Request *temp = req_by_port(atoi(command));
        if (temp == NULL)
        {
            writer(set_color(BLUE, "bad port!\n"));
            set_curr_command(NULL);
        }
        else
        {
            curr_req = *temp;
            set_curr_command(YES_NO);
        }
    }
    else if (strcmp(current_command, YES_NO) == 0)
    {
        if (strcmp("yes", command) == 0)
        {
            if (rem_food_ing(curr_req.food_name) == 0)
            {
                command = "no";
                writer(set_color(BLUE, "not enough ingredients!\n"));
                rem_request(curr_req.socket, DENIED);
            }
            else
                rem_request(curr_req.socket, ACCEPTED);
            char *res = str_make(3, command, "|", username);
            send(curr_req.socket, res, strlen(res), 0);
            writer(set_color(GREEN, "done!\n"));
        }
        else if (strcmp("no", command) == 0)
        {
            char *res = str_make(3, command, "|", username);
            send(curr_req.socket, res, strlen(res), 0);
            rem_request(curr_req.socket, DENIED);
            writer(set_color(GREEN, "done!\n"));
        }
        else
            writer(set_color(BLUE, "not valid answer!\n"));
        set_curr_command(NULL);
    }
    else if (strcmp(current_command, ING_NAME) == 0)
    {
        curr_con.ingredient.name = command;
        set_curr_command(ING_COUNT);
    }
    else if (strcmp(current_command, ING_COUNT) == 0)
    {
        curr_con.ingredient.count = atoi(command);
        set_curr_command(NULL);
        writer(str_make(4, set_color(YELLOW, "waiting "), "for ", set_color(PINK, "Supplier"), "'s response ...\n"));
        int tcp = connect_server(curr_con.port);
        if (tcp < 0)
            return;
        char *buffer = malloc(INPUT_BUF_SIZE);
        set_showing(0, &master);
        int n = recv(tcp, buffer, INPUT_BUF_SIZE, 0);
        set_showing(1, &master);
        if (n < 0)
            die("err read tcp!\n");
        buffer[n] = 0;
        char *res = split(&buffer, "|");
        if (strcmp("yes", res) == 0)
        {
            add_ingredient(curr_con.ingredient);
            writer(str_make(3, set_color(CYAN, buffer), set_color(PINK, " Supplier"), " accepted!\n"));
        }
        else if (strcmp("no", res) == 0)
            writer(str_make(3, set_color(CYAN, buffer), set_color(PINK, " Supplier"), " denied!\n"));
        else if (strcmp("", res) == 0)
            writer(set_color(BLUE, "the supplier is busy!\n"));
        else
            writer(set_color(BLUE, "Time out!\n"));
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
    if (strcmp("start working", command) == 0)
        if (is_open)
            writer(set_color(BLUE, "already is open!\n"));
        else
        {
            writer(set_color(GREEN, "done!\n"));
            send_message(CUSTOMER, str_make(3, set_color(CYAN, username), set_color(PINK, " Restaurant"), " opened!\n"), broadcasater);
            is_open = 1;
        }
    else if (!is_open)
        writer(set_color(BLUE, "not opened yet!\n"));
    else if (strcmp("break", command) == 0)
    {
        if (req_list != NULL)
        {
            writer(set_color(BLUE, "you have some requests!\n"));
            return;
        }
        writer(set_color(GREEN, "done!\n"));
        send_message(CUSTOMER, str_make(3, set_color(CYAN, username), set_color(PINK, " Restaurant"), " closed!\n"), broadcasater);
        is_open = 0;
    }
    else if (strcmp("show recipes", command) == 0)
    {
        writer("\n---------------------------------------\n");
        for (int i = 0; i < recipes.count; i++)
        {
            writer(str_make(4, to_str(i + 1), "- ", set_color(PINK, recipes.recipes[i].name), ":\n"));
            for (int j = 0; j < recipes.recipes[i].ingredients.count; j++)
                writer(str_make(5, "\t", set_color(YELLOW, recipes.recipes[i].ingredients.ingredients[j].name),
                                " : ", set_color(GREEN, to_str(recipes.recipes[i].ingredients.ingredients[j].count)), "\n"));
        }
        writer("---------------------------------------\n\n");
    }
    else if (strcmp("show requests list", command) == 0)
    {
        writer("\n---------------------------------------\n");
        Request *temp = req_list;
        while (temp != NULL)
        {
            writer(str_make(6, set_color(CYAN, temp->username), " ",
                            set_color(YELLOW, to_str(temp->port)), " ",
                            set_color(PINK, temp->food_name), "\n"));
            temp = temp->next;
        }

        writer("---------------------------------------\n\n");
    }
    else if (strcmp("show suppliers", command) == 0)
    {
        writer("\n---------------------------------------\n");
        is_searching = 1;
        send_message(FINDOUT_S, username, broadcasater);
        set_timeout(FINDOUT_S);
    }
    else if (strcmp("request ingredient", command) == 0)
        set_curr_command(SUPP_PORT);
    else if (strcmp("answer request", command) == 0)
        set_curr_command(ANS_REQ);
    else if (strcmp("show ingredients", command) == 0)
    {
        writer("\n---------------------------------------\n");
        for (int i = 0; i < ingredients.count; i++)
            writer(str_make(4, set_color(YELLOW, ingredients.ingredients[i].name), " :\t",
                            set_color(GREEN, to_str(ingredients.ingredients[i].count)), "\n"));
        writer("---------------------------------------\n\n");
    }
    else if (strcmp("show sales history", command) == 0)
    {
        writer("\n---------------------------------------\n");
        History *temp = histories;
        while (temp != NULL)
        {
            char *state = temp->state == ACCEPTED ? set_color(GREEN, " accepted") : temp->state == DENIED ? set_color(YELLOW, " denied")
                                                                                                          : set_color(YELLOW, " time out");
            writer(str_make(5, set_color(CYAN, temp->name), " ", set_color(PINK, temp->food_name), state, "!\n"));
            temp = temp->next;
        }
        writer("---------------------------------------\n\n");
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
    writer(str_make(5, "Welcome ", set_color(CYAN, username), " as ", set_color(PINK, "Restaurant"), "!!\n"));
    send_message(CUSTOMER, str_make(3, set_color(CYAN, username), set_color(PINK, " Restaurant"), " opened!\n"), broadcasater);
    is_open = 1;
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

    int port = atoi(argv[1]);
    broadcasater = get_broadcaster(port);
    tcp = get_tcp(&port);

    is_open = 0;
    read_recipes();

    int max_fd = tcp;
    fd_set work;
    FD_ZERO(&master);
    FD_SET(STDIN_FILENO, &master);
    FD_SET(broadcasater, &master);
    FD_SET(tcp, &master);
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
                else if (res == ACK_FIND[0])
                    handle_ack_find(message);
                else if (username != NULL && res == FINDOUT_R[0])
                {
                    send_message(ACK_FIND_R, str_make(5, message, "|", username, "|", to_str(port)), broadcasater);
                }
                else if (res == Type)
                    writer(message);
                free(message);
            }
            else if (tcp == i)
            {
                set_ready(0);
                int temp = accept_client(tcp);
                if (username == NULL || !is_open)
                {
                    close(temp);
                    continue;
                }

                writer(str_make(3, "new ", set_color(PINK, "Order"), "!\n"));
                Request req;
                req.socket = temp;
                FD_SET(req.socket, &master);
                if (req.socket > max_fd)
                    max_fd = req.socket;
                char *buffer = malloc(INPUT_BUF_SIZE);
                int n = recv(req.socket, buffer, INPUT_BUF_SIZE, 0);
                buffer[n] = 0;
                req.username = split(&buffer, "|");
                req.port = atoi(split(&buffer, "|"));
                req.food_name = split(&buffer, "|");
                add_request(&req);
            }
            else
            {
                rem_request(i, TIME_OUT);
                FD_CLR(i, &master);
                close(i);
                set_ready(0);
            }
        }
    }
}
