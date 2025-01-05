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
    virtual void init() override;
    virtual void notice(int) override;

   private:
    std::string data_file;
    int step;
};

class LoadDataScene : public Scene {
   public:
    LoadDataScene(const std::string& str);
    virtual void init() override;

   private:
    std::string data_file;
};

class LobbyScene : public Scene {
   public:
    virtual void init() override;
};

class ConfirmScene : public Scene {
   public:
    ConfirmScene(const std::string& str);
    virtual void init() override;

   private:
    std::string title;
};

class OkScene : public Scene {
   public:
    OkScene(const std::string& str);
    virtual void init() override;

   private:
    std::string title;
};

#endif