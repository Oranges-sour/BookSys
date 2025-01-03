#ifndef __LOG_SYSTEM__
#define __LOG_SYSTEM__

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

using LogID = int;

using LogTime = std::chrono::system_clock::time_point;

enum class LogLevel { Info, Warn, Error };

class Log {
   public:
    Log(LogID _id, LogLevel _level, LogTime _time, const std::string& _info);

    bool ok();
    LogID id();
    LogLevel level();
    LogTime time();
    const std::string& info();

   private:
    LogID _id;
    LogLevel _level;
    LogTime _time;
    std::string _info;
};

class LogSystem {
   public:
    std::shared_ptr<Log> new_log(LogLevel _level, const std::string& _info);

    std::shared_ptr<Log> get_log(LogID _id);

    bool out_to_file(const std::string& _file_name);

   private:
    std::mutex mu;
    std::vector<std::shared_ptr<Log>> _data;
};

LogSystem& log_sys();

#endif