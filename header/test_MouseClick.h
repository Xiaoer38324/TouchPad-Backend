#pragma once
#include <windows.h>
#include"InputSystem.h"
class testMouseClick :public InputInstance {
    bool isLeftButtonPressed = false;
    POINT temp;
    bool GetPos(std::vector<InputMeta>& value, int& number) override
    {
        if (value.size() < 1) {
            value.push_back(InputMeta());
            value.at(0).index = 0;
        }
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { // �����ǰ������
            if (!isLeftButtonPressed){
                isLeftButtonPressed = true;
                value.at(0).type = TouchDown;
            }
            else {
                value.at(0).type = TouchPress;
            }
            // ��ȡ���λ��
            if (GetCursorPos(&temp)) {
                value.at(0).points.x = temp.x;
                value.at(0).points.y = temp.y;
                number = 1;
                return true;
            }
        }
        else { // �����ǰδ������
            if (isLeftButtonPressed) {
                isLeftButtonPressed = false;
                value.at(0).type = TouchUp;
                // ��ȡ���λ��
                if (GetCursorPos(&temp)) {
                    value.at(0).points.x = temp.x;
                    value.at(0).points.y = temp.y;
                    number = 1;
                    return true;
                }
            }
        }
        number = 0;
        return false;
    }
};
