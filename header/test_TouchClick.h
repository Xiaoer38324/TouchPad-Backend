#pragma once
#include <windows.h>
#include"InputSystem.h"
class test_TouchClick :public InputInstance {
    UINT32 pointerCount = 0;
    std::vector<POINTER_INFO> pointerInfos;
    bool GetPos(std::vector<InputMeta>& value, int& number) override
    {
        // ��ȡ��ǰ֡�е�ָ������
        if (GetPointerFrameInfo(PT_TOUCH, &pointerCount, nullptr))
        {
            if (pointerCount > 0)
            {
                if (pointerCount > pointerInfos.size()) {
                    pointerInfos.reserve(pointerCount);
                }
                number = pointerCount;
                // ��ȡ����ָ����Ϣ
                if (GetPointerFrameInfo(PT_TOUCH, &pointerCount, pointerInfos.data()))
                {
                    for (UINT32 i = 0; i < pointerCount; i++)
                    {
                        value.at(i).points.x = pointerInfos[i].ptPixelLocation.x;
                        value.at(i).points.y = pointerInfos[i].ptPixelLocation.y;

                        // ����ÿ��������
                        if (pointerInfos[i].pointerFlags & POINTER_FLAG_INCONTACT)
                        {
                            if (pointerInfos[i].pointerFlags & POINTER_FLAG_DOWN)
                            {
                                value.at(i).type = TouchDown;

                            }
                            else if (pointerInfos[i].pointerFlags & POINTER_FLAG_UPDATE)
                            {
                                value.at(i).type = TouchPress;
                                // �����ƶ�
                            }
                        }
                        else if (pointerInfos[i].pointerFlags & POINTER_FLAG_UP)
                        {
                            value.at(i).type = TouchUp;
                            // ��������
                        }

                        // ��ȡ������ID
                        value.at(i).index = pointerInfos[i].pointerId;
                    }
                }
            }
        }
    }
};
