#include <iostream>

#include "Library.hpp"
#include "LogSystem.hpp"
using namespace std;

int main() {
    log_sys().new_log(LogLevel::Info, "hi");
    log_sys().new_log(LogLevel::Error, "XX出现错误");

    cout << log_sys().out_to_file("log1.txt")->info() << endl;

    return 0;
}