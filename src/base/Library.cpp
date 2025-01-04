#include "Library.hpp"

#include <fstream>
#include <future>

#include "LogSystem.hpp"
#include "ThreadPool.hpp"

Library& library() {
    static Library lib;
    return lib;
}

void Library::load_data(
    const std::string& _lib_file,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    thread_pool().push([this, _lib_file, _call_back]() {
        log_sys().new_log(LogLevel::Info, "图书馆初始化，读取数据库!");
        auto file = std::fstream(_lib_file, std::ios_base::in);
        if (!file) {
            char buffer[512];
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
                char buffer[512];
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
            std::unique_lock lock(mu);
            _data = std::move(_new_data);
        }

        file.close();
        _call_back(log_sys().new_log(LogLevel::Info, "图书馆初始化完毕!"));
    });
}

void Library::save_data(
    const std::string& _lib_file,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    thread_pool().push([this, _lib_file, _call_back]() {
        log_sys().new_log(LogLevel::Info, "图书馆存储数据库!");
        auto file = std::fstream(_lib_file, std::ios_base::out);
        if (!file) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::save_data "
                     "无法打开所给定的文件以存储数据库！文件:%s",
                     _lib_file.c_str());
            _call_back(log_sys().new_log(LogLevel::Error, buffer));
            return;
        }

        std::vector<Book> _temp;
        {
            std::unique_lock lock(mu);
            for (const auto& book : _data) {
                _temp.push_back(book.second);
            }
        }
        int p = 0;
        for (const auto& book : _temp) {
            if (p > 0) {
                file << "\n";
            }
            file << book._isbn << " " << book._name << " " << book._author
                 << " " << book._publisher;
            if (file.fail()) {
                char buffer[512];
                snprintf(buffer, sizeof(buffer),
                         "Library::save_data "
                         "文件写入失败，无法完成存储！位置:%d",
                         p);
                _call_back(log_sys().new_log(LogLevel::Error, buffer));
                return;
            }
            p += 1;
        }

        file.close();

        char buffer[512];
        snprintf(buffer, sizeof(buffer),
                 "Library::save_data "
                 "图书馆数据库存储完毕! 共存储:%d 条书籍信息.",
                 p);
        _call_back(log_sys().new_log(LogLevel::Info, buffer));
    });
}

void Library::insert_book(
    const Book& _book,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    thread_pool().push([this, _book, _call_back]() {
        bool suc = false;
        {
            std::unique_lock lock(mu);
            auto iter = _data.find(_book._isbn);
            if (iter == _data.end()) {
                _data.insert({_book._isbn, _book});
                suc = true;
            }
        }
        if (suc) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "成功插入数据！ ISBN:%s",
                     _book._isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Info, buffer));
        } else {
            char buffer[512];
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
    thread_pool().push([this, _isbn, _call_back]() {
        bool suc = false;
        {
            std::unique_lock lock(mu);
            auto iter = _data.find(_isbn);
            if (iter != _data.end()) {
                _data.erase(iter);
                suc = true;
            }
        }
        if (suc) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "成功删除数据！ ISBN:%s",
                     _isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Info, buffer));
        } else {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::insert_book "
                     "删除数据失败，无对应ISBN号！ ISBN:%s",
                     _isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Warn, buffer));
        }
    });
}

void Library::find_book(
    const std::string& _isbn,
    const std::function<void(Book&&, std::shared_ptr<Log>)>& _call_back) {
    thread_pool().push([this, _isbn, _call_back]() {
        bool suc = false;
        Book _book;
        {
            std::shared_lock lock(mu);
            auto iter = _data.find(_isbn);
            if (iter != _data.end()) {
                _book = iter->second;
                suc = true;
            }
        }
        if (suc) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::find_book "
                     "成功查找数据！ ISBN:%s",
                     _isbn.c_str());
            _call_back(std::move(_book),
                       log_sys().new_log(LogLevel::Info, buffer));
        } else {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::find_book "
                     "查找数据失败，无对应ISBN号！ ISBN:%s",
                     _isbn.c_str());
            _call_back(std::move(_book),
                       log_sys().new_log(LogLevel::Warn, buffer));
        }
    });
}

void Library::update_book(
    const std::string& _isbn, const Book& _book,
    const std::function<void(std::shared_ptr<Log>)>& _call_back) {
    thread_pool().push([this, _isbn, _book, _call_back]() {
        bool suc = false;
        {
            std::unique_lock lock(mu);
            auto iter = _data.find(_isbn);
            if (iter != _data.end()) {
                iter->second = _book;
                suc = true;
            }
        }
        if (suc) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::update_book "
                     "成功更改数据！ ISBN:%s",
                     _isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Info, buffer));
        } else {
            char buffer[512];
            snprintf(buffer, sizeof(buffer),
                     "Library::update_book "
                     "更改数据失败，无对应ISBN号！ ISBN:%s",
                     _isbn.c_str());
            _call_back(log_sys().new_log(LogLevel::Warn, buffer));
        }
    });
}