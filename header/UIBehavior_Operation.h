#pragma once
#include "VTP_Type.h"
#include "UIOperation.h"
class UIBehavior_Operation {
    //UI的对外的控制接口
protected:
    UIINT name=UINT32_MAX;
    UIOperation* operation;//在内核中的具体操作
public:
    void SetPosition(ImVec2 pos);
    void SetSize(ImVec2 size);
    virtual ImVec2 GetPosition();
    virtual ImVec2 GetSize();
    virtual ImVec4 GetTransform();
    virtual ~UIBehavior_Operation() {};
};

