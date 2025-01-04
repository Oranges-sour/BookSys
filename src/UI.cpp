#include "UI.hpp"

#include <thread>

#include "ncurses.h"
using namespace std;

UI& ui() {
    static UI _ui;
    return _ui;
}

UI::UI() {
    _fresh = false;
    _input_mode = false;
    _input_x = 0;
    _input_y = 0;
}

void UI::init() {
    initscr();  // 初始化 ncurses
    noecho();   // 不回显输入
    cbreak();   // 立即响应按键
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);  // 开启键盘输入
    curs_set(0);           // 隐藏光标
}

void UI::release() {
    endwin();  // 退出 ncurses 模式
}

void UI::interupt() { _fresh = true; }

void UI::on_input(int x, int y) {
    _input_mode = true;
    _input_x = x;
    _input_y = y;
}

void UI::push(std::shared_ptr<Scene> scene) {
    _scene.push(scene);
    this->interupt();
}

void UI::pop(int _param) {
    _scene.pop();
    if (!_scene.empty()) {
        _scene.top()->notice(_param);
    }
}

bool UI::run() {
    if (_scene.empty()) {
        return false;
    }
    auto& sc = _scene.top();
    if (_fresh) {
        sc->draw();
        _fresh = false;
    }

    if (!_input_mode) {
        int ch = getch();
        if (ch != ERR) {
            sc->keyboard(ch);

            this->interupt();
        }
    }
    if (_input_mode) {
        nodelay(stdscr, FALSE);
        move(_input_y, _input_x);
        echo();       // 打开回显以便用户看见输入
        curs_set(1);  // 显示光标
        char buffer[1024];
        mvgetnstr(_input_y, _input_x, buffer, sizeof(buffer));

        sc->input(buffer);

        noecho();
        curs_set(0);
        _input_mode = false;
        this->interupt();
        nodelay(stdscr, TRUE);
    }

    return true;
}

void Scene::init() {}

void Scene::add_button(const Button& button) {
    _button.push_back(button);
    ui().interupt();
}

void Scene::add_text(const Text& button) {
    _text.push_back(button);
    ui().interupt();
}

void Scene::add_input(const Input& button) {
    _input.push_back(button);
    ui().interupt();
}

void Scene::draw() {
    clear();
    for (auto& it : _button) {
        it.draw();
    }

    for (auto& it : _text) {
        it.draw();
    }

    for (auto& it : _input) {
        it.draw();
    }
    refresh();
}

void Scene::input(const std::string& str) {
    unsigned int p = sel_idx % (_button.size() + _input.size());
    if (p >= _button.size()) {
        p -= _button.size();
        _input[p]._func(_input[p], str);
    }
}

void Scene::notice(int _param) {}

void Scene::keyboard(int ch) {
    if (ch == '\n') {
        unsigned int p = sel_idx % (_button.size() + _input.size());
        if (p >= _button.size()) {
            p -= _button.size();
            ui().on_input(_input[p].x + 1, _input[p].y + 1);
        } else {
            _button[p]._func(_button[p]);
        }
    }
    if (ch == KEY_RIGHT) {
        sel_idx += 1;
    }
    if (ch == KEY_LEFT) {
        sel_idx -= 1;
    }
    for (auto& it : _button) {
        it.selected = false;
    }
    for (auto& it : _input) {
        it.selected = false;
    }

    unsigned int p = sel_idx % (_button.size() + _input.size());
    if (p >= _button.size()) {
        p -= _button.size();
        _input[p].selected = true;
    } else {
        _button[p].selected = true;
    }
}

Button::Button(const std::string& _label, int x, int y,
               const std::function<void(Button&)>& _func)
    : label(_label), x(x), y(y), selected(false), _func(_func) {}

void Button::draw() {
    if (selected) {
        attron(A_REVERSE);  // 选中的按钮高亮
    }
    mvprintw(y, x, "%s", label.c_str());
    if (selected) {
        attroff(A_REVERSE);
    }
}

Text::Text(const std::string& _label, int x, int y)
    : label(_label), x(x), y(y) {}

void Text::draw() { mvprintw(y, x, "%s", label.c_str()); }

Input::Input(const std::string& _label, int x, int y, int w,
             const std::function<void(Input&, const std::string&)>& _func)
    : label(_label), x(x), y(y), w(w), _func(_func), selected(false) {}

void Input::draw() {
    mvhline(y, x, ACS_HLINE, w);  // 输入框的顶部边框

    mvvline(y, x, ACS_VLINE, 2);      // 左边框
    mvvline(y, x + w, ACS_VLINE, 2);  // 右边框

    mvhline(y + 2, x, ACS_HLINE, w);  // 底部边框
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w, ACS_URCORNER);
    mvaddch(y + 2, x, ACS_LLCORNER);
    mvaddch(y + 2, x + w, ACS_LRCORNER);

    if (selected) {
        attron(A_REVERSE);  // 选中的按钮高亮
    }
    mvprintw(y, x, "%s ", label.c_str());
    if (selected) {
        attroff(A_REVERSE);
    }
}