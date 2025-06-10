#include "../header/UIBehavior_Operation.h"

void UIBehavior_Operation::SetPosition(ImVec2 pos)
{
    operation->Move(name, pos);
}

void UIBehavior_Operation::SetSize(ImVec2 size)
{
    operation->ReSize(name, size);
}

ImVec2 UIBehavior_Operation::GetPosition()
{
    return operation->GetPosition(name);
}

ImVec2 UIBehavior_Operation::GetSize()
{
    return operation->GetSize(name);
}

ImVec4 UIBehavior_Operation::GetTransform()
{
    return operation->GetTransform(name);
}
