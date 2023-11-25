#include <cerrno>
#include <iostream>
#include <cstring>
#include "include/log.hpp"
#include "include/color.hpp"

using namespace std;

const string DBUG = Color::Yellow + "[DBUG]" + Color::Reset;
const string INFO = Color::Green + "[INFO]" + Color::Reset;
const string ERROR = Color::Red + "[ERRO]" + Color::Reset;
const string RESULT = Color::Cyan + "[RSLT]" + Color::Reset;

void log::logMsg(const string &level, const string &msg, const string &perr = "")
{
    string res = level + " " + msg + (perr != "" ? ": " + perr + "\n" : "\n");
    write(STDERR_FILENO, res.c_str(), res.size());
}

void log::info(const string &msg)
{
    if (lLevel <= log::level::info)
    {
        logMsg(INFO, msg);
    }
}

void log::dbug(const string &msg)
{
    if (lLevel <= log::level::dbug)
    {
        logMsg(DBUG, msg);
    }
}

void log::result(const string &msg)
{
    if (lLevel <= log::level::result)
    {
        logMsg(RESULT, msg);
    }
}

void log::error(const string &msg)
{
    if (lLevel <= log::level::error)
    {
        logMsg(ERROR, msg);
    }
}

void log::perror(const string &msg)
{
    if (lLevel <= log::level::error)
    {
        logMsg(ERROR, msg, strerror(errno));
    }
}