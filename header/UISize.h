#pragma once
class UISize {
public:
    virtual void SizeChange(float xMagnification,float yMagnification,int old_width,int old_height) = 0;//修改UI大小时触发。

};
