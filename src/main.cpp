#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <random>
#include <stack>
#include <thread>

#include "Library.hpp"
#include "LogSystem.hpp"
#include "ThreadPool.hpp"
#include "UI.hpp"
#include "ncurses.h"
using namespace std;
using namespace std::chrono;

Library lib;

std::string rs(size_t length);

// 主函数
int main() {
    initscr();  // 初始化 ncurses
    noecho();   // 不回显输入
    cbreak();   // 立即响应按键
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);  // 开启键盘输入
    curs_set(0);           // 隐藏光标

    Scene sc;
    sc.add_button(Button{"Start", 3, 3});
    sc.add_button(Button{"Quit", 3, 4});

    scene_stack().push(sc);

    while (!scene_stack().empty()) {
        auto& sc = scene_stack().top();
        sc.draw();

        int key = 0;
        while (true) {
            int ch = getch();
            if (ch == ERR) {
                this_thread::sleep_for(1ms);
            } else {
                key = ch;
                break;
            }
        }
        sc.keyboard(key);
    }

    endwin();  // 退出 ncurses 模式
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
