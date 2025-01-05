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
#include "../base/LogSystem.hpp"
#include "../base/UI.hpp"
using namespace std;
using namespace std::chrono;

constexpr int YES = 1;
constexpr int NO = 2;

MyScene::MyScene() { step = 0; }

void MyScene::init() {
    this->add_item(make_shared<Text>("图书管理信息系统", 3, 1));

    this->add_item(make_shared<Text>("请输入要加载的数据库文件", 3, 5));

    this->add_item(make_shared<Input>(
        "数据库文件", 3, 7, 15,
        [this](shared_ptr<Input> _input, const string& str) {
            data_file = str;
            auto sc = make_shared<ConfirmScene>("是否开始加载数据库？");
            ui().push(sc);
        }));

    this->add_item(make_shared<Button>(
        "[ 退出系统 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));

    // this->add_item(make_shared<Box>(3, 11, 2, 2));
}

void MyScene::notice(int x) {
    Scene::notice(x);
    if (x == YES && step == 0) {
        step += 1;
        auto sc = make_shared<LoadDataScene>(data_file);
        ui().push(sc);
        return;
    }
    if (x == YES && step == 1) {
        step += 1;

        ui().pop(0);
        auto sc = make_shared<LobbyScene>();
        ui().push(sc);
        return;
    }

    if (x == NO && step == 1) {
        step = 0;
        return;
    }
}

void LobbyScene::init() {
    this->add_item(make_shared<Text>("图书管理信息系统  v0.1", 3, 1));

    this->add_item(make_shared<Button>(
        "[ 退出系统 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

LoadDataScene::LoadDataScene(const std::string& str) { data_file = str; }

void LoadDataScene::init() {
    this->add_item(make_shared<Text>("正在加载数据库...", 3, 1));
    this->add_item(make_shared<Text>(data_file, 3, 3));

    library().load_data(data_file, [](shared_ptr<Log> log) {
        if (log->ok()) {
            ui().pop(YES);
        } else {
            ui().pop(NO);
            auto sc =
                make_shared<OkScene>("数据库加载失败, 请确认文件是否正确!");
            ui().push(sc);
        }
    });
}

ConfirmScene::ConfirmScene(const std::string& str) { title = str; }

void ConfirmScene::init() {
    this->add_item(make_shared<Text>(title, 3, 1));

    this->add_item(make_shared<Button>(
        "[ 是 ]", 3, 3, [](shared_ptr<Button> _btn) { ui().pop(YES); }));

    this->add_item(make_shared<Button>(
        "[ 否 ]", 10, 3, [](shared_ptr<Button> _btn) { ui().pop(NO); }));
}

OkScene::OkScene(const std::string& str) { title = str; }

void OkScene::init() {
    this->add_item(make_shared<Text>(title, 3, 1));

    this->add_item(make_shared<Button>(
        "[ 好 ]", 3, 3, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}