#include "LogSystem.hpp"

#include <cstdlib>
#include <fstream>

LogSystem& log_sys() {
    static LogSystem sys;
    return sys;
}

Log::Log(LogID _id, LogLevel _level, LogTime _time, const std::string& _info)
    : _id(_id), _level(_level), _time(_time), _info(_info) {}

bool Log::ok() { return _level == LogLevel::Info; }

LogID Log::id() { return _id; }

LogLevel Log::level() { return _level; }

LogTime Log::time() { return _time; }

const std::string& Log::info() { return _info; }

std::shared_ptr<Log> LogSystem::new_log(LogLevel _level,
                                        const std::string& _info) {
    LogID id = rand();
    auto log = std::make_shared<Log>(id, _level, time(NULL), _info);
    {
        std::unique_lock lock(mu);
        _data.insert({id, log});
    }
    return log;
}

std::shared_ptr<Log> LogSystem::get_log(LogID _id) {
    std::unique_lock lock(mu);
    auto iter = _data.find(_id);
    if (iter == _data.end()) {
        return nullptr;
    }
    return iter->second;
}

std::shared_ptr<Log> LogSystem::out_to_file(const std::string& _file_name) {
    auto file = std::fstream(_file_name, std::ios_base::app);
    if (!file) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "LogSystem::out_to_file "
                 "无法打开所给定的文件以生成日志！文件:%s",
                 _file_name.c_str());
        return log_sys().new_log(LogLevel::Warn, buffer);
    }
    std::vector<std::shared_ptr<Log>> _temp;

    {
        std::unique_lock lock(mu);
        _temp.reserve(_data.size());
        for (const auto& log : _data) {
            _temp.push_back(log.second);
        }
    }

    std::sort(_temp.begin(), _temp.end(),
              [](const std::shared_ptr<Log>& a, const std::shared_ptr<Log>& b)
                  -> bool { return a->time() < b->time(); });

    for (const auto& log : _temp) {
        // 将时间戳转换为结构体 tm
        LogTime time = log->time();
        struct tm* timeInfo = localtime(&time);

        char time_buffer[128];
        // 格式化时间为字符串，格式为 YYYY-MM-DD HH:MM:SS
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S",
                 timeInfo);
        static char i0[3][16] = {"INFO", "WARN", "ERROR"};

        file << time_buffer << ":[" << i0[(int)log->level()] << "] "
             << log->info() << "\n";
        if (file.fail()) {
            file.close();

            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "LogSystem::out_to_file "
                     "日志文件写入错误！文件:%s",
                     _file_name.c_str());
            return log_sys().new_log(LogLevel::Warn, buffer);
        }
    }
    file.close();

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "LogSystem::out_to_file "
             "日志信息写入文件成功！共写入:%zu 条日志.",
             _temp.size());
    return log_sys().new_log(LogLevel::Info, buffer);
}