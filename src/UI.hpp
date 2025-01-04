#ifndef __UI_H__
#define __UI_H__

#include <stack>
#include <string>
#include <vector>

class Button {
   public:
    std::string label;  // 按钮的文字
    int x, y;           // 按钮的位置
    bool selected;      // 是否被选中

    Button(const std::string& _label, int x, int y);
    void draw();
};

struct Text {
   public:
    std::string label;  // 按钮的文字
    int x, y;           // 按钮的位置

    Text(const std::string& _label, int x, int y);
    void draw();
};

struct Input {
   public:
    std::string label;
    int x, y;
    int w, h;
    std::string input;
    bool selected;  // 是否被选中

    Input(const std::string& _label, int x, int y, int w, int h);
    void draw();
};

class Scene {
   public:
    void draw();

    void keyboard(int ch);

    void add_button(const Button& _button);
    void add_text(const Text& _text);
    void add_input(const Input& _input);

   private:
    std::vector<Button> _button;
    std::vector<Text> _text;
    std::vector<Input> _input;

    unsigned int sel_idx;
};

class UI {
   public:
    UI();

    void init();

    void release();

    void interupt();

    bool run();

    void push(const Scene& scene);

    // param 传递给下一层的数据
    void pop(int _param);

    void on_input(int x, int y);

   private:
    bool _input_mode;
    int _input_x, _input_y;

    bool _fresh;
    std::stack<Scene> _scene;
};

UI& ui();

#endif