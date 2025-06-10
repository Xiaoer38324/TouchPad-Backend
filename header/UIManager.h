#pragma once
#include <imgui.h>
#include "UICreatable.h"
#include "UIContent.h"
#include "Logic_Script.h"
static class UIManager {
    //这个类直接管理UI上下文的功能，包括实例获取以及上下文切换。
    UIManager() = delete;
    UIManager(const UIManager&) = delete;
    void operator=(const UIManager&) = delete;
public:
    //基础组件的生成逻辑
    /*using UICreator = std::function<UICreateable* (const UIParams&)>;*/
    template <typename T>
    static void RegisterUI(const std::string& name) {
        auto& m_creators = GetCreatorMap();
        m_creators[name] = &T::Create;
    }
    static  UIBehavior_Operation* Create(ComponentAdder* content,const std::string& uitype, const UIParams& params);
    //逻辑组件的生成逻辑
    //using LogicCreator = std::function<ScriptBehavior* (const LogicParams&)>;
    template <typename T>
    static void RegisterLogic(const std::string& name) {
        auto& m_creators = GetLogicCreatorMap();
        m_creators[name] = &T::Create;
        auto& m_logiccreators = GetLogicParamCreatorMap();
        m_logiccreators[name] = &T::GetParam;
    }//注册逻辑组件
    static  std::string  CreateLogic(ComponentAdder* content,const std::string& uitype, LogicParams* params);//创建逻辑组件
    static  LogicParams*  CreateLogicParam(const std::string& uitype);//创建逻辑组件
private:
    static std::unordered_map<std::string, UICreateable* (*)(const UIParams&)>& GetCreatorMap() {
        static std::unordered_map<std::string, UICreateable* (*)(const UIParams&)> creators;
        return creators;
    }
    static std::unordered_map<std::string, LogicParams* (*)()>& GetLogicParamCreatorMap() {
        static std::unordered_map<std::string, LogicParams* (*)()> creators;
        return creators;
    }
    static std::unordered_map<std::string, ScriptBehavior* (*)( LogicParams*)>& GetLogicCreatorMap() {
        static std::unordered_map<std::string, ScriptBehavior* (*)(LogicParams*)> creators;
        return creators;
    }
    static UICreateable* CreateUI(const std::string& name, const UIParams& params);
    static ScriptBehavior* CreateLogic(const std::string& name,  LogicParams* params);
};
