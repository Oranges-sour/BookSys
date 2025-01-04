#ifndef __UI_H__
#define __UI_H__

#include <functional>
#include <stack>
#include <string>
#include <vector>

class UI_Item {
   public:
    virtual void draw() = 0;
    virtual void call(std::shared_ptr<UI_Item>, const std::string& str) = 0;
    virtual bool selectable() = 0;
    virtual void set_select(bool _sel) = 0;
    virtual int getx() = 0;
    virtual int gety() = 0;
};

class UI_Select {
   public:
    UI_Select() : _sel(false) {}
    bool get() { return _sel; }
    void set(bool sel) { _sel = sel; };

   private:
    bool _sel;
};

class Button : public UI_Item {
   public:
    Button(const std::string& _label, int x, int y,
           const std::function<void(std::shared_ptr<Button>)>& _func);

    virtual void draw() override;
    virtual void call(std::shared_ptr<UI_Item>,
                      const std::string& str) override;
    virtual bool selectable() override;
    virtual void set_select(bool _sel) override;
    virtual int getx() override;
    virtual int gety() override;

    std::string label;  // 按钮的文字
    int x, y;           // 按钮的位置
    UI_Select _sel;

    std::function<void(std::shared_ptr<Button>)> _func;
};

class Text : public UI_Item {
   public:
    std::string label;  // 按钮的文字
    int x, y;           // 按钮的位置

    Text(const std::string& _label, int x, int y);
    virtual void draw() override;
    virtual void call(std::shared_ptr<UI_Item>,
                      const std::string& str) override;
    virtual bool selectable() override;
    virtual void set_select(bool _sel) override;
    virtual int getx() override;
    virtual int gety() override;
};

class Input : public UI_Item {
   public:
    Input(const std::string& _label, int x, int y, int w,
          const std::function<void(std::shared_ptr<Input>, const std::string&)>&
              _func);

    virtual void draw() override;
    virtual void call(std::shared_ptr<UI_Item>,
                      const std::string& str) override;
    virtual bool selectable() override;
    virtual void set_select(bool _sel) override;
    virtual int getx() override;
    virtual int gety() override;

    std::string label;
    int x, y;
    int w;
    UI_Select _sel;

    std::function<void(std::shared_ptr<Input>, const std::string&)> _func;
};

class Box : public UI_Item {
   public:
    Box(int x, int y, int w, int h);
    virtual void draw() override;
    virtual void call(std::shared_ptr<UI_Item>,
                      const std::string& str) override;
    virtual bool selectable() override;
    virtual void set_select(bool _sel) override;
    virtual int getx() override;
    virtual int gety() override;

    int x, y;
    int w, h;
};

class Scene {
   public:
    Scene();

    virtual void init();

    virtual void draw();

    virtual void keyboard(int ch);

    virtual void input(const std::string& str);

    virtual void notice(int _param);

    void add_item(std::shared_ptr<UI_Item> _item);

   private:
    std::vector<std::shared_ptr<UI_Item>> _item;
    std::vector<std::shared_ptr<UI_Item>> _sel_item;

    unsigned int sel_idx;
};

class UI {
   public:
    UI();

    void init();

    void release();

    void interupt();

    bool run();

    void push(std::shared_ptr<Scene> scene);

    // param 传递给下一层的数据
    void pop(int _param);

    void on_input(int x, int y);

   private:
    bool _input_mode;
    int _input_x, _input_y;

    bool _fresh;
    std::stack<std::shared_ptr<Scene>> _scene;
};

UI& ui();

#endif