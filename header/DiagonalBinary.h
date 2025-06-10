#pragma once
#include<cmath>
#include <cassert>
#include<algorithm>
#include <type_traits>
#include"Box.h"
#include<iostream>
using namespace quadtree;
template<typename T, typename GetBox>
class Binary_Diagonal {
    static_assert(std::is_convertible_v<std::invoke_result_t<GetBox, const T&>, Box<float>>,
        "GetBox must be a callable of signature Box<Float>(const T&)");
private:
    struct TwithPos {
        T t;
        float pos;
    };
    float diagonal_length = 0;
    float diagonal_normal= sqrt(2)/2;
    ScreenINT width, height;
    std::vector<TwithPos> diagonal;
    GetBox mGetBox;
    float ToDiagonalPos(float x,float y) {
        return  x * diagonal_normal + y * diagonal_normal;
    }
public:
    Binary_Diagonal(const GetBox& getBox, ScreenINT w, ScreenINT h): mGetBox(getBox),width(w),height(h) {
        diagonal_length = sqrt(w * w + h * h);
    }
    void add(T t) {
        Box<float> box = mGetBox(t);
        TwithPos tp{
        };
        tp.t = t;
        tp.pos = ToDiagonalPos(box.left, box.top);
        diagonal.push_back(tp);
        std::sort(diagonal.begin(), diagonal.end(), [](const TwithPos& a, const TwithPos& b) { return a.pos < b.pos; });
    }
    void FlashPos() {
        for (TwithPos& tp : diagonal) {
            Box<float> box = mGetBox(tp.t);
            tp.pos = ToDiagonalPos(box.left, box.top);
        }
    }
    bool query(POINT point,T& result) {
        float pos = ToDiagonalPos(point.x, point.y);
        auto left = std::lower_bound(diagonal.begin(), diagonal.end(), pos,
            [](const TwithPos& r, float val) { return r.pos < val; })- diagonal.begin()-1;
        for (;left>=0; --left){
            Box<float> box = mGetBox(diagonal.at(left).t);
            if (box.contains(point.x, point.y)) {
                result= diagonal.at(left).t;
                return true;
            }
        }
        
        return false;
    }
};
