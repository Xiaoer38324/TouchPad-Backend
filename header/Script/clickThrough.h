#pragma once
#include"../header/Logic_Script.h"
#include"../UI/Button_Empty.h"
class clickThroughEvent;
class clickThrough :public ScriptBehavior//逻辑脚本的模板。
{
public:
    //这个是用于导入时的数据结构，你也可以当成实际数据存储结构使用。
    struct Params : public LogicParams {
    public:
        Button_Empty::Params params;//这仅仅是一个案例ButtonWithMask是基础组件
        Params(Button_Empty::Params params)
            :params(params), LogicParams(name, "clickThrough")
        {

        }
        Params() : LogicParams("unknow", "clickThrough") {

        }
        nlohmann::json Serialization() override {//序列化数据，在这里你可以保存或转换你这个逻辑组件所需要的数据。
            //std::string baby = params.Serialization();//组件也是实现了序列化的。
            nlohmann::json j = LogicParams::Serialization();
            j["Button"] = params.Serialization();
            return j;
        }
        bool Deserialization(nlohmann::json j) override {//反序列化数据。
            
            try {
                if (LogicParams::Deserialization(j) == false)
                    return false;
                return params.Deserialization(j["Button"].get<nlohmann::json>());  
            }
            catch (...) {
                return false;
            }
            return true;
        }
    };
    //一定要实现，如果你需要通过UIManager生成
    //这个方法是用于配合UIManager工厂来自动化生成逻辑组件实例的。
    static ScriptBehavior* Create(LogicParams* params) {

        Params* p = dynamic_cast<Params*>(params);
        return new clickThrough(p);
    }
    //一定要实现，如果你需要通过UIManager生成
    //这个方法是用于配合UIManager工厂来自动化生成逻辑组件参数实例的。
    static LogicParams* GetParam() {

        return new Params();
    }
    //构造函数，参数可以自定义，只需要在Crate方法里面能提供即可。
    clickThrough(Params* params) :ScriptBehavior(params->name)
    {
         this->mydata = params;
    }
private:
    ImVec2 size;
    //ButtonWithMask* mybaby = nullptr;//这个逻辑组件包含的基础组件
    bool Start(ComponentAdder* euc) override;//初始化时触发一次，可以在这里初始化数据，添加基础组件。
    void Destory();//销毁时触发
    void Enter() override;//每次切换会该逻辑组件的上下文时触发
    void Leave() override;//每次退出该逻辑组件上下文时触发
    virtual nlohmann::json Serialization() override;//序列化数据，提供给上下文类保存时用。一般是序列化Param。可以自行添加额外逻辑
    virtual bool Deserialization(nlohmann::json value) override;//反序列化，不需要写。反序列化是在Param内部。该函数无用。
    Button_Empty* empty;
    clickThroughEvent* xxx;
    Params* mydata;//Param数据，可以选择直接使用保存数据当成该逻辑组件数据，也可以不用。只要在保存的时候能转换就行。
    void Update() override;//每一帧触发。
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height) override;
};


