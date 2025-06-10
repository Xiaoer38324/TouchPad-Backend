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
    //�Զ�������ĳ�����

public:
   virtual void Update(const UIUpdateInfo&)=0;//ÿһ֡�ĸ���
   virtual void Render() = 0;//��Ⱦ����
   virtual void Start() = 0;//��ʼ������
   virtual void Destory() = 0;//�������
   virtual ~UIBehavior() = default;
};

