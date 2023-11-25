#ifndef LOG__HPP
#define LOG__HPP

#include <string>

class log
{
public:
    enum class level
    {
        dbug,
        info,
        error,
        result,
        none
    };

    static void info(const std::string &msg);
    static void dbug(const std::string &msg);
    static void result(const std::string &msg);
    static void error(const std::string &msg);
    static void perror(const std::string &msg);

private:
    static void logMsg(const std::string &level, const std::string &fmt, const std::string &perr);
    inline static level lLevel = level::dbug;
};

#endif