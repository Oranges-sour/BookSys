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

    this->add_item(
        make_shared<Input>("数据库文件", 3, 7, 15,
                           [this](shared_ptr<Input> _input, const string& str) {
                               data_file = str;
                               auto sc = make_shared<LoadDataScene>(data_file);
                               ui().push(sc);
                               step += 1;
                           }));

    this->add_item(make_shared<Button>(
        "[ 退出系统 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));

    // this->add_item(make_shared<Box>(3, 11, 2, 2));
}

void MyScene::notice(int x) {
    Scene::notice(x);
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
        "[ 数据库基本信息 ]", 3, 5, [](shared_ptr<Button> _btn) {
            auto sc = make_shared<LibraryInfoScene>();
            ui().push(sc);
        }));

    this->add_item(
        make_shared<Button>("[ 查询书籍 ]", 3, 6, [](shared_ptr<Button> _btn) {
            auto sc = make_shared<SearchBookISBNScene>();
            ui().push(sc);
        }));

    this->add_item(make_shared<Button>(
        "[ 修改书籍信息 ]", 3, 7, [](shared_ptr<Button> _btn) {
            auto sc = make_shared<EditBookSearchISBNScene>();
            ui().push(sc);
        }));

    this->add_item(
        make_shared<Button>("[ 删除书籍 ]", 3, 8, [](shared_ptr<Button> _btn) {
            auto sc = make_shared<DelBookScene>();
            ui().push(sc);
        }));

    this->add_item(make_shared<Button>(
        "[ 备份数据库数据 ]", 3, 11, [](shared_ptr<Button> _btn) {
            auto sc = make_shared<BackupDataScene>();
            ui().push(sc);
        }));

    this->add_item(make_shared<Button>(
        "[ 退出系统 ]", 3, 14, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

void DelBookScene::init() {
    this->add_item(make_shared<Text>("删除书籍", 3, 1));

    this->add_item(make_shared<Text>("请输入要删除的书籍的ISBN号", 3, 5));

    this->add_item(make_shared<Input>(
        "ISBN号", 3, 7, 15,
        [this](shared_ptr<Input> _input, const string& str) {
            if (str.size() == 0) {
                return;
            }
            isbn_str = str;

            bool suc = true;
            for (auto ch : isbn_str) {
                if (!isnumber(ch)) {
                    suc = false;
                }
            }

            if (!suc) {
                auto sc = make_shared<OkScene>("输入的ISBN号格式不正确！");
                ui().push(sc);
            } else {
                library().find_book(
                    isbn_str, [](Book&& book, shared_ptr<Log> log) {
                        if (log->ok()) {
                            auto sc = make_shared<DelBookCheckScene>(book);
                            ui().push(sc);
                        } else {
                            auto sc = make_shared<OkScene>("未找到对应书籍！");
                            ui().push(sc);
                        }
                    });
            }
        }));

    this->add_item(make_shared<Button>(
        "[ 返回 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

DelBookCheckScene::DelBookCheckScene(const Book& _book) : _book(_book) {}

void DelBookCheckScene::init() {
    this->add_item(make_shared<Text>("确定要删除此书吗?", 3, 1));

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "ISBN号: %s", _book._isbn.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 3));

    snprintf(buffer, sizeof(buffer), "书名: %s", _book._name.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 4));

    snprintf(buffer, sizeof(buffer), "作者: %s", _book._author.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 5));

    snprintf(buffer, sizeof(buffer), "出版社: %s", _book._publisher.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 6));

    this->add_item(
        make_shared<Button>("[ 确定 ]", 3, 12, [this](shared_ptr<Button> _btn) {
            ui().pop(0);
            auto sc = make_shared<DelBookSaveScene>(_book._isbn);
            ui().push(sc);
        }));

    this->add_item(make_shared<Button>(
        "[ 取消 ]", 14, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

DelBookSaveScene::DelBookSaveScene(const string& str) : isbn_str(str) {}

void DelBookSaveScene::init() {
    this->add_item(make_shared<Text>("删除书籍中...", 3, 1));

    library().erase_book(isbn_str, [](shared_ptr<Log> log) {
        if (log->ok()) {
            ui().pop(NO);
            auto sc = make_shared<OkScene>("书籍删除成功！");
            ui().push(sc);
        } else {
            ui().pop(NO);
            auto sc = make_shared<OkScene>("书籍删除失败！");
            ui().push(sc);
        }
    });
}

void BackupDataScene::init() {
    this->add_item(make_shared<Text>("数据库数据备份", 3, 1));

    this->add_item(make_shared<Text>("请输入备份的目标文件", 3, 5));

    this->add_item(
        make_shared<Input>("文件名", 3, 7, 15,
                           [this](shared_ptr<Input> _input, const string& str) {
                               if (str.size() == 0) {
                                   return;
                               }
                               auto sc = make_shared<BackupDataSaveScene>(str);
                               ui().push(sc);
                           }));

    this->add_item(make_shared<Button>(
        "[ 返回 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

void BackupDataScene::notice(int x) {
    if (x == YES) {
        ui().pop(0);
    }
}

BackupDataSaveScene::BackupDataSaveScene(const string& _file) : _file(_file) {}

void BackupDataSaveScene::init() {
    this->add_item(make_shared<Text>("数据库数据备份中...", 3, 1));

    library().save_data(_file, [](shared_ptr<Log> log) {
        if (log->ok()) {
            ui().pop(YES);
            auto sc = make_shared<OkScene>("数据库备份成功！");
            ui().push(sc);
        } else {
            ui().pop(NO);
            auto sc = make_shared<OkScene>("数据库备份失败！");
            ui().push(sc);
        }
    });
}

void EditBookSearchISBNScene::init() {
    this->add_item(make_shared<Text>("修改书籍信息", 3, 1));

    this->add_item(make_shared<Text>("请输入要修改的书籍的ISBN号", 3, 5));

    this->add_item(make_shared<Input>(
        "ISBN号", 3, 7, 15,
        [this](shared_ptr<Input> _input, const string& str) {
            if (str.size() == 0) {
                return;
            }
            isbn_str = str;

            bool suc = true;
            for (auto ch : isbn_str) {
                if (!isnumber(ch)) {
                    suc = false;
                }
            }

            if (!suc) {
                auto sc = make_shared<OkScene>("输入的ISBN号格式不正确！");
                ui().push(sc);
            } else {
                library().find_book(
                    isbn_str, [](Book&& book, shared_ptr<Log> log) {
                        if (log->ok()) {
                            auto sc = make_shared<EditBookScene>(book);
                            ui().push(sc);
                        } else {
                            auto sc = make_shared<OkScene>("未找到对应书籍！");
                            ui().push(sc);
                        }
                    });
            }
        }));

    this->add_item(make_shared<Button>(
        "[ 返回 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

EditBookScene::EditBookScene(const Book& _book) : _book(_book), now_edit(0) {
    _isbn_old = _book._isbn;
}

void EditBookScene::init() {
    this->add_item(make_shared<Text>("修改书籍信息", 3, 1));

    this->add_item(make_shared<Text>("请选择并输入要修改的信息", 3, 5));
    this->add_item(
        make_shared<Button>("[ISBN号]", 3, 6, [this](shared_ptr<Button> _btn) {
            now_edit = 1;
            _input->label = "ISBN号";
        }));
    this->add_item(
        make_shared<Button>("[书名]", 11, 6, [this](shared_ptr<Button> _btn) {
            now_edit = 2;
            _input->label = "书名";
        }));
    this->add_item(
        make_shared<Button>("[作者]", 17, 6, [this](shared_ptr<Button> _btn) {
            now_edit = 3;
            _input->label = "作者";
        }));
    this->add_item(
        make_shared<Button>("[出版社]", 23, 6, [this](shared_ptr<Button> _btn) {
            now_edit = 4;
            _input->label = "出版社";
        }));

    _input = make_shared<Input>(
        "未选择", 3, 8, 15,
        [this](shared_ptr<Input> _input, const string& str) {
            if (now_edit == 0) {
                auto sc = make_shared<OkScene>("未选择修改项！");
                ui().push(sc);
                return;
            }
            if (str.size() == 0) {
                return;
            }
            switch (now_edit) {
                case 1: {
                    bool suc = true;
                    for (auto ch : str) {
                        if (!isnumber(ch)) {
                            suc = false;
                        }
                    }

                    if (!suc) {
                        auto sc =
                            make_shared<OkScene>("输入的ISBN号格式不正确！");
                        ui().push(sc);
                    } else {
                        _book._isbn = str;
                    }
                    this->add_item(make_shared<Text>("新!", 23, 8));
                } break;
                case 2: {
                    _book._name = str;
                    this->add_item(make_shared<Text>("新!", 23, 9));
                } break;
                case 3: {
                    _book._author = str;
                    this->add_item(make_shared<Text>("新!", 23, 10));
                } break;
                case 4: {
                    _book._publisher = str;
                    this->add_item(make_shared<Text>("新!", 23, 11));
                } break;
                default:
                    break;
            }
            update_show();
        });
    this->add_item(_input);

    _t_isbn = make_shared<Text>("", 26, 8);
    _t_name = make_shared<Text>("", 26, 9);
    _t_author = make_shared<Text>("", 26, 10);
    _t_publisher = make_shared<Text>("", 26, 11);
    this->add_item(_t_isbn);
    this->add_item(_t_name);
    this->add_item(_t_author);
    this->add_item(_t_publisher);
    update_show();

    this->add_item(
        make_shared<Button>("[ 保存 ]", 3, 17, [this](shared_ptr<Button> _btn) {
            ui().pop(0);
            auto sc = make_shared<EditBookSaveScene>(_isbn_old, _book);
            ui().push(sc);
        }));

    this->add_item(make_shared<Button>(
        "[ 取消 ]", 12, 17, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

void EditBookScene::update_show() {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "ISBN号: %s", _book._isbn.c_str());
    _t_isbn->label = buffer;

    snprintf(buffer, sizeof(buffer), "书名: %s", _book._name.c_str());
    _t_name->label = buffer;

    snprintf(buffer, sizeof(buffer), "作者: %s", _book._author.c_str());
    _t_author->label = buffer;

    snprintf(buffer, sizeof(buffer), "出版社: %s", _book._publisher.c_str());
    _t_publisher->label = buffer;
}

EditBookSaveScene::EditBookSaveScene(const string& old_isbn, const Book& book)
    : _old_isbn(old_isbn), _book(book) {}

void EditBookSaveScene::init() {
    this->add_item(make_shared<Text>("正在应用书籍修改...", 3, 1));

    if (_book._isbn == _old_isbn) {
        library().update_book(_book._isbn, _book, [](shared_ptr<Log> log) {
            if (log->ok()) {
                ui().pop(0);
                auto sc = make_shared<OkScene>("书籍信息修改成功！");
                ui().push(sc);
            } else {
                ui().pop(0);
                auto sc = make_shared<OkScene>("书籍信息修改失败！");
                ui().push(sc);
            }
        });
    } else {
        library().find_book(_book._isbn, [this](Book&&, shared_ptr<Log> log) {
            if (log->ok()) {
                ui().pop(0);
                auto sc = make_shared<OkScene>(
                    "书籍信息修改失败！新的ISBN已存在, 需解决冲突.");
                ui().push(sc);
            } else {
                library().erase_book(_old_isbn, [this](shared_ptr<Log> log) {
                    if (!log->ok()) {
                        ui().pop(0);
                        auto sc = make_shared<OkScene>(
                            "书籍信息修改失败, 删除原ISBN对应书籍出错！");
                        ui().push(sc);
                    } else {
                        library().insert_book(_book, [](shared_ptr<Log> log) {
                            if (log->ok()) {
                                ui().pop(0);
                                auto sc =
                                    make_shared<OkScene>("修改书籍信息成功！");
                                ui().push(sc);
                            } else {
                                ui().pop(0);
                                auto sc = make_shared<OkScene>(
                                    "书籍信息修改失败, "
                                    "插入新ISBN对应书籍出错！");
                                ui().push(sc);
                            }
                        });
                    }
                });
            }
        });
    }
}

void LibraryInfoScene::init() {
    this->add_item(make_shared<Text>("图书数据库基本信息", 3, 1));

    this->add_item(make_shared<Text>("系统状态: 正常", 3, 5));
    this->add_item(make_shared<Text>("藏书数量: 97541册", 3, 6));
    this->add_item(make_shared<Text>("借出数量: 351册", 3, 7));

    this->add_item(make_shared<Button>(
        "[ 返回 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

void SearchBookISBNScene::init() {
    this->add_item(make_shared<Text>("查询书籍", 3, 1));

    this->add_item(make_shared<Text>("请输入要查询的书籍的ISBN号", 3, 5));

    this->add_item(make_shared<Input>(
        "ISBN号", 3, 7, 15,
        [this](shared_ptr<Input> _input, const string& str) {
            isbn_str = str;

            bool suc = true;
            for (auto ch : isbn_str) {
                if (!isnumber(ch)) {
                    suc = false;
                }
            }

            if (!suc) {
                auto sc = make_shared<OkScene>("输入的ISBN号格式不正确！");
                ui().push(sc);
            } else {
                library().find_book(
                    isbn_str, [](Book&& book, shared_ptr<Log> log) {
                        if (log->ok()) {
                            auto sc = make_shared<BookInfoScene>(book);
                            ui().push(sc);
                        } else {
                            auto sc = make_shared<OkScene>("未找到对应书籍！");
                            ui().push(sc);
                        }
                    });
            }
        }));

    this->add_item(make_shared<Button>(
        "[ 返回 ]", 3, 12, [](shared_ptr<Button> _btn) { ui().pop(0); }));
}

BookInfoScene::BookInfoScene(const Book& _book) : _book(_book) {}

void BookInfoScene::init() {
    this->add_item(make_shared<Text>("书籍信息", 3, 1));

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "ISBN号: %s", _book._isbn.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 3));

    snprintf(buffer, sizeof(buffer), "书名: %s", _book._name.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 4));

    snprintf(buffer, sizeof(buffer), "作者: %s", _book._author.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 5));

    snprintf(buffer, sizeof(buffer), "出版社: %s", _book._publisher.c_str());
    this->add_item(make_shared<Text>(buffer, 3, 6));

    this->add_item(
        make_shared<Text>("借阅状态: 借出 2024.12.23借出 (借阅35天)", 3, 8));

    this->add_item(make_shared<Button>(
        "[ 返回 ]", 3, 11, [](shared_ptr<Button> _btn) { ui().pop(0); }));
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