#pragma once
#include "InputSystem.h"
#include "Jsonable.h"
class UIParams:public Jsonable {
public:
    virtual ~UIParams() = default;
    float x, y, w, h;
    UIParams(float x, float y, float w, float h)
        : x(x), y(y), w(w), h(h)
    {

    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
            { "Transform",nlohmann::json::array({x,y,w,h})}
        };
        return j;
    }
    bool Deserialization(nlohmann::json value) override {
        try {
            std::vector<float> trans = value["Transform"].get<std::vector<float>>();
            x = trans[0];
            y = trans[1];
            w = trans[2];
            h = trans[3];
        }
        catch (...) {
            return false;
        }


    };
    
};
class UIBehavior {
    //自定义组件的抽象类

public:
   virtual void Update(const UIUpdateInfo&)=0;//每一帧的更新
   virtual void Render() = 0;//渲染代码
   virtual void Start() = 0;//初始化代码
   virtual void Destory() = 0;//清楚代码
   virtual ~UIBehavior() = default;
};

