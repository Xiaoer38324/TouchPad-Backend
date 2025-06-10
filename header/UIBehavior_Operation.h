#pragma once
#include "VTP_Type.h"
#include "UIOperation.h"
class UIBehavior_Operation {
    //UI�Ķ���Ŀ��ƽӿ�
protected:
    UIINT name=UINT32_MAX;
    UIOperation* operation;//���ں��еľ������
public:
    void SetPosition(ImVec2 pos);
    void SetSize(ImVec2 size);
    virtual ImVec2 GetPosition();
    virtual ImVec2 GetSize();
    virtual ImVec4 GetTransform();
    virtual ~UIBehavior_Operation() {};
};

