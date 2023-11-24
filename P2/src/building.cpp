#include "include/defs.hpp"
#include "include/building.hpp"

using namespace std;

void Building::set_data(Resource resource, string data)
{
    auto big_data = split(data, '|');
    Building::data.push_back({resource, convert(split(big_data[0], ',')), convert(split(big_data[1], ',')), convert(split(big_data[2], ','))});
}

string Building::get_data()
{
    vector<string> res;
    for (const auto &d : Building::data)
        res.push_back(res_to_str(d.resource) + "|" + concat(d.peaks) + "|" + concat(d.peak_hours) + "|" + concat(d.sums));
    return concat(res, '%');
}

int main(int argc, char *argv[])
{
    string path(argv[1]);
    string name(split(path, '/').back());
    string need_resources;
    cin >> need_resources;
    auto reqs = get_resources(need_resources);
    Building my_building;
    for (const auto &req : reqs)
        my_building.set_data(req, create_process("/home/aligh/Desktop/Os/P2/meter", path + "/" + res_to_str(req)));

    create_pipe(name);
    send_to_pipe(name, my_building.get_data());
    cout << "done";
    // log::info("Sent from %s to bill_calc!", name);
}