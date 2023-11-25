#include "include/defs.hpp"
#include "include/building.hpp"

using namespace std;

void Building::set_data(Resource resource, string data)
{
    auto big_data = split(data, '|');
    Building::data.push_back({resource, convert(split(big_data[0], ',')), convert(split(big_data[1], ',')), convert(split(big_data[2], ',')), convert(split(big_data[3], ','))});
}

string Building::get_data()
{
    vector<string> res;
    for (const auto &d : Building::data)
        res.push_back(res_to_str(d.resource) + "|" + concat(d.peaks) + "|" + concat(d.peak_hours) + "|" + concat(d.leasts) + "|" + concat(d.sums));
    return concat(res, '%');
}

int main(int argc, char *argv[])
{
    string path(argv[1]);
    string name(split(path, '/').back());
    string need_resources;
    getline(cin, need_resources);
    auto reqs = get_resources(need_resources);
    Building my_building;
    vector<pair<Resource, int>> res_fd;
    for (const auto &req : reqs)
    {
        log::info("building " + name + " creates meter for " + res_to_str(req));
        res_fd.push_back({req, create_process(METER, path + "/" + res_to_str(req))});
    }
    for (const auto &p : res_fd)
    {
        my_building.set_data(p.first, read(p.second));
        log::info("meter " + res_to_str(p.first) + " of " + name + " is done!");
    }
    send_to_pipe(name, my_building.get_data());
}