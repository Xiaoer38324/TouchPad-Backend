#pragma once
#pragma once
#include<cmath>
#include <cassert>
#include<algorithm>
#include <type_traits>
#include"Box.h"
#include<iostream>
using namespace quadtree;
template<typename T, typename GetBox>
class PriorityIterate {
    static_assert(std::is_convertible_v<std::invoke_result_t<GetBox, const T&>, Box<float>*>,
        "GetBox must be a callable of signature Box<Float>(const T&)");
private:
    struct TWithBox{
    public:
        T t;
        Box<float>* box;
        TWithBox(const T& t, Box<float>* box)
            : t(t), box(box)
        {

        }
    };
    std::vector<TWithBox> value;
    ScreenINT width, height;
    GetBox mGetBox;
public:
    PriorityIterate(const GetBox& getBox, ScreenINT w, ScreenINT h) : mGetBox(getBox), width(w), height(h) {
       
    }
    void add(T t) {
        Box<float>* box = mGetBox(t);
        value.push_back(TWithBox(t,box));
    }
    //倒序搜索，越在后面的实际上是渲染越晚的，越靠前的。
    bool query(POINT point, T& result) {
        for (int i = value.size()-1;i >=0 ;i--) {
            TWithBox tb = value.at(i);
            if (tb.box->contains(point.x,point.y)&&tb.box->mask->IsHitable(point.x, point.y)) {
                result = tb.t;
                return true;
            }
        }
        return false;
    }
};
