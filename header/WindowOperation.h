#pragma once
#include <string>
#include <Windows.h>
#include "UISize.h"
#include <vector>
#include "VTP_Type.h"
#include"imgui.h"
class KeyValue;
class MouseKeyValue;
class MouseMoveValue;
/// <summary>
/// ���ɵ�������Ϊ������������ڡ�����һ��Ҫ��ʼ������Ȼ�����x��,���˽����ڲ���ȫ��������ⲿ��֧��
/// ��Ⱦ������
/// </summary>
class WindowOperation {
private:
    HWND target = nullptr;
    HWND myself = nullptr;
    std::vector<UISize*> ui_member;
    ImVec2 size= ImVec2(-1,-1), pos = ImVec2(0, 0);
    ImVec2 origin_size;
    ~WindowOperation()=default;
    WindowOperation()=default;
public:
    WindowOperation(const WindowOperation&) = delete;
    WindowOperation& operator=(const WindowOperation&) = delete;
    static WindowOperation& getInstance() {
        static WindowOperation instance;  // C++11��֤��̬�ֲ�������ʼ���̰߳�ȫ
        return instance;
    }
    void AddUISize(UISize* us) {
        ui_member.push_back(us);
    }
    inline void Init() {
        myself = GetActiveWindow();
    }
    bool SetTargetWindow(std::wstring windowName);
    bool AilgnWindow();
    bool SetCoveredWindow()const;
    /// <summary>
    /// Click 
    /// </summary>
    /// <param name="clientCoords">ImGUI's Coords</param>
    /// <returns></returns>
    bool LeftClickWindow(const POINT& clientCoords, unsigned char tt) const;
    bool RightClickWindow(const POINT& clientCoords, unsigned char tt) const ;
    bool Client2Target(POINT& screenCoords)const;
    bool Screen2Client(POINT& screenCoords)const;
    void SetSizeForInit(ImVec2);
    int GetMemberSize()const;
    void SetOriginWindowPositionAndSize(const int& begin)const;
    ImVec2 GetSize()const;
    ImVec2 GetPos()const;
    bool SetWindowPositionAndSize(int x, int y, int width, int height);
    bool Contains_Map(POINT& screenCoords) const;
    //�����Ԥ�����ͨ���¼���
    void DoKey(const KeyValue&);
    void DoMouseKey(const MouseKeyValue&) const;
    void DoMoiseMove(const MouseMoveValue&);
};
