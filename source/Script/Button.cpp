#include "../header/Script/Button.h"
#include "../header/UIManager.h"
#include"../header/Event_Int_Interpreter.h"
#include "../header/UI_Event_Resources.h"
/// <summary>
/// 0 是进入
/// 1 是点下
/// 2 是抬起
/// 3 是离开
/// 自己拉数据！
/// </summary>
class ButtonEvent:public OutputEvent{
    Event_Int_Interpreter  eii;
    inline void DoIt(int index) {
        ConnectorVariant cv = eii.GetInterpretConnector(index);
        std::visit([](auto&& arg) {  // 自动推导当前类型
            using T = std::decay_t<decltype(arg)>;//不同的进行不同操作。
            if constexpr (std::is_same_v<T, KeyConnector>) {
                arg.DoDefault();
            }
            else if constexpr (std::is_same_v<T, MouseKeyConnector>) {
                arg.DoDefault();
            }
            else if constexpr (std::is_same_v<T, ContentConnector>) {
                arg.execute();
            }
            }, cv);
    }
    void Touch_Enter() override {
        DoIt(0);
    };
    void Touch_Down(int x, int y)override {
        DoIt(1);
        //std::cout << d << std::endl;
    };
    void Touch_Up(int x, int y) override {
        DoIt(2);
    };
    void Touch_Press(int x, int y) override {
        
    };
    void Touch_Leave() override {
        DoIt(3);
    };
    std::string d;
public:
    ButtonEvent(std::string name) {
       d = name;
       eii.Deserialization(UI_Event_Resources::getInstance().Get(name));
    };
    nlohmann::json GetEIISer() {
        return eii.Serialization();
    }
};

bool Button::Start(ComponentAdder* euc)
{
    UIBehavior_Operation* temp = UIManager::Create(euc, uitype, mydata->params);
    if (!temp) {
        std::cout << name<<":创建Button失败" << std::endl;
        //日志
        return false;
    }
    mybaby = dynamic_cast<ButtonWithMask*>(temp);
    event=new ButtonEvent(name);
    mybaby->_event = event;
    return true;
    //如果是编辑器模式，就不用删，如果是游玩模式，直接Delete，游玩模式不能保存。
    //delete mydata;
}
void Button::Destory()
{
    delete event;
    delete mydata;
}

void Button::Enter()
{
    //mybaby->SetPosition(ImVec2(250, 250));
    //mybaby->SetSize(ImVec2(300, 300));
}

void Button::Leave()
{
}

nlohmann::json Button::Serialization()
{
    auto transform = mybaby->GetInfo()->bounds;
    mydata->params.x = transform.left;
    mydata->params.y = transform.top;
    mydata->params.w = transform.width;
    mydata->params.h = transform.height;
    nlohmann::json j = event->GetEIISer();
    if(j!=nullptr)
        UI_Event_Resources::getInstance().Add(name,j,true);
    return mydata->Serialization();
}

bool Button::Deserialization(nlohmann::json value)
{
    throw "Do not Deserialization  in Logic Script.in Param and Create By UIManager";
    return false;
}

void Button::Update()
{
    /*ImVec2 newpos;
    newpos.x = 250 + cos(cv) * 100;
    newpos.y = 250 + sin(cv) * 100;
    cv += offset;
    mybaby->SetPosition(newpos);*/
}
