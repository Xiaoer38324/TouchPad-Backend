#include "../header/Script/Joy.h"
#include "../header/UIManager.h"
#include "../header/Event_Range_Interpreter.h"
#include "../header/UI_Event_Resources.h"
#include "../header/Event_Int_Interpreter.h"
#include <nlohmann/json.hpp>
bool Joy::Start(ComponentAdder* euc)
{
    UIBehavior_Operation* temp = UIManager::Create(euc,"ButtonWithMask", mydata->background);
    if (!temp) {
        std::cout << name << ":摇杆背景控件创建失败" << std::endl;
        //日志，生成失败
        return false;
    }
    background = dynamic_cast<ButtonWithMask*>(temp);
    background->accept_out_up = true;
    temp = UIManager::Create(euc, "ButtonWithMask", mydata->point);
    if (!temp) {
        std::cout << name << ":摇杆把手控件创建失败" << std::endl;
        //日志，生成失败
        return false;
    }
    point= dynamic_cast<ButtonWithMask*>(temp);
    point->accept_input = false;
    ImVec2 p_size = background->GetSize();
    background_lt = background->GetPosition();
    center = background_lt;
    background_lt.x -= p_size.x / 2;
    background_lt.y -= p_size.y / 2;
    p_size.x *= mydata->rate;
    p_size.y *= mydata->rate;
    point->SetSize(p_size);
    point->SetPosition(background->GetPosition());
    background->_event = this;
    background->accept_out_press = true;
    nlohmann::json j = UI_Event_Resources::getInstance().Get(name);
    eri.Deserialization(j);
    return true;
}

void Joy::Destory()
{
}

void Joy::Enter()
{
    PointGo(center);
}

void Joy::Leave()
{

}

nlohmann::json Joy::Serialization()
{
    auto transform = background->GetInfo()->bounds;
    mydata->background.x = transform.left;
    mydata->background.y = transform.top;
    mydata->background.w = transform.width;
    mydata->background.h = transform.height;
    return mydata->Serialization();
    //---------------------------------
    transform = point->GetInfo()->bounds;
    mydata->point.x = transform.left;
    mydata->point.y = transform.top;
    mydata->point.w = transform.width;
    mydata->point.h = transform.height;
    //添加事件！
    UI_Event_Resources::getInstance().Add(name,eri.Serialization(), true);
    return mydata->Serialization();
}

bool Joy::Deserialization(nlohmann::json value)
{
    throw "Do not Deserialization  in Logic Script.in Param and Create By UIManager";
    return false;
}

inline void Joy::PointGo(const ImVec2& pos)
{
    point->SetPosition(pos);
}

void Joy::Update()
{

}

void Joy::SizeChange(float xMagnification, float yMagnification, int old_width, int old_height)
{
    std::cout << "Joy Change" << std::endl;
    ImVec2 p_size = background->GetSize();
    background_lt = background->GetPosition();
    center = background_lt;
    background_lt.x -= p_size.x / 2;
    background_lt.y -= p_size.y / 2;
    mydata->circle_r *= std::min(xMagnification, yMagnification);
    PointGo(center);
}
