#include "MyScene.hpp"

#include <ncurses.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <random>
#include <stack>
#include <thread>

#include "../base/Library.hpp"
#include "../base/UI.hpp"
using namespace std;
using namespace std::chrono;

void MyScene::init() {
    this->add_item(make_shared<Text>("图书管理信息系统", 3, 1));

    this->add_item(make_shared<Button>(
        "Start", 3, 3, [](shared_ptr<Button> _btn) { _btn->label += "a"; }));
    this->add_item(
        make_shared<Button>("Quit", 3, 5, [](shared_ptr<Button>) {}));
    this->add_item(make_shared<Text>("Hello", 3, 1));
    this->add_item(make_shared<Input>(
        "ISBN号", 3, 7, 15,
        [](shared_ptr<Input> _input, const string& str) { _input->x += 1; }));
    this->add_item(make_shared<Box>(3, 11, 2, 2));
}