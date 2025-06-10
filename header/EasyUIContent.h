#pragma once
#include "UIContent.h"
#include "DataBase.h"
#include <imgui.h>
#include "../source/Process_Event.cpp"
#include "Logic_Script.h"
class PlayingUIContent :public ComponentAdder,public UIContent,public Jsonable//游玩时用的上下文组件。编辑时是另一个。
{
    DataBase db;
    //点位数据由输入系统的对象池管理
    std::vector<DataPool<InputMeta*>> message;
    Normal_Process pe;
    InputSystem* input;
    std::vector<ScriptBehavior*> scripts;
    UIINT index=0;
    bool register_by_deser = false;
public:
     void Init() override;//初始化
     void Enter() override;//进行上下文
     void Quit()override;//退出上下文
     void Update()override;//循环上下文
     void Leave()override;//离开上下文
     nlohmann::json Serialization()override;//序列化
     bool Deserialization(nlohmann::json value)override;//反序列化
     UIINT Add(UICreateable* ui, const UIParams& params)override;//添加组件
     bool Remove(UIINT ui) override;//删除组件
     bool AddLogic(ScriptBehavior*)override;
     bool RemoveLogic(std::string ui)override;//删除一个逻辑脚本。
    PlayingUIContent(InputInstance* input_ins) {
        ImVec2 window_size = WindowOperation::getInstance().GetSize();
        input=new InputSystem(input_ins, window_size.x, window_size.y);
        WindowOperation::getInstance().AddUISize(&db);
        WindowOperation::getInstance().AddUISize(input);
    };
    ~PlayingUIContent()
    {
        delete input;
        for (auto v : scripts) {
            delete v;
        }
        
    }
};

