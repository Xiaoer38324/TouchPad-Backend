#include "../header/Script/clickThrough.h"
#include "../header/UIManager.h"
#include "../header/Event_Int_Interpreter.h"
#include "../header/UI_Event_Resources.h"

/// <summary>
/// 0 是点下
/// 1 是抬起
/// 自己拉数据！
/// </summary>
class clickThroughEvent :public OutputEvent {
    Event_Int_Interpreter  eii;
    ImVec2 pos;
    inline void DoIt(float x,float y,int index, unsigned char state) {
        ConnectorVariant cv = eii.GetInterpretConnector(index);
        std::visit([this, state,x,y](auto&& arg) {  // 自动推导当前类型
            using T = std::decay_t<decltype(arg)>;//不同的进行不同操作。
            if constexpr (std::is_same_v<T, MouseKeyConnector>) {
                arg.ChangePos(x,y);
                switch (state)
                {
                case TouchDown:
                    //std::cout << x << ":Down:" << y << std::endl;
                    arg.Down();
                    break;
                case TouchUp:
                    //std::cout << x << ":UP:" << y << std::endl;
                    arg.Up();
                    break;
                case TouchClick:
                    arg.Click();
                    break;
                case TouchPress:
                    arg.Press();
                    break;
                default:
                    break;
                }
            }
            else if constexpr (std::is_same_v<T, MouseMoveConnector>) {
                arg.ChangeDirection(x, y);
                arg.execute();
            }
            }, cv);
         
    }
    void Touch_Enter() override {
        
    };
    void Touch_Down(int x, int y)override {
        x += pos.x;
        y += pos.y;
        DoIt(x,y,0,TouchDown);
    };
    void Touch_Up(int x, int y) override {
        x += pos.x;
        y += pos.y;
        DoIt(x, y, 0, TouchUp);
    };
    void Touch_Press(int x, int y) override {
        x += pos.x;
        y += pos.y;
        DoIt(x, y, 0, TouchPress);
    };
    void Touch_Leave() override {
        
    };
    
public:
    clickThroughEvent(std::string name,float x,float y) {
        pos.x = x;
        pos.y = y;
        eii.Deserialization(UI_Event_Resources::getInstance().Get(name));
    };
    void SetPos(float x, float y) {
        pos.x = x;
        pos.y = y;
    }
    nlohmann::json GetEIISer() {
        return eii.Serialization();
    }
};
bool clickThrough::Start(ComponentAdder* euc)
{
    UIBehavior_Operation* temp = UIManager::Create(euc, "Button_Empty", mydata->params);
    empty = dynamic_cast<Button_Empty*>(temp);
    auto lt=empty->GetInfo()->bounds.getTopLeft();
    xxx = new clickThroughEvent(name,lt.x,lt.y);
    empty->_event = xxx;
    size = empty->GetSize();
    return true;
}

void clickThrough::Destory()
{
    //不要在这里销毁组件
    delete xxx;
    delete mydata;
}

void clickThrough::Enter()
{

}

void clickThrough::Leave()
{
}

nlohmann::json clickThrough::Serialization()
{
    auto transform = empty->GetInfo()->bounds;
    mydata->params.x = transform.left;
    mydata->params.y = transform.top;
    mydata->params.w = transform.width;
    mydata->params.h = transform.height;
    UI_Event_Resources::getInstance().Add(name, xxx->GetEIISer(), true);
    return mydata->Serialization();
}

bool clickThrough::Deserialization(nlohmann::json value)
{
    return false;
}

void clickThrough::Update()
{
}

void clickThrough::SizeChange(float xMagnification, float yMagnification, int old_width, int old_height)
{
    size.x *= xMagnification;
    size.y *= yMagnification;
    empty->SetSize(size);
    auto pos = empty->GetInfo()->bounds.getTopLeft();
    xxx->SetPos(pos.x, pos.y);
}
