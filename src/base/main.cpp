#include <ncurses.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <random>
#include <stack>
#include <thread>

#include "../user/MyScene.hpp"
#include "Library.hpp"
#include "LogSystem.hpp"
#include "ThreadPool.hpp"
#include "UI.hpp"
using namespace std;
using namespace std::chrono;

int main() {
    setlocale(LC_ALL, "");
    ui().init();

    auto sc = make_shared<MyScene>();
    ui().push(sc);

    while (ui().run()) {
        this_thread::sleep_for(1ms);
    }
    ui().release();
    log_sys().out_to_file("log1.txt");
    return 0;
}