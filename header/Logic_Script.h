#pragma once
#include"UIOperation.h"
#include"UIContent.h"
#include "Jsonable.h"
class LogicParams:public Jsonable {
public:
    virtual ~LogicParams() = default;
    std::string name;
    std::string type_name;
    unsigned short priority=0;//优先度。越小越在下面，越晚接收点击事件，渲染越早。
    
    LogicParams(const std::string& name,const std::string type_name)
        : name(name),type_name(type_name)
    {
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
            {"Priority",priority},
            {"Name",name },
            {"Type",type_name}
        };
        return j;
    }
    bool Deserialization(nlohmann::json j) override {
        try {
            name = j["Name"].get<std::string>();
            type_name = j["Type"].get<std::string>();
            priority = j["Priority"].get<unsigned short>();
        }
        catch (...) {
            return false;
        }
        return true;
    }
};
class ScriptBehavior :public UISize,public Jsonable {//Jsonable一般是在子类继承，但是为了方便上下文管理，给ScriptBehavior里面了。
protected:
    std::string name;//脚本UI。就是带逻辑处理的UI，可以组合基础UI使用。也就是用户能见到的UI。这里直接混用。
    //因为不继承双父
public:
    virtual void Enter() = 0;
    virtual void Leave() = 0;
    virtual void Update() = 0;//每一帧的更新
    virtual bool Start(ComponentAdder*) = 0;//初始化代码
    virtual void Destory() = 0;//清楚代码
    virtual ~ScriptBehavior() = default;
    ScriptBehavior(const std::string& name)
        : name(name)
    {
    }
};



