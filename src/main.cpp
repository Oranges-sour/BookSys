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
    setlocale(LC_ALL, "");
    ui().init();

    auto sc = make_shared<Scene>();
    sc->add_button(
        Button{"Start", 3, 3, [](Button& _btn) { _btn.label += "a"; }});
    sc->add_button(Button{"Quit", 3, 5, [](Button&) {}});
    sc->add_text(Text{"Hello", 3, 1});
    sc->add_input(
        Input{"ISBN号", 3, 7, 15,
              [](Input& _input, const string& str) { _input.x += 1; }});

    ui().push(sc);

    while (ui().run()) {
        this_thread::sleep_for(1ms);
    }
    ui().release();
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
