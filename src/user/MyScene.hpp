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
    virtual void init() override;
};

#endif