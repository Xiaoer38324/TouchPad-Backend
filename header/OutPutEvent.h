#pragma once
#include"VTP_Type.h"
class OutputEvent {
public:
    virtual void Touch_Enter() = 0;
    virtual void Touch_Down(int x, int y)=0;
    virtual void Touch_Up(int x, int y) = 0;
    virtual void Touch_Press(int x, int y) = 0;
    virtual void Touch_Leave() = 0;
};
