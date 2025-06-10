#pragma once
class InputMask {
public:
    virtual bool IsHitable(int x, int y) {
        return false;
    };
};
