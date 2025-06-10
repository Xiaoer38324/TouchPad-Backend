#pragma once
#include"../Logic_Script.h"
#include"../UI/ButtonWithMask.h"
class ButtonEvent;
class Button :public ScriptBehavior
{
public:
    struct Params : public LogicParams {//�������Ҫ�������ݣ����Ҳ�Ǳ���ġ�
    public:
        ButtonWithMask::Params params;
        Params(ButtonWithMask::Params params)
            :params(params), LogicParams(name, "Button")
        {

        }
        Params() : LogicParams("unknow","Button") {

        }
        nlohmann::json Serialization() override {
            nlohmann::json baby = params.Serialization();
            nlohmann::json j = LogicParams::Serialization();
            j["Button1"] = baby;
            return j;
        }
        bool Deserialization(nlohmann::json j) override {
            try {
                if (LogicParams::Deserialization(j) == false)
                    return false;
            }
            catch (...) {
                return false;
            }
            return params.Deserialization(j["Button1"].get<nlohmann::json>());
         }
    };
    static ScriptBehavior* Create( LogicParams* params) {//һ��Ҫʵ�֣��������Ҫͨ��UIManager����
        Params* p = dynamic_cast<Params*>(params);
        return new Button(p);
    }
    static LogicParams* GetParam() {//һ��Ҫʵ�֣��������Ҫͨ��UIManager����
        return new Params();
    }
    Button(Params* params) :ScriptBehavior(params->name)
    {
        this->mydata = params;   
    }
private:
    ButtonWithMask* mybaby = nullptr;
    bool Start(ComponentAdder* euc) override;
    void Destory();
    void Enter() override;
    void Leave() override;
    virtual nlohmann::json Serialization() override;
    virtual bool Deserialization(nlohmann::json value) override;
    Params* mydata;
    ButtonEvent* event=nullptr;
    std::string uitype = "ButtonWithMask";
    void Update() override;
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height) override {

    }
};

