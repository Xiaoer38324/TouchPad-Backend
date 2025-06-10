#pragma once
#include <windows.h>
#include"InputSystem.h"
class test_TouchClick :public InputInstance {
    UINT32 pointerCount = 0;
    std::vector<POINTER_INFO> pointerInfos;
    bool GetPos(std::vector<InputMeta>& value, int& number) override
    {
        // 获取当前帧中的指针数量
        if (GetPointerFrameInfo(PT_TOUCH, &pointerCount, nullptr))
        {
            if (pointerCount > 0)
            {
                if (pointerCount > pointerInfos.size()) {
                    pointerInfos.reserve(pointerCount);
                }
                number = pointerCount;
                // 获取所有指针信息
                if (GetPointerFrameInfo(PT_TOUCH, &pointerCount, pointerInfos.data()))
                {
                    for (UINT32 i = 0; i < pointerCount; i++)
                    {
                        value.at(i).points.x = pointerInfos[i].ptPixelLocation.x;
                        value.at(i).points.y = pointerInfos[i].ptPixelLocation.y;

                        // 处理每个触摸点
                        if (pointerInfos[i].pointerFlags & POINTER_FLAG_INCONTACT)
                        {
                            if (pointerInfos[i].pointerFlags & POINTER_FLAG_DOWN)
                            {
                                value.at(i).type = TouchDown;

                            }
                            else if (pointerInfos[i].pointerFlags & POINTER_FLAG_UPDATE)
                            {
                                value.at(i).type = TouchPress;
                                // 触摸移动
                            }
                        }
                        else if (pointerInfos[i].pointerFlags & POINTER_FLAG_UP)
                        {
                            value.at(i).type = TouchUp;
                            // 触摸结束
                        }

                        // 获取触摸点ID
                        value.at(i).index = pointerInfos[i].pointerId;
                    }
                }
            }
        }
    }
};
