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
    //�������߼������ڴ����¼���
    //�������£�
    /*
    �ؼ��ڰ��µ���TouchDown
    �ؼ���UP�Ŀ�������������accept_out_upΪtrue����ܿؼ����UP��
    //ֻҪ�ǰ����н��뼴��Enter���뿪ͬ��
    Press�¼�ȡ����accept_nodown.��Ϊtrue����û��DownҲ�ᴥ������֮���ᡣ
    �ؼ����Pressȡ����accept_out_precess,������ڱ��ؼ����£�ֻҪû��̧����������ᴥ�������������
    */
public:
    bool accept_out_up=false,
        accept_nodown_press = false,
        accept_out_press = false,
        accept_nodown_up= false,
        down = false;
protected:
    /*
    accept_out_up:���ܲ��ڿؼ��ϵ��ͷ��¼�
    accept_nodown_press:����û�а��µ�press�¼�
    accept_out_press:���ܲ��ڿؼ��ϵ�press�¼�
    accept_nodown_up:����û�а��µ��ͷ��¼�
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
////�������Ҫ��ȡ����ʽ�ġ�Ҳ������Ҫά��һ���־û��ĵ�����ݡ�
/// </summary>
class  InputInstance {
 public:
    virtual bool GetPos(std::vector<InputMeta>&,int&)=0;//��ȡ����ԭʼ��Ϣ
 };
 /// <summary>
 /// component can not exceed 256 in gaming mode.
 /// process :
 /// Give  A Input Trigger(InputInstance,this class is for checking input)
 /// Add UI Bounds
 ///Call Update From Main Process Event
 /// Get std::vector<DataPool<InputMeta*>>,and send it to UI,you need do it in Main ProcessEvent.
 /// Clear UI State After UI used input info.
 /// ��TM�񾭲���дӢ��ע�͡�
 /// </summary>
 class InputSystem: public UISize{
 private:
     std::vector<InputMeta> metas;
     std::vector<Box<float>*> boxes;
     /// <summary>
     /// ����ʵ���������ǹ�����
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
             //��ԭ�ؼ�����
             if(index!=MAXUINUMBER)
                GiveOne(im,boxes.at(im->master), message.at(im->master));
         }
         else if (im->type == TouchPress) {
             //���������кܶ��ˡ��������ڿؼ��ڲ���Ҳ�п������ڿؼ��ⲿ��
             //std::cout << index << ":" << im->temp_master << std::endl;
             if (index != im->temp_master) {
                 //˵���뿪���߽����ˡ���index��һ��enter����temp_master��һ��leave��
                 if (im->temp_master != MAXUINUMBER) {
                     GiveFromPool(im,TouchLeave,im->temp_master,message);
                     
                 }
                 if (index != MAXUINUMBER) {
                     GiveFromPool(im, TouchEnter, index, message);
                     
                 }
                 
                 im->temp_master = index;
                 
             }
             //��ԭ�ؼ�����Press���ݡ�
             if (im->master != MAXUINUMBER) {
                 GiveFromPool(im, TouchPress, im->master, message);
             }
             //����ǰ�ؼ�����Press���ݡ�
             if (index != im->master&&index != MAXUINUMBER) {
                 GiveFromPool(im, TouchPress, index, message);
             }
         }
         else if (im->type == TouchUp) {
             //��ԭ�ؼ���һ��Up
             if(im->master != MAXUINUMBER)
                GiveOne(im, boxes.at(im->master), message.at(im->master));
             //����ǰ�ؼ���һ��UP��ԭ�ؼ������ڵ�ǰ����¡�
             if (im->master != index&&index!=MAXUINUMBER) {
                 GiveFromPool(im, TouchUp, index, message);
             }
         }
     };
     ~InputSystem();
     void SizeChange(float xMagnification, float yMagnification, int new_width, int new_height)override;
 };
