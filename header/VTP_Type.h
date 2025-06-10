#pragma once
#define UIINT unsigned short
#define ScreenINT unsigned int
#define TouchINT unsigned int
#define TouchTypeNumber unsigned char
#define TouchUnknow 0
#define TouchDown 1
#define TouchUp 2
#define TouchPress 3
#define TouchEnter 4
#define TouchLeave 5
#define TouchUseless 6
#define TouchClick 7
#define MAXUINUMBER 65535
#include"Box.h"
#include<vector>
#include <memory>
#include <functional>
using namespace quadtree;
struct TouchPOINT
{
public:
    int x;
    int y;
    TouchPOINT(int x, int y)
        : x(x), y(y)
    {
    }
    TouchPOINT():x(0),y(0) {

    }
};
struct UIRenderInfo {//渲染所需要的数据。待定
    Box<float> bounds;
    UIRenderInfo() = default;
    UIRenderInfo(const UIRenderInfo&) = delete;
    UIRenderInfo& operator=(const UIRenderInfo&) = delete;
    ~UIRenderInfo() = default;
    friend class DataBase;
};
/// <summary>
///实际上并不是真正意义上的池。是假的，不做数据管理，仅仅为指针。我们用的时候仅仅作为指引。
/// </summary>
/// <typeparam name="T"></typeparam>
template<class T>
struct DataPool {
    int usable = 0;
    std::vector<T> data;
};
class InputSystem;
class InputMeta{
public:
    friend InputSystem;
    unsigned char index = MAXUINUMBER;
    TouchPOINT points;
    TouchTypeNumber type=0;
    void CopyFrom(const InputMeta* other) {
        if (this != other) {
            index = other->index;
            points = other->points;
            type = other->type;
            master = other->master;
            temp_master = other->temp_master;
        }
    }
private:
    UIINT master = MAXUINUMBER;//down时的控件id
    UIINT temp_master = MAXUINUMBER;//press时临时所在的id。
};
struct UIUpdateInfo {//用户交互的所需要的基础数据
    bool enable_render = true;//是否渲染,这将会关闭掉渲染和信息接受的操作，但是update依然会调用。
    DataPool<InputMeta*>* points;
};


