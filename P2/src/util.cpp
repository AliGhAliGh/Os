#include "include/defs.hpp"

using namespace std;

vector<string> split(const string &str, char delim)
{
    vector<string> elems;
    istringstream ss(str);
    string item;
    while (getline(ss, item, delim))
    {
        item.erase(0, item.find_first_not_of(' '));
        item.erase(item.find_last_not_of(' ') + 1);
        if (!item.empty())
            elems.push_back(item);
    }
    return elems;
}

vector<string> get_dirs(const string &path)
{
    vector<string> res;

    DIR *directory = opendir(path.c_str());
    dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (entry->d_name[0] != '.')
            {
                res.push_back(entry->d_name);
            }
        }
    }
    closedir(directory);
    return res;
}

vector<int> convert(vector<string> data)
{
    vector<int> row;
    transform(data.begin(), data.end(), back_inserter(row), [](const string &str)
              { return stoi(str); });
    return row;
}

vector<float> convert_f(vector<string> data)
{
    vector<float> row;
    transform(data.begin(), data.end(), back_inserter(row), [](const string &str)
              { return stof(str); });
    return row;
}

vector<vector<int>> get_csv(const string &path)
{
    vector<vector<int>> res;
    ifstream file;
    file.open(path + ".csv");
    string line;
    getline(file, line);
    while (getline(file, line))
        res.push_back(convert(split(line, ',')));
    file.close();
    return res;
}

string create_process(const char *proc_name, string param)
{
    int p1[2];
    pipe(p1);
    if (fork() == 0)
    {
        close(1);
        dup(p1[1]);
        close(p1[0]);
        close(p1[1]);
        execl(proc_name, proc_name, param.c_str(), NULL);
    }
    else
    {
        close(p1[1]);
        char buffer[buffer_size];
        read(p1[0], buffer, buffer_size * sizeof(char));
        close(p1[0]);
        return buffer;
    }
    throw;
}

string create_process(const char *proc_name, string send_data, string param)
{
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    log::warn("before fork");
    if (fork() == 0)
    {
        log::warn("child fork");
        close(0);
        dup(p1[0]);
        close(p1[0]);
        close(p1[1]);
        close(1);
        dup(p2[1]);
        close(p2[0]);
        close(p2[1]);
        // log::warn("before exec");
        execl(proc_name, proc_name, param.c_str(), NULL);
    }
    else
    {
        log::warn("parent fork");
        close(p1[0]);
        close(p2[1]);
        write(p1[1], send_data.c_str(), send_data.size());
        log::warn("after write parent");
        char buffer[buffer_size];
        read(p2[0], buffer, buffer_size * sizeof(char));
        log::warn("after read parent");
        close(p1[1]);
        close(p2[0]);
        return buffer;
    }
    throw;
}

string get_fifo_name(const string &name)
{
    return ".fifo/" + name;
}

void send_to_pipe(const string &name, const string &data)
{
    string fifoName = get_fifo_name(name);
    int fd = open(fifoName.c_str(), O_WRONLY);
    if (fd == -1)
    {
        log::perror("open");
        exit(1);
    }
    if (write(fd, data.c_str(), data.size()) == -1)
    {
        log::perror("write");
        exit(1);
    }
    close(fd);
}

string recv_from_pipe(const string &name)
{
    string fifoName = get_fifo_name(name);
    int fd = open(fifoName.c_str(), O_RDONLY);
    if (fd == -1)
    {
        log::perror("open");
        exit(1);
    }
    char buffer[buffer_size];
    if (read(fd, buffer, buffer_size * sizeof(char)) == -1)
    {
        log::perror("read");
        exit(1);
    }
    close(fd);
    return buffer;
}

void delete_pipe(const std::string &name)
{
    if (unlink(get_fifo_name(name).c_str()) == -1)
    {
        log::perror("unlink");
        exit(1);
    }
}

void create_pipe(const std::string &name)
{
    if (mkfifo(get_fifo_name(name).c_str(), 0666) == -1)
    {
        log::perror("mkfifo");
        exit(1);
    }
}

vector<Resource> get_resources(string input)
{
    vector<Resource> res;
    auto req = split(input);
    for (auto name : req)
        if (name == "e")
            res.push_back(Resource::Electricity);
        else if (name == "w")
            res.push_back(Resource::Water);
        else if (name == "g")
            res.push_back(Resource::Gas);
    return res;
}