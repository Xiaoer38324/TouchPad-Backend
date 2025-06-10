#pragma once
#include"UICreatable.h"
class ScriptBehavior;
class UIContent {
    //UI上下文，实际上就是一个UI界面。我们可以有很多个UI界面。可以切换使用。
    //这是一个抽象类，主要是负责上下文的事件。
public:
    virtual void Init() = 0;//初始化
    virtual void Enter() = 0;//进行上下文
    virtual void Update() = 0;//上下文循环
    virtual void Leave() = 0;//离开上下文
    virtual void Quit()=0;//退出上下文，一般意味着销毁或卸载。
    
};
class ComponentAdder {
    //有添加逻辑组件和基础组件事件的接口。
public:
    virtual UIINT Add(UICreateable* ui, const UIParams& params) = 0;//添加一个UIBehavior。
    virtual bool Remove(UIINT) = 0;//删除一个UIBehavior。
    virtual bool AddLogic(ScriptBehavior*)=0;
    virtual bool RemoveLogic(std::string ui) = 0;//删除一个逻辑脚本。
};
