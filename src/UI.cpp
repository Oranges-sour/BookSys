#include "UI.hpp"

#include "ncurses.h"

std::stack<Scene>& scene_stack() {
    static std::stack<Scene> _s;
    return _s;
}

void Scene::add_button(const Button& button) { _button.push_back(button); }

void Scene::add_text(const Text& button) { _text.push_back(button); }

void Scene::add_input(const Input& button) { _input.push_back(button); }

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

void Scene::keyboard(int ch) {
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

Button::Button(const std::string& _label, int x, int y)
    : label(_label), x(x), y(y), selected(false) {}

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

Input::Input(const std::string& _label, int x, int y, int w, int h)
    : label(_label), x(x), y(y), w(w), h(h) {}

void Input::draw() { mvprintw(y, x, "input:%s", label.c_str()); }