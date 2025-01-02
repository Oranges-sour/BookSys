#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Log;

struct Book {
    std::string _isbn;
    std::string _name;
    std::string _author;
    std::string _publisher;
};

class Library {
   public:
    Library();

    /*
    _lib_file_list是图书数据库的文件
     对象构造后会异步的加载图书数据库，通过_call_back返回进度
     call_back的bool 代表是否出现故障 false无故障 true出现故障
      false:
      true: int为故障信息的ID（对应日志系统）
    */
    void load_data(const std::string& _lib_file,
                   const std::function<void(std::shared_ptr<Log>)>& _call_back);

    void insert_book(
        const Book& _book,
        const std::function<void(std::shared_ptr<Log>)>& _call_back);

    void erase_book(
        const std::string& _isbn,
        const std::function<void(std::shared_ptr<Log>)>& _call_back);

    void find_book(const std::string& _isbn,
                   const std::function<void(std::shared_ptr<Log>)>& _call_back);

    void update_book(
        const std::string& _isbn, const Book& _book,
        const std::function<void(std::shared_ptr<Log>)>& _call_back);

   private:
    std::mutex mu;
    std::map<std::string, Book> _data;
};

#endif