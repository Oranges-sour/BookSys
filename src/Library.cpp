#include "Library.hpp"

#include <fstream>
#include <future>

#include "LogSystem.hpp"

void Library::load_data(
    const std::string& _lib_file,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    std::async([this, &_lib_file, &_call_back]() {
        log_sys().new_log(LogLevel::Info, "图书馆初始化，读取数据库!");
        auto file = std::fstream(_lib_file, std::ios_base::in);
        if (!file) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Library::load_data "
                     "无法打开所给定的文件以读取数据库！文件:%s",
                     _lib_file.c_str());
            _call_back(log_sys().new_log(LogLevel::Error, buffer));
            return;
        }

        int p = 0;
        std::map<std::string, Book> _new_data;
        while (!file.eof()) {
            Book book;
            file >> book._isbn >> book._name >> book._author >> book._publisher;
            if (file.fail()) {
                char buffer[256];
                snprintf(buffer, sizeof(buffer),
                         "Library::load_data "
                         "数据库格式有误，无法完成读取！位置:%d",
                         p);
                _call_back(log_sys().new_log(LogLevel::Error, buffer));
                return;
            }

            _new_data.insert({book._isbn, book});
            p += 1;
        }

        {
            std::unique_lock(mu);
            _data = std::move(_new_data);
        }

        file.close();
        _call_back(log_sys().new_log(LogLevel::Info, "图书馆初始化完毕!"));
    });
}

void Library::insert_book(
    const Book& _book,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    std::async([this, &_book, &_call_back]() {
        bool suc = false;
        {
            std::unique_lock(mu);
            auto iter = _data.find(_book._isbn);
            if (iter == _data.end()) {
                _data.insert({_book._isbn, _book});
                suc = true;
            }
        }
        if (suc) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "成功插入数据！ ISBN:%s",
                     _book._isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Info, buffer));
        } else {
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "插入数据失败，对应ISBN号已存在！ ISBN:%s",
                     _book._isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Warn, buffer));
        }
    });
}

void Library::erase_book(
    const std::string& _isbn,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    std::async([this, &_isbn, &_call_back]() {
        bool suc = false;
        {
            std::unique_lock(mu);
            auto iter = _data.find(_isbn);
            if (iter != _data.end()) {
                _data.erase(iter);
                suc = true;
            }
        }
        if (suc) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "成功删除数据！ ISBN:%s",
                     _isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Info, buffer));
        } else {
            char buffer[256];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "删除数据失败，无对应ISBN号！ ISBN:%s",
                     _isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Warn, buffer));
        }
    });
}
