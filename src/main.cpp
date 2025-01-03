#include <iostream>
#include <random>
#include <thread>

#include "ncurses.h"
#include "Library.hpp"
#include "LogSystem.hpp"
using namespace std;
using namespace std::chrono;

Library lib;

std::string rs(size_t length);


void draw_input_box(int start_y, int start_x, int width, int height,
                    const char *label) {
    // 画输入框边框
    mvprintw(start_y - 1, start_x, "%s", label);
    for (int i = 0; i < width; ++i) {
        mvaddch(start_y, start_x + i, '-');
        mvaddch(start_y + height - 1, start_x + i, '-');
    }
    for (int i = 0; i < height; ++i) {
        mvaddch(start_y + i, start_x, '|');
        mvaddch(start_y + i, start_x + width - 1, '|');
    }
    mvaddch(start_y, start_x, '+');
    mvaddch(start_y, start_x + width - 1, '+');
    mvaddch(start_y + height - 1, start_x, '+');
    mvaddch(start_y + height - 1, start_x + width - 1, '+');
}

int main() {
    // auto t0 = steady_clock::now();
    // lib.load_data("data1.txt", [t0](std::shared_ptr<Log> log) {
    //     // for (int i = 1; i <= 100000; ++i) {
    //     //     lib.insert_book({rs(10), rs(5), rs(5), rs(5)},
    //     //                     [](std::shared_ptr<Log> log) {});
    //     // }
    //     auto t1 = steady_clock::now();
    //     cout << "数据库构建完成！"
    //          << duration_cast<milliseconds>(t1 - t0).count() << "ms" << endl;
    // });
    // int kkk;
    // scanf("%d", &kkk);
    // lib.save_data("data1.txt", [](std::shared_ptr<Log> log) {});
    // std::this_thread::sleep_for(300ms);
    // cout << log_sys().out_to_file("log1.txt") << endl;


    setlocale(LC_ALL, "");
    return 0;
}

std::string rs(size_t length) {
    // 定义可能的字符集
    const std::string charSet =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    std::random_device rd;         // 获得可用的随机数种子
    std::mt19937 generator(rd());  // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<> distribution(0, charSet.size() - 1);

    std::string randomString;
    for (size_t i = 0; i < length; ++i) {
        // 在字符集中随机选择一个字符并添加到结果字符串中
        randomString += charSet[distribution(generator)];
    }

    return randomString;
}