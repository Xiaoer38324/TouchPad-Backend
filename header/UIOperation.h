#pragma once
#include "VTP_Type.h"
#include <imgui.h>
class UIOperation {
public:
    virtual void ReSize(UIINT, ImVec2)=0;
    virtual void Move(UIINT, ImVec2) = 0;
    virtual ImVec2 GetPosition(UIINT)=0;
    virtual ImVec2 GetSize(UIINT) = 0;
    virtual ImVec4 GetTransform(UIINT) = 0;
};
