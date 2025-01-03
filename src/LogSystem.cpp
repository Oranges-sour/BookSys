#include "LogSystem.hpp"

#include <cstdlib>
#include <fstream>

#include "ThreadPool.hpp"

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
    auto time = std::chrono::system_clock::now();
    auto log = std::make_shared<Log>(0, _level, time, _info);

    thread_pool().push([this, log]() {
        std::unique_lock lock(mu);
        _data.push_back(log);
    });
    return log;
}

std::shared_ptr<Log> LogSystem::get_log(LogID _id) { return nullptr; }

bool LogSystem::out_to_file(const std::string& _file_name) {
    auto file = std::fstream(_file_name, std::ios_base::app);
    if (!file) {
        return false;
    }
    std::vector<std::shared_ptr<Log>> _temp;

    {
        std::unique_lock lock(mu);
        _temp = _data;
    }

    std::sort(_temp.begin(), _temp.end(),
              [](const std::shared_ptr<Log>& a, const std::shared_ptr<Log>& b)
                  -> bool { return a->time() < b->time(); });

    for (const auto& log : _temp) {
        // 将时间戳转换为结构体 tm
        LogTime time = log->time();
        auto timeT = std::chrono::system_clock::to_time_t(time);
        struct tm* timeInfo = localtime(&timeT);

        auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(
                            time.time_since_epoch()) %
                        1000;

        char time_buffer[128];
        // 格式化时间为字符串，格式为 YYYY-MM-DD HH:MM:SS
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S",
                 timeInfo);
        static char i0[3][16] = {"INFO", "WARN", "ERROR"};

        file << time_buffer << ":" << millisec.count() << " ["
             << i0[(int)log->level()] << "] " << log->info() << "\n";
        if (file.fail()) {
            file.close();
            return false;
        }
    }
    file.close();

    return true;
}