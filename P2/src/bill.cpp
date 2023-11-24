#include "include/defs.hpp"

using namespace std;

string get_resources()
{
    cout << "Which resources? (g/w/e)" << endl;
    string input;
    getline(cin, input);
    auto req = split(input);
    for (auto name : req)
        if (name != "e" && name != "w" && name != "g")
        {
            log::warn("pls enter correct resources!");
            return get_resources();
        }
    return input;
}

vector<string> get_buildings(vector<string> &names)
{
    cout << "Which building?" << endl;
    vector<string> res;
    string input;
    getline(cin, input);
    auto req = split(input);
    for (auto name : req)
        if (find(names.begin(), names.end(), name) != names.end())
            res.push_back(name);
        else
        {
            log::warn("pls enter correct name!");
            return get_buildings(names);
        }
    return res;
}

vector<Report> str_to_rep(string data)
{
    vector<Report> res;
    auto reports = split(data, '%');
    for (const auto &report : reports)
    {
        auto vars = split(report, '|');
        res.push_back({convert_f(split(vars[0])), convert(split(vars[1])), convert(split(vars[2])), convert_f(split(vars[3])), convert_f(split(vars[4]))});
    }
    return res;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        log::error("usage: bill FilePath");
        exit(1);
    }

    string path(argv[1]);
    auto building_names = get_dirs(path);

    log::info("%d buildings found:", building_names.size());
    for (const auto &name : building_names)
        log::info(name);

    auto req = get_resources();
    auto names = get_buildings(building_names);
    int building_count = names.size();

    for (int i = 0; i < building_count; i++)
    {
        log::warn("before");
        create_process("/home/aligh/Desktop/Os/P2/building", req, path + "/" + names[i]);
        log::warn("after");
    }
    auto result = str_to_rep(create_process("/home/aligh/Desktop/Os/P2/bill_calc", concat(names) + "|" + req, path));
    cout << result.size() << endl;
}