#include "include/defs.hpp"

using namespace std;

string get_resources()
{
    cout << "Which resources? (g/w/e)" << endl;
    string input;
    getline(cin, input);
    auto req = split(input);
    for (const auto &name : req)
        if (name != "e" && name != "w" && name != "g")
        {
            log::error("pls enter correct resources!");
            return get_resources();
        }
    return input;
}

vector<string> get_report_type()
{
    cout << "Which reports?\n"
         << "d\t(difference)\n"
         << "a\t(average)\ns\t(sum)\np\t(peak)\nb\t(bill)" << endl;
    string input;
    getline(cin, input);
    auto req = split(input);
    for (const auto &name : req)
        if (name != "a" && name != "s" && name != "p" && name != "b" && name != "d")
        {
            log::error("pls enter correct report!");
            return get_report_type();
        }
    return req;
}

pair<int, int> get_month()
{
    cout << "Which month?(<start>-<end>)" << endl;
    string input;
    getline(cin, input);
    try
    {
        auto months = split(input, '-');

        auto start = stoi(months[0]);
        auto end = stoi(months[1]);
        if (start < 13 && start > 0 && end < 13 && end >= start)
            return {start, end};
        else
        {
            log::error("pls enter correct month!");
            return get_month();
        }
    }
    catch (const exception &e)
    {
        log::error("pls enter correct month!");
        return get_month();
    }
}

vector<string> get_buildings(vector<string> &names)
{
    cout << "Which building?" << endl;
    vector<string> res;
    string input;
    getline(cin, input);
    auto req = split(input);
    for (const auto &name : req)
        if (find(names.begin(), names.end(), name) != names.end())
            res.push_back(name);
        else
        {
            log::error("pls enter correct name!");
            return get_buildings(names);
        }
    return res;
}

vector<vector<Report>> str_to_rep(string data)
{
    vector<vector<Report>> res;
    auto all = split(data, '%');
    for (const auto &building_rep : all)
    {
        vector<Report> temp;
        auto reports = split(building_rep, '^');
        for (const auto &report : reports)
        {
            auto vars = split(report, '|');
            temp.push_back({str_to_res(vars[0]), convert_f(split(vars[1], ',')), convert(split(vars[2], ',')), convert(split(vars[3], ',')), convert_f(split(vars[4], ',')), convert_f(split(vars[5], ','))});
        }
        res.push_back(temp);
    }
    return res;
}

vector<string> extract_report(Report report, vector<string> types, int month)
{
    vector<string> res;
    for (const auto &type : types)
    {
        if (type == "a")
            res.push_back("\t\t\taverage: " + to_string(report.averages[month]));
        if (type == "s")
            res.push_back("\t\t\tsum: " + to_string(report.sums[month]));
        if (type == "p")
            res.push_back("\t\t\tpeak: " + to_string(report.peak_hours[month]));
        if (type == "b")
            res.push_back("\t\t\tbill: " + to_string(report.bill[month]));
        if (type == "d")
            res.push_back("\t\t\tdifference: " + to_string(report.diff[month]));
    }
    return res;
}

//[building count][resources][5 report type][month]
void show_result(vector<vector<Report>> data, vector<string> names, vector<string> rep_types, pair<int, int> months)
{
    int count = names.size();
    for (int i = 0; i < count; i++)
    {
        log::result("building " + names[i] + ":");
        int res_count = data[i].size();
        for (int k = 0; k < res_count; k++)
        {
            log::result("\tmeter " + res_to_str(data[i][k].resource) + ":");
            for (int j = months.first; j <= months.second; j++)
            {
                log::result("\t\tmonth " + to_string(j) + ":");
                auto res = extract_report(data[i][k], rep_types, j - 1);
                for (const auto &rep : res)
                    log::result(rep);
            }
        }
    }
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
    log::result(to_string(building_names.size()) + " buildings found:");
    for (const auto &name : building_names)
        log::result(name);

    auto requests = get_resources();
    auto names = get_buildings(building_names);
    auto report_types = get_report_type();
    auto months = get_month();
    int building_count = names.size();

    for (int i = 0; i < building_count; i++)
    {
        create_pipe(names[i]);
        create_process(BUILDING, requests, path + "/" + names[i]);
    }
    int res_fd = create_process(BILL_CALC, concat(names) + "|" + requests, path);
    log::info("all process created, waiting...");
    show_result(str_to_rep(read(res_fd)), names, report_types, months);
}