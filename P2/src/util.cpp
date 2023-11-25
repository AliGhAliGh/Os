#include "include/defs.hpp"

using namespace std;

vector<int> slicing(vector<int> arr, int x)
{
    int y = arr.size() - 1;
    vector<int> result(y - x + 1);
    copy(arr.begin() + x, arr.begin() + y + 1, result.begin());
    return result;
}

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
        if (entry->d_type == DT_DIR && entry->d_name[0] != '.')
            res.push_back(entry->d_name);
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

vector<vector<int>> get_csv(const string &path, int ignore_count)
{
    vector<vector<int>> res;
    ifstream file;
    file.open(path + ".csv");
    string line;
    getline(file, line);
    while (getline(file, line))
        res.push_back(slicing(convert(split(line, ',')), ignore_count));
    file.close();
    return res;
}

int create_process(const char *proc_name, string param)
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
        return p1[0];
    }
    throw;
}

int create_process(const char *proc_name, string send_data, string param)
{
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    if (fork() == 0)
    {
        close(0);
        dup(p1[0]);
        close(p1[0]);
        close(p1[1]);
        close(1);
        dup(p2[1]);
        close(p2[0]);
        close(p2[1]);
        execl(proc_name, proc_name, param.c_str(), NULL);
    }
    else
    {
        close(p1[0]);
        close(p2[1]);
        write(p1[1], send_data.c_str(), send_data.size());
        close(p1[1]);
        return p2[0];
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

string read(int fd)
{
    char buffer[buffer_size];
    int r;
    string data = "";
    while (true)
    {
        r = read(fd, buffer, (buffer_size - 1) * sizeof(char));
        buffer[r] = 0;
        data += buffer;
        if (r < buffer_size - 1)
            break;
    }
    close(fd);
    if (r < 0)
    {
        log::perror("read");
        exit(1);
    }
    // log::dbug("recv data via pipe:\n" + data);
    return data;
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
    return read(fd);
}

void delete_pipe(const string &name)
{
    string fifoName = get_fifo_name(name);
    if (unlink(get_fifo_name(name).c_str()) == -1)
    {
        log::perror("unlink");
        exit(1);
    }
}

void create_pipe(const string &name)
{
    string fifoName = get_fifo_name(name);
    if (mkfifo(get_fifo_name(name).c_str(), 0777) == -1)
    {
        log::perror("mkfifo");
        exit(1);
    }
}

vector<Resource> get_resources(string input)
{
    vector<Resource> res;
    auto req = split(input, ' ');
    for (const auto &name : req)
        if (name == "e")
            res.push_back(Resource::Electricity);
        else if (name == "w")
            res.push_back(Resource::Water);
        else if (name == "g")
            res.push_back(Resource::Gas);
    return res;
}

string concat(const vector<int> &data, const char delim)
{
    stringstream ss;
    for (size_t i = 0; i < data.size(); ++i)
    {
        ss << to_string(data[i]);
        if (i != data.size() - 1)
            ss << delim;
    }
    return ss.str();
}

string concat(const vector<long long> &data, const char delim)
{
    stringstream ss;
    for (size_t i = 0; i < data.size(); ++i)
    {
        ss << to_string(data[i]);
        if (i != data.size() - 1)
            ss << delim;
    }
    return ss.str();
}

string concat(const vector<float> &data, const char delim)
{
    stringstream ss;
    for (size_t i = 0; i < data.size(); ++i)
    {
        ss << fixed << setprecision(2) << data[i];
        if (i != data.size() - 1)
            ss << delim;
    }
    return ss.str();
}

string concat(const vector<string> &data, const char delim)
{
    stringstream ss;
    for (size_t i = 0; i < data.size(); ++i)
    {
        ss << data[i];
        if (i != data.size() - 1)
            ss << delim;
    }
    return ss.str();
}
