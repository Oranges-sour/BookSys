#include "UI.hpp"

#include <thread>

#include "_ncurses.h"
using namespace std;

UI& ui() {
    static UI _ui;
    return _ui;
}

UI::UI() {
    _fresh = false;
    _input_mode = false;
    _quit = false;
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

void UI::refresh() { _fresh = true; }

void UI::on_input(int x, int y) {
    _input_mode = true;
    _input_x = x;
    _input_y = y;
}

void UI::push(std::shared_ptr<Scene> scene) {
    scene->init();
    _scene.push(scene);
    this->refresh();
}

void UI::pop(int _param) {
    _scene.pop();
    if (!_scene.empty()) {
        _scene.top()->notice(_param);
    }
    this->refresh();
}

bool UI::run() {
    if (_quit) {
        return false;
    }
    if (_scene.empty()) {
        return true;
    }
    auto sc = _scene.top();
    if (_fresh) {
        sc->draw();
        _fresh = false;
    }

    if (!_input_mode) {
        int ch = getch();
        if (ch != ERR) {
            sc->keyboard(ch);

            this->refresh();
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
        this->refresh();
        nodelay(stdscr, TRUE);
    }

    return true;
}

void UI::quit() { _quit = true; }

Scene::Scene() : sel_idx(0) {}

void Scene::init() {}

void Scene::add_item(shared_ptr<UI_Item> _item) {
    if (_item->selectable()) {
        _sel_item.push_back(_item);
    }
    this->_item.push_back(_item);
    this->update_select();
}

void Scene::draw() {
    clear();
    box(stdscr, ACS_VLINE, ACS_HLINE);
    for (auto& it : _item) {
        it->draw();
    }
    refresh();
}

void Scene::input(const std::string& str) {
    if (_sel_item.size() == 0) {
        return;
    }
    unsigned int p = sel_idx % _sel_item.size();
    _sel_item[p]->call(_sel_item[p], str);
}

void Scene::notice(int _param) {}

void Scene::keyboard(int ch) {
    if (ch == '\n') {
        if (_sel_item.size() > 0) {
            unsigned int p = sel_idx % _sel_item.size();
            if (_sel_item[p]->inputable()) {
                ui().on_input(_sel_item[p]->getx() + 1,
                              _sel_item[p]->gety() + 1);
            } else {
                _sel_item[p]->call(_sel_item[p], string{});
            }
        }
    }
    if (ch == KEY_DOWN) {
        sel_idx += 2;
    }
    if (ch == KEY_UP) {
        sel_idx -= 2;
    }
    if (ch == KEY_RIGHT) {
        sel_idx += 1;
    }
    if (ch == KEY_LEFT) {
        sel_idx -= 1;
    }
    this->update_select();
}

void Scene::update_select() {
    if (_sel_item.size() > 0) {
        for (auto it : _sel_item) {
            it->set_select(false);
        }
        unsigned int p = sel_idx % _sel_item.size();
        _sel_item[p]->set_select(true);
    }
}

Button::Button(const std::string& _label, int x, int y,
               const std::function<void(shared_ptr<Button>)>& _func)
    : label(_label), x(x), y(y), _func(_func) {}

void Button::draw() {
    if (_sel.get()) {
        attron(A_REVERSE);  // 选中的按钮高亮
    }
    mvprintw(y, x, "%s", label.c_str());
    if (_sel.get()) {
        attroff(A_REVERSE);
    }
}

void Button::call(shared_ptr<UI_Item> _ptr, const string& str) {
    _func(dynamic_pointer_cast<Button>(_ptr));
}

bool Button::selectable() { return true; }

bool Button::inputable() { return false; }

void Button::set_select(bool _sel) { this->_sel.set(_sel); }

int Button::getx() { return x; }
int Button::gety() { return y; }

Text::Text(const std::string& _label, int x, int y)
    : label(_label), x(x), y(y) {}

void Text::draw() { mvprintw(y, x, "%s", label.c_str()); }

void Text::call(shared_ptr<UI_Item> _ptr, const string& str) {}

bool Text::selectable() { return false; }

bool Text::inputable() { return false; }

void Text::set_select(bool _sel) {}

int Text::getx() { return x; }
int Text::gety() { return y; }

Input::Input(
    const std::string& _label, int x, int y, int w,
    const std::function<void(shared_ptr<Input>, const std::string&)>& _func)
    : label(_label), x(x), y(y), w(w), _func(_func) {}

void Input::draw() {
    mvhline(y, x, ACS_HLINE, w);  // 输入框的顶部边框

    mvvline(y, x, ACS_VLINE, 2);      // 左边框
    mvvline(y, x + w, ACS_VLINE, 2);  // 右边框

    mvhline(y + 2, x, ACS_HLINE, w);  // 底部边框
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w, ACS_URCORNER);
    mvaddch(y + 2, x, ACS_LLCORNER);
    mvaddch(y + 2, x + w, ACS_LRCORNER);

    if (_sel.get()) {
        attron(A_REVERSE);  // 选中的按钮高亮
    }
    mvprintw(y, x, "%s ", label.c_str());
    if (_sel.get()) {
        attroff(A_REVERSE);
    }
}

void Input::call(shared_ptr<UI_Item> _ptr, const string& str) {
    _func(dynamic_pointer_cast<Input>(_ptr), str);
}

bool Input::selectable() { return true; }

bool Input::inputable() { return true; }

void Input::set_select(bool _sel) { this->_sel.set(_sel); }

int Input::getx() { return x; }
int Input::gety() { return y; }

Box::Box(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}

void Box::draw() {
    mvhline(y, x, ACS_HLINE, w - 1);
    mvvline(y, x, ACS_VLINE, h - 1);
    mvvline(y, x + w - 1, ACS_VLINE, h - 1);
    mvhline(y + h - 1, x, ACS_HLINE, w - 1);

    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + w - 1, ACS_URCORNER);
    mvaddch(y + h - 1, x, ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
}

void Box::call(shared_ptr<UI_Item>, const string& str) {}

bool Box::selectable() { return false; }

bool Box::inputable() { return false; }

void Box::set_select(bool) {}

int Box::getx() { return x; }
int Box::gety() { return y; }