#ifndef BUILDING__HPP
#define BUILDING__HPP

#include "defs.hpp"

class Building
{
public:
    void set_data(Resource resource, std::string data);
    std::string get_data();

private:
    std::vector<Meter> data;
};

#endif