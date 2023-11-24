#include <cerrno>
#include <iostream>
#include <cstring>
#include "include/log.hpp"
#include "include/color.hpp"

using namespace std; 

const string INFO  = Color::Green  + "[INFO]"  + Color::Reset;
const string WARN  = Color::Yellow + "[WARN]"  + Color::Reset;
const string ERROR = Color::Red    + "[ERRO]"  + Color::Reset;

void log::setLevel(log::level l)
{
    if (l >= log::level::info && l <= log::level::none)
        lLevel = l;
}

void log::logMsg(const string &level, const string &fmt, va_list &args, const string &perr = "")
{
    cout << level +" ";
    vprintf(fmt.c_str(), args);
    if (perr != "")
        cout << ": " << perr;
    cout << endl;
}

void log::info(const string &fmt, ...)
{
    if (lLevel <= log::level::info)
    {
        va_list args;
        va_start(args, fmt);
        logMsg(INFO, fmt, args);
        va_end(args);
    }
}

void log::warn(const string &fmt, ...)
{
    if (lLevel <= log::level::warn)
    {
        va_list args;
        va_start(args, fmt);
        logMsg(WARN, fmt, args);
        va_end(args);
    }
}

void log::error(const string &fmt, ...)
{
    if (lLevel <= log::level::error)
    {
        va_list args;
        va_start(args, fmt);
        logMsg(ERROR, fmt, args);
        va_end(args);
    }
}

void log::perror(const string &fmt, ...)
{
    if (lLevel <= log::level::error)
    {
        va_list args;
        va_start(args, fmt);
        logMsg(ERROR, fmt, args, strerror(errno));
        va_end(args);
    }
}