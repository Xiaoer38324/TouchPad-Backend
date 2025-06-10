#include "../header/WindowOperation.h"
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include <imgui.h>
#include <iostream>
#include "../header/Event_Interpreter.h"
#pragma comment (lib, "dwmapi.lib")
inline void SetWindowAsToolWindow(HWND hwnd) {
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
}
inline void SetWindowNoActivate(HWND hwnd) {
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
}
inline void SetWindowTopMost(HWND hwnd) {
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
inline HWND GetWindowHandleByTitle(const std::wstring& title) {
    return FindWindow(NULL, title.c_str());
}
// 获取窗口的位置和大小
inline RECT GetWindowRect(HWND hwnd) {
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return rect;
}
bool WindowOperation::SetTargetWindow(const std::wstring windowName)
{
    target = GetWindowHandleByTitle(windowName);
    if (!target) {
        return false;
    }
    
    return true;
}
bool GetTargetWindowRect(HWND target, RECT& outRect) {
    if (!IsWindow(target)) return false;

    // 尝试获取窗口矩形（即使最小化）
    if (!GetWindowRect(target, &outRect)) {
        return false;
    }

    // 检查是否最小化（此时坐标无效）
    if (IsIconic(target)) {
        // 临时恢复窗口（不抢焦点）
        WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
        GetWindowPlacement(target, &wp);

        if (wp.showCmd == SW_SHOWMINIMIZED) {
            // 保存当前状态后临时取消最小化
            wp.showCmd = SW_RESTORE;
            SetWindowPlacement(target, &wp);

            // 重新获取坐标
            GetWindowRect(target, &outRect);

            //// 恢复最小化状态（可选）
            //wp.showCmd = SW_SHOWMINIMIZED;
            //SetWindowPlacement(target, &wp);
        }
    }

    // 处理多显示器 DPI 虚拟化问题
    HMONITOR monitor = MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(monitor, &monitorInfo);

    // 确保坐标在显示器范围内
    outRect.left = std::max(outRect.left, monitorInfo.rcMonitor.left);
    outRect.top = std::max(outRect.top, monitorInfo.rcMonitor.top);
    outRect.right = std::min(outRect.right, monitorInfo.rcMonitor.right);
    outRect.bottom = std::min(outRect.bottom, monitorInfo.rcMonitor.bottom);

    return true;
}
bool GetTargetClientRect(HWND target, RECT& outRect) {
    if (!IsWindow(target)) return false;

    // 尝试获取窗口矩形（即使最小化）
    if (!GetClientRect(target, &outRect)) {
        return false;
    }

    // 检查是否最小化（此时坐标无效）
    if (IsIconic(target)) {
        // 临时恢复窗口（不抢焦点）
        WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
        GetWindowPlacement(target, &wp);

        if (wp.showCmd == SW_SHOWMINIMIZED) {
            // 保存当前状态后临时取消最小化
            wp.showCmd = SW_RESTORE;
            SetWindowPlacement(target, &wp);

            // 重新获取坐标
            GetClientRect(target, &outRect);

            //// 恢复最小化状态（可选）
            //wp.showCmd = SW_SHOWMINIMIZED;
            //SetWindowPlacement(target, &wp);
        }
    }

    // 处理多显示器 DPI 虚拟化问题
    HMONITOR monitor = MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(monitor, &monitorInfo);

    // 确保坐标在显示器范围内
    outRect.left = std::max(outRect.left, monitorInfo.rcMonitor.left);
    outRect.top = std::max(outRect.top, monitorInfo.rcMonitor.top);
    outRect.right = std::min(outRect.right, monitorInfo.rcMonitor.right);
    outRect.bottom = std::min(outRect.bottom, monitorInfo.rcMonitor.bottom);

    return true;
}
bool WindowOperation::AilgnWindow()
{
    try {
        if (target) {
            RECT targetRect;
            if (!GetTargetWindowRect(target, targetRect)) {
                return false;
            }
            int x = targetRect.left;
            int y = targetRect.top;
            int width = targetRect.right - targetRect.left;
            int height = targetRect.bottom - targetRect.top;
            
            return SetWindowPositionAndSize( x, y, width, height);;
        }
    }
    catch (...) {
        return false;
    }
}
bool WindowOperation::SetCoveredWindow() const
{
    try {
        if (target) {
            DWM_BLURBEHIND bb = { 0 };
            HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，
            //参数(0,0,-1,-1)可以让整个窗口客户区变成透明的，而鼠标是可以捕获到透明的区域
            bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
            bb.hRgnBlur = hRgn;
            bb.fEnable = TRUE;
            SetWindowTopMost(GetActiveWindow());
            DwmEnableBlurBehindWindow(GetActiveWindow(), &bb);
            SetWindowAsToolWindow(GetActiveWindow());
            SetWindowNoActivate(GetActiveWindow());
            return true;
        }
    }
    catch (...) {
        return false;
    }
    
}
//Not Use Try Catch,for busy updating.
bool WindowOperation::LeftClickWindow(const POINT& clientCoords, unsigned char tt) const
{
    DWORD err = 0;
    if (target) {
        POINT tartegt_pos= clientCoords;
        Client2Target(tartegt_pos);
        switch (tt) {
        case TouchDown:
            PostMessage(target, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(tartegt_pos.x, tartegt_pos.y));
            break;
        case TouchUp:
            PostMessage(target, WM_LBUTTONUP, 0, MAKELPARAM(tartegt_pos.x, tartegt_pos.y));
            break;
        case TouchPress:
            PostMessage(target, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(tartegt_pos.x, tartegt_pos.y));
            break;
        }
        return true;
    }
    return false;
}
bool WindowOperation::RightClickWindow(const POINT& clientCoords, unsigned char tt) const
{
    if (target) {
        POINT tartegt_pos = clientCoords;
        Client2Target(tartegt_pos);
        switch (tt) {
        case TouchDown:
            PostMessage(target, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(tartegt_pos.x, tartegt_pos.y));
            break;
        case TouchUp:
            PostMessage(target, WM_RBUTTONUP, 0, MAKELPARAM(tartegt_pos.x, tartegt_pos.y));
            break;
        case TouchPress:
            PostMessage(target, WM_MOUSEMOVE, MK_RBUTTON, MAKELPARAM(tartegt_pos.x, tartegt_pos.y));
            break;
        }
    }
    return false;
}
bool WindowOperation::Client2Target(POINT& screenCoords) const
{
    if (target) {
        if(ClientToScreen(myself, &screenCoords))
            if(ScreenToClient(target, &screenCoords))
                return true;
    }
    return false;
}
bool WindowOperation::Screen2Client(POINT& screenCoords) const{
    return ScreenToClient(myself, &screenCoords);
}

void WindowOperation::SetSizeForInit(ImVec2 size)
{
    if (this->size.x < 0) {
        this->size = size;
        this->origin_size = size;
    }
}

int WindowOperation::GetMemberSize() const
{
    return ui_member.size();
}

void WindowOperation::SetOriginWindowPositionAndSize(const int & begin) const
{
    float x_magnification = origin_size.x;
    float y_magnification = origin_size.y;
    x_magnification = size.x / x_magnification;
    y_magnification = size.y / y_magnification;
    for (int i = begin;i < ui_member.size();i++) {
        ui_member[i]->SizeChange(x_magnification, y_magnification, size.x, size.y);
    }
}


ImVec2 WindowOperation::GetSize() const
{
    return size;
}

ImVec2 WindowOperation::GetPos() const
{
    return pos;
}

bool WindowOperation::SetWindowPositionAndSize(int x, int y, int width, int height)
{
    
    //RECT size = GetWindowRect(myself);
    //float x_magnification = (size.right - size.left);
    //float y_magnification = (size.bottom - size.top);
    float x_magnification = size.x;
    float y_magnification = size.y;
    RECT size2;
    if (GetTargetClientRect(target, size2))
    if (SetWindowPos(myself, NULL, x, y, width, height, SWP_NOZORDER)) {
        x_magnification = (size2.right-size2.left) / x_magnification;
        y_magnification = (size2.bottom-size2.top) / y_magnification;
        for (UISize* ui_target : ui_member) {
            ui_target->SizeChange(x_magnification, y_magnification, size.x, size.y);
        }
        pos.x = x;
        pos.y = y;
        this->size.x = (size2.right - size2.left);
        this->size.y = (size2.bottom - size2.top);
        return true;
    }
    return false;
}

bool WindowOperation::Contains_Map(POINT& screenCoords) const
{
    //这里只是测试，因为测试过程中会频繁手动拖动窗口。实际使用时不会让用户拖动的。
    //实际时可以用size和pos更快。
    RECT size = GetWindowRect(myself);
    if (screenCoords.x > size.left && screenCoords.x < size.right && screenCoords.y>size.top && screenCoords.y < size.bottom) {
        return Screen2Client(screenCoords);
    }
    return false;
}
 
//通用事件
// 辅助函数：生成单个键盘输入事件
bool IsExtendedKey(WORD vk) {
    switch (vk) {
    case VK_RCONTROL:
    case VK_RMENU:
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_DIVIDE:
    case VK_NUMLOCK:
        return true;
    default:
        return false;
    }
}

INPUT MakeKeyInput(WORD vk, bool isUp) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = 0;
    if (IsExtendedKey(vk)) {
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }
    if (isUp) {
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
    }
    return input;
}
void WindowOperation::DoKey(const KeyValue& kv) {
    std::vector<INPUT> inputs;

    // 1. 修饰键按下
    if (kv.up == Down || kv.up == Click) {
        if (kv.ctrl)  inputs.push_back(MakeKeyInput(VK_CONTROL, false));
        if (kv.alt)   inputs.push_back(MakeKeyInput(VK_MENU, false));
        if (kv.shift) inputs.push_back(MakeKeyInput(VK_SHIFT, false));
    }

    // 2. 处理目标键
    if (kv.up == Click) {
        // 只处理目标键，分两次 SendInput
        inputs.push_back(MakeKeyInput(kv.vkey, false));
        //按下
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
        inputs.clear();
        inputs.push_back(MakeKeyInput(kv.vkey, true));
        Sleep(300); // 这里延时 200ms，具体可调大调小
        //这个部分到时候要修改成多线程，离线输入，不能让输出影响输入。
    }
    else {
        // 原有逻辑
        switch (kv.up) {
        case Down:
            inputs.push_back(MakeKeyInput(kv.vkey, false));
            break;
        case Up:
            inputs.push_back(MakeKeyInput(kv.vkey, true));
            break;
        }
    }

    // 3. 修饰键释放
    if (kv.up == Up || kv.up == Click) {
        if (kv.shift) inputs.push_back(MakeKeyInput(VK_SHIFT, true));
        if (kv.alt)   inputs.push_back(MakeKeyInput(VK_MENU, true));
        if (kv.ctrl)  inputs.push_back(MakeKeyInput(VK_CONTROL, true));
    }

    // 4. 发送剩余输入（修饰键）
    if (!inputs.empty()) {
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
}
//TODO 先搞个测试
void WindowOperation::DoMouseKey(const MouseKeyValue& mkv) const
{
    if (mkv.left) {
        if (mkv.state == TouchClick) {
            LeftClickWindow(mkv.point, TouchDown);
            LeftClickWindow(mkv.point, TouchUp);
            return;
        }
        LeftClickWindow(mkv.point, mkv.state);
    }
    else {
        if (mkv.state == TouchClick) {
            RightClickWindow(mkv.point, TouchDown);
            RightClickWindow(mkv.point, TouchUp);
            return;
        }
        RightClickWindow(mkv.point, mkv.state);
    }
}

void WindowOperation::DoMoiseMove(const MouseMoveValue& mmv)
{

}
