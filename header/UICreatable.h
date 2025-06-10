#pragma once
#include"UIBehavior.h"
#include "UIBehavior_Operation.h"
class UICreateable :public UIBehavior, public UISize,public UIBehavior_Operation,public InputMask {
public:
    /// <summary>
    /// 不要擅修改该属性。通过UIManager统一操作,不然可能出现渲染和判断不一致的情况！。
    /// </summary>
    /// <returns></returns>
    UIRenderInfo* GetInfo()//这个操作并不安全，意味着其他人也可以访问和修改渲染信息。
    {
        return info;
    }
    bool SetInfo(UIRenderInfo* info,const UIINT& name, UIOperation* operation)//这个操作并不安全，意味着其他人也可以访问和修改渲染信息,但是这是为了延后。我们先获取实例，然后初始化数据。
    {
        if (this->info != nullptr||info ==nullptr)
            return false;
        this->info = info;
        this->name = name;
        this->operation = operation;
        return true;
    }
protected:
    UIRenderInfo* info=nullptr;
    UICreateable(UIINT name) {//提供的UI信息
        this->name = name;
    }
};

