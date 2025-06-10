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
struct UIRenderInfo {//��Ⱦ����Ҫ�����ݡ�����
    Box<float> bounds;
    UIRenderInfo() = default;
    UIRenderInfo(const UIRenderInfo&) = delete;
    UIRenderInfo& operator=(const UIRenderInfo&) = delete;
    ~UIRenderInfo() = default;
    friend class DataBase;
};
/// <summary>
///ʵ���ϲ��������������ϵĳء��Ǽٵģ��������ݹ�������Ϊָ�롣�����õ�ʱ�������Ϊָ����
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
    UIINT master = MAXUINUMBER;//downʱ�Ŀؼ�id
    UIINT temp_master = MAXUINUMBER;//pressʱ��ʱ���ڵ�id��
};
struct UIUpdateInfo {//�û�����������Ҫ�Ļ�������
    bool enable_render = true;//�Ƿ���Ⱦ,�⽫��رյ���Ⱦ����Ϣ���ܵĲ���������update��Ȼ����á�
    DataPool<InputMeta*>* points;
};


