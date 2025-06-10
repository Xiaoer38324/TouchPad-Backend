#include "../header/InputSystem.h"
#include "../header/WindowOperation.h"
 

InputSystem::InputSystem(InputInstance* instance, ScreenINT w, ScreenINT h)
{
    this->instance = instance;
    //std::function < Box<float> (const UIINT&)> oh_yes = std::bind(&InputSystem::GetBox, this, std::placeholders::_1);
    //BoxQuad = new Binary_Diagonal <UIINT, std::function<Box<float>(const UIINT&)>>(oh_yes, w, h);
    std::function < Box<float>* (const UIINT&)> oh_yes = std::bind(&InputSystem::GetBox, this, std::placeholders::_1);
    BoxQuad = new PriorityIterate<UIINT, std::function<Box<float>*(const UIINT&)>>(oh_yes, w, h);
}
bool InputSystem::AddBounds(Box<float>* box, UIINT index)
{
    if (boxes.size()>=index) {
        int appendnum = index - boxes.size()+1;
        for (;appendnum > 0;appendnum--) {
            boxes.push_back(nullptr);
        }
    }
    boxes.at(index) = box;
    BoxQuad->add(index);
    return true;
}
/// <summary>
/// 
/// </summary>
/// <param name="message"> size of message must equal ui component's number</param>
/// <returns></returns>
bool InputSystem::Update(std::vector<DataPool<InputMeta*>>& message)
{
    int useful_num;
    //TouchPenetration::getInstance().Clear();已放弃，转用点击穿透控件，提升通用性。而不是单独作为一个机制。

    if (instance->GetPos(metas, useful_num) == false)
        return false;
    if (useful_num > 0) {
        POINT temp;
        for (int i = 0;i < useful_num;i++) {
            //判断是不是点在屏幕区域上的。
            //分拣数据。
            //将屏幕坐标转换到客户端坐标。
            //进行点击穿透分拣。
            //从客户端目标转换到UI坐标。
            //控件外信号分拣。
            temp.x = metas.at(i).points.x;
            temp.y = metas.at(i).points.y;
            if (!WindowOperation::getInstance().Contains_Map(temp)) {
                continue;
            }
            metas.at(i).points.x = temp.x;
            metas.at(i).points.y = temp.y;
            UIINT index;
            //咱们这里需要优化，把点击穿透当成一个组件来用。在搜索的时候，我们需要加入Mask机制。
            if (!BoxQuad->query(temp,index)) {//空处
                index = MAXUINUMBER;//这个值代表着空处的id
            };
            Exter(&metas.at(i),index, message);
            
        }
        return true;
    }
    return false;
}

bool InputSystem::ClearState(UIINT index, std::vector<DataPool<InputMeta*>>& message)
{
    message.at(index).usable = 0;
    pools.releaseAll();
    return true;
}


InputSystem::~InputSystem()
{
    pools.clear();
    delete BoxQuad;
}

void InputSystem::SizeChange(float xMagnification, float yMagnification, int new_width, int new_height)
{
    /*BoxQuad->FlashPos();*/
}
