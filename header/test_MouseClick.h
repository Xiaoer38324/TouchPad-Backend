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
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) { // 左键当前被按下
            if (!isLeftButtonPressed){
                isLeftButtonPressed = true;
                value.at(0).type = TouchDown;
            }
            else {
                value.at(0).type = TouchPress;
            }
            // 获取鼠标位置
            if (GetCursorPos(&temp)) {
                value.at(0).points.x = temp.x;
                value.at(0).points.y = temp.y;
                number = 1;
                return true;
            }
        }
        else { // 左键当前未被按下
            if (isLeftButtonPressed) {
                isLeftButtonPressed = false;
                value.at(0).type = TouchUp;
                // 获取鼠标位置
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
