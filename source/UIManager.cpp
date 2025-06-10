#include"../header/UIManager.h"

//生命周期由UIContent控制。
UIBehavior_Operation* UIManager::Create(ComponentAdder* content, const std::string& uitype, const UIParams& params)
{
    UICreateable* ui = CreateUI(uitype, params);
    if (!ui) {
        return ui;
    }
    content->Add(ui, params);
    return ui;
}
std::string UIManager::CreateLogic(ComponentAdder* content,const std::string& uitype, LogicParams* params)
{
    ScriptBehavior* logic = CreateLogic(uitype, params);
    bool yes=content->AddLogic(logic);
    if (yes)
        return params->name;
    return "";
}
LogicParams* UIManager::CreateLogicParam(const std::string& uitype)
{
    auto& creators = GetLogicParamCreatorMap();
    auto it = creators.find(uitype);
    if (it == creators.end()) {
        throw std::runtime_error("Logic params not registered: " + uitype);
    }
    return it->second();
}
UICreateable* UIManager::CreateUI(const std::string& name, const UIParams& params) {
    auto& creators = GetCreatorMap();
    auto it = creators.find(name);
    if (it == creators.end()) {
        throw std::runtime_error("UI type not registered: " + name);
    }
    return it->second(params);
}
ScriptBehavior* UIManager::CreateLogic(const std::string& name,LogicParams* params) {
    auto& creators = GetLogicCreatorMap();
    auto it = creators.find(name);
    if (it == creators.end()) {
        throw std::runtime_error("Logic type not registered: " + name);
    }
    return it->second(params);
}
