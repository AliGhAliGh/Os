#ifndef LOG__HPP
#define LOG__HPP

#include <cstdarg>
#include <string>

class log
{
public:
    enum class level
    {
        info,
        warn,
        error,
        none
    };

    static void setLevel(level l);
    static void info(const std::string &fmt, ...);
    static void warn(const std::string &fmt, ...);
    static void error(const std::string &fmt, ...);
    static void perror(const std::string &fmt, ...);

private:
    static void logMsg(const std::string &level, const std::string &fmt, va_list &args, const std::string &perr);
    inline static level lLevel = level::info;
};

#endif