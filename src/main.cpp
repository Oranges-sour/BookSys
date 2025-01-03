#include <iostream>
#include <random>
#include <thread>

#include "Library.hpp"
#include "LogSystem.hpp"
#include "ncurses.h"
using namespace std;
using namespace std::chrono;

Library lib;

std::string rs(size_t length);

int main() {
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