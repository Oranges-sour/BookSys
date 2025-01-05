#ifndef __MY_SCENE_H__
#define __MY_SCENE_H__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <random>
#include <stack>
#include <thread>

#include "../base/Library.hpp"
#include "../base/UI.hpp"

class MyScene : public Scene {
   public:
    MyScene();
    void init() override;
    void notice(int) override;

   private:
    std::string data_file;
    int step;
};

class LoadDataScene : public Scene {
   public:
    LoadDataScene(const std::string& str);
    void init() override;

   private:
    std::string data_file;
};

class LobbyScene : public Scene {
   public:
    void init() override;
};

class BackupDataScene : public Scene {
   public:
    void init() override;
    void notice(int) override;
};

class BackupDataSaveScene : public Scene {
   public:
    BackupDataSaveScene(const std::string& _file);
    void init() override;

   private:
    std::string _file;
};

class LibraryInfoScene : public Scene {
   public:
    void init() override;
};

class EditBookSearchISBNScene : public Scene {
   public:
    void init() override;

   private:
    std::string isbn_str;
};

class EditBookScene : public Scene {
   public:
    EditBookScene(const Book& _book);

    void init() override;

   private:
    std::shared_ptr<Input> _input;

    void update_show();

    std::shared_ptr<Text> _t_isbn;
    std::shared_ptr<Text> _t_name;
    std::shared_ptr<Text> _t_author;
    std::shared_ptr<Text> _t_publisher;

    int now_edit;

    std::string _isbn_old;
    Book _book;
};

class EditBookSaveScene : public Scene {
   public:
    EditBookSaveScene(const std::string& old_isbn, const Book& _book);

    void init() override;

   private:
    std::string _old_isbn;
    Book _book;
};

class BookInfoScene : public Scene {
   public:
    BookInfoScene(const Book& _book);

    virtual void init() override;

   private:
    Book _book;
};

class SearchBookISBNScene : public Scene {
   public:
    void init() override;

   private:
    std::string isbn_str;
};

class ConfirmScene : public Scene {
   public:
    ConfirmScene(const std::string& str);
    void init() override;

   private:
    std::string title;
};

class OkScene : public Scene {
   public:
    OkScene(const std::string& str);
    void init() override;

   private:
    std::string title;
};

#endif