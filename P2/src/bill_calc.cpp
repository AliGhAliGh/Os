#include "include/defs.hpp"

using namespace std;

int get_resource_index(Resource res)
{
    switch (res)
    {
    case Water:
        return 0;
    case Gas:
        return 1;
    case Electricity:
        return 2;
    default:
        throw;
    }
}

float calculate_bill(const Meter &meter, int index, int coe)
{
    float res;
    switch (meter.resource)
    {
    case Electricity:
        res = meter.sums[index] + meter.peaks[index] * (float).25 - meter.leasts[index] * (float).25;
        break;
    case Gas:
        res = meter.sums[index];
        break;
    case Water:
        res = meter.sums[index] + meter.peaks[index] * (float).25;
        break;
    default:
        throw;
    }
    return res * coe;
}

vector<Meter> get_meters(string data, vector<Resource> limits)
{
    vector<Meter> res;
    auto resources = split(data, '%');
    for (const auto &resource : resources)
    {
        auto vars = split(resource, '|');
        if (find(limits.begin(), limits.end(), str_to_res(vars[0])) == limits.end())
            continue;
        Meter meter = {str_to_res(vars[0]), convert(split(vars[1], ',')), convert(split(vars[2], ',')), convert(split(vars[3], ',')), convert(split(vars[4], ','))};
        res.push_back(meter);
    }
    return res;
}

vector<Report> get_bill(vector<Meter> data, vector<vector<int>> coe, string building_name)
{
    vector<Report> res;
    for (const auto &meter : data)
    {
        Report rep;
        rep.resource = meter.resource;
        rep.sums = meter.sums;
        rep.peak_hours = meter.peak_hours;
        for (int i = 0; i < month_of_year; i++)
        {
            rep.averages.push_back(meter.sums[i] / (float)(day_of_month * hour_of_day));
            rep.diff.push_back(meter.peaks[i] / (float)day_of_month - rep.averages[i]);
            rep.bill.push_back(calculate_bill(meter, i, coe[i][get_resource_index(meter.resource)]));
        }
        res.push_back(rep);
    }
    return res;
}

string rep_to_str(vector<Report> data)
{
    vector<string> res;
    for (const auto &d : data)
        res.push_back(res_to_str(d.resource) + "|" + concat(d.averages) + "|" + concat(d.sums) + "|" + concat(d.peak_hours) + "|" + concat(d.bill) + "|" + concat(d.diff));
    return concat(res, '^');
}

int main(int argc, char *argv[])
{
    string input(argv[1]);
    auto coe = get_csv(input + "/bills", 2);
    getline(cin, input);
    auto data = split(input, '|');
    auto buildings = split(data[0], ',');
    auto limits = get_resources(data[1]);

    string res = "";
    for (const auto &building : buildings)
    {
        res += rep_to_str(get_bill(get_meters(recv_from_pipe(building), limits), coe, building));
        if (building != buildings.back())
            res += "%";
        delete_pipe(building);
        log::info("building " + building + " is done!");
    }
    cout << res;
}