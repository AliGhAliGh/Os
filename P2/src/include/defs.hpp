#ifndef DEFS__HPP
#define DEFS__HPP

#include "log.hpp"
#include <vector>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

constexpr int buffer_size = 512;
constexpr int day_of_month = 30;
constexpr int hour_of_day = 6;
constexpr int month_of_year = 12;

enum Resource
{
    Gas,
    Electricity,
    Water
};

typedef struct meter
{
    Resource resource;
    std::vector<int> peaks;
    std::vector<int> peak_hours;
    std::vector<int> sums;
} Meter;

typedef struct report
{
    std::vector<float> averages;
    std::vector<int> sums;
    std::vector<int> peak_hours;
    std::vector<float> bill;
    std::vector<float> diff;
} Report;

inline const std::string res_to_str(Resource v)
{
    switch (v)
    {
    case Gas:
        return "Gas";
    case Electricity:
        return "Electricity";
    case Water:
        return "Water";
    default:
        throw;
    }
}

inline Resource str_to_res(const std::string s)
{
    if (s == "Gas")
        return Resource::Gas;
    else if (s == "Electricity")
        return Resource::Electricity;
    else if (s == "Water")
        return Resource::Water;
    else
        throw;
}

std::vector<std::string> split(const std::string &str, char delim = ' ');

std::vector<std::string> get_dirs(const std::string &path);

std::vector<std::vector<int>> get_csv(const std::string &path);

std::vector<int> convert(std::vector<std::string> data);

std::vector<float> convert_f(std::vector<std::string> data);

std::string create_process(const char *proc_name, std::string send_data, std::string param);

std::string create_process(const char *proc_name, std::string param);

void send_to_pipe(const std::string &name, const std::string &data);

void create_pipe(const std::string &name);

void delete_pipe(const std::string &name);

std::string recv_from_pipe(const std::string &name);

std::vector<Resource> get_resources(std::string input);

template <typename T>
std::string concat(std::vector<T> data, const char delim = ',')
{
    std::ostringstream data_stream;
    copy(data.begin(), data.end() - 1, std::ostream_iterator<T>(data_stream, &delim));
    data_stream << data.back();
    return data_stream.str();
}

#endif