#pragma once
#include "WindowOperation.h"
#include "VTP_Type.h"
#include <vector>
//#include"DiagonalBinary.h"
#include <functional>
#include "ObjectPool.h"
#include"PriorityIterate.h"
using namespace quadtree;
class  TouchFiliter {
    //按键的逻辑。用于触发事件。
    //定义如下：
    /*
    控件内按下的算TouchDown
    控件外UP的看参数而定，若accept_out_up为true则接受控件外的UP。
    //只要是按下中进入即算Enter，离开同理。
    Press事件取决于accept_nodown.若为true，则没有Down也会触发，反之不会。
    控件外的Press取决于accept_out_precess,如果是在本控件按下，只要没有抬起，则后续都会触发，无论在哪里。
    */
public:
    bool accept_out_up=false,
        accept_nodown_press = false,
        accept_out_press = false,
        accept_nodown_up= false,
        down = false;
protected:
    /*
    accept_out_up:接受不在控件上的释放事件
    accept_nodown_press:接受没有按下的press事件
    accept_out_press:接受不在控件上的press事件
    accept_nodown_up:接受没有按下的释放事件
    down
    */
   
    inline bool Filiter(InputMeta* im, const float& w,const float& h) {
        bool result = true;
        switch (im->type) {
        case TouchDown:
            down = true;
            break;
        case TouchUp:
            if (!down)
                result= result&&accept_nodown_up;
            if (!(im->points.x < w && im->points.y < h&& im->points.x >0  && im->points.y > 0)) {
                result= result && accept_out_up;
            }
            down = true;
            break;
        case TouchPress:
            if (!down)
                result= result && accept_nodown_press;
            if (!(im->points.x < w && im->points.y < h && im->points.x >0 && im->points.y > 0)) {
                result= result && accept_out_press;
            }
            break;
        }
        return result;
    }
};
/// <summary>
////这个类需要采取管理式的。也就是需要维护一个持久化的点击数据。
/// </summary>
class  InputInstance {
 public:
    virtual bool GetPos(std::vector<InputMeta>&,int&)=0;//获取输入原始信息
 };
 /// <summary>
 /// component can not exceed 256 in gaming mode.
 /// process :
 /// Give  A Input Trigger(InputInstance,this class is for checking input)
 /// Add UI Bounds
 ///Call Update From Main Process Event
 /// Get std::vector<DataPool<InputMeta*>>,and send it to UI,you need do it in Main ProcessEvent.
 /// Clear UI State After UI used input info.
 /// 我TM神经病，写英文注释。
 /// </summary>
 class InputSystem: public UISize{
 private:
     std::vector<InputMeta> metas;
     std::vector<Box<float>*> boxes;
     /// <summary>
     /// 输入实例不由我们管理。！
     /// </summary>
     InputInstance* instance;
     /*Box<float> GetBox(const UIINT& index) {
         if (boxes.size()> index) {
             return *boxes.at(index);
         }
         return Box<float>();
     }
     Binary_Diagonal <UIINT, std::function<Box<float>(const UIINT&)>>* BoxQuad;
     */
     Box<float>* GetBox(const UIINT& index) {
         if (boxes.size() > index) {
             return boxes.at(index);
         }
         return nullptr;
     }
     PriorityIterate<UIINT, std::function<Box<float>*(const UIINT&)>>* BoxQuad;
     ObjectPool<InputMeta> pools;
 public:
     InputSystem(InputInstance* instance, ScreenINT w, ScreenINT h);
     bool AddBounds(Box<float>*, UIINT);
     bool Update(std::vector<DataPool<InputMeta*>>&);
     bool ClearState(UIINT, std::vector<DataPool<InputMeta*>>&);
     inline void GiveOne(InputMeta* im, Box<float>* boxes, DataPool<InputMeta*>& message) {
         im->points.x -= boxes->left;
         im->points.y -= boxes->top;
         if (message.data.size() <= message.usable) {
             message.data.push_back(im);
         }
         else {
             message.data.at(message.usable) = im;
         }
         message.usable++;
     }
     inline void GiveFromPool(InputMeta* im,unsigned char _type,UIINT index, std::vector<DataPool<InputMeta*>>& message) {
                InputMeta* leave = pools.acquire();
                leave->CopyFrom(im);
                leave->type = _type;
                GiveOne(leave, boxes.at(index), message.at(index));
     }
     inline void Exter(InputMeta* im,UIINT index, std::vector<DataPool<InputMeta*>>& message) {
         if (im->type == TouchDown) {
             im->master = index;
             im->temp_master = index;
             //给原控件发送
             if(index!=MAXUINUMBER)
                GiveOne(im,boxes.at(im->master), message.at(im->master));
         }
         else if (im->type == TouchPress) {
             //这个情况就有很多了。可能是在控件内部，也有可能是在控件外部。
             //std::cout << index << ":" << im->temp_master << std::endl;
             if (index != im->temp_master) {
                 //说明离开或者进入了。给index发一个enter，给temp_master发一个leave。
                 if (im->temp_master != MAXUINUMBER) {
                     GiveFromPool(im,TouchLeave,im->temp_master,message);
                     
                 }
                 if (index != MAXUINUMBER) {
                     GiveFromPool(im, TouchEnter, index, message);
                     
                 }
                 
                 im->temp_master = index;
                 
             }
             //给原控件发送Press数据。
             if (im->master != MAXUINUMBER) {
                 GiveFromPool(im, TouchPress, im->master, message);
             }
             //给当前控件发送Press数据。
             if (index != im->master&&index != MAXUINUMBER) {
                 GiveFromPool(im, TouchPress, index, message);
             }
         }
         else if (im->type == TouchUp) {
             //给原控件放一个Up
             if(im->master != MAXUINUMBER)
                GiveOne(im, boxes.at(im->master), message.at(im->master));
             //给当前控件放一个UP。原控件不等于当前情况下。
             if (im->master != index&&index!=MAXUINUMBER) {
                 GiveFromPool(im, TouchUp, index, message);
             }
         }
     };
     ~InputSystem();
     void SizeChange(float xMagnification, float yMagnification, int new_width, int new_height)override;
 };
