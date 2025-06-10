#pragma once
#include "Jsonable.h"
#include <functional>
#include <variant>
#include "WindowOperation.h"
#include "ContentManager.h"
//��������ֱ��д�����ˡ�һ�����ⰴ���ò�����ô�ߵ���չ�ԡ�
template <typename T>
struct EventConnector:public Jsonable {//�������л��뷴���л�����Ҫ��3�鰴���������������������������ƶ���������
protected:
    T value;//any���÷����л�
    std::function<void(const T&)> action;  // ����
public:
    void execute() {
        if (action) action(value);
    }
    //���л�Ҫ����ֵ���������͡�������͹��ˡ�
};
enum ButtonState {
    Up,Down,Click
};
struct KeyValue{
    bool ctrl, alt, shift;
    ButtonState up;
    DWORD vkey;
};
class KeyConnector :public EventConnector<KeyValue> {
public:
    void Up() {
        value.up = ButtonState::Up;
        execute();
    }
    void Down() {
        value.up = ButtonState::Down;
        execute();
    }
    void Click() {
        value.up = ButtonState::Click;
        execute();
    }
    void DoDefault() {
        execute();
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
                {"HelpKey",nlohmann::json::array({value.ctrl,value.alt,value.shift})},
                {"Key",value.vkey},
                {"State",value.up}
        };
        return j;
    };
    bool Deserialization(nlohmann::json j)override {
        try {
            std::vector<bool> helper = j["HelpKey"].get<std::vector<bool>>();
            this->value.ctrl = helper[0];
            this->value.alt = helper[1];
            this->value.shift = helper[2];
            this->value.vkey = j["Key"].get<DWORD>();
            this->value.up = j["State"].get<ButtonState>();
        }
        catch (...) {
            return false;
        }
    };
    KeyConnector(DWORD vkey,bool ctrl,bool alt,bool shift, ButtonState state= ButtonState::Click) {
        value.vkey = vkey;
        value.ctrl = ctrl;
        value.alt = alt;
        value.shift = shift;
        value.up = state;
        action = [](const KeyValue& kv) {
            WindowOperation::getInstance().DoKey(kv);
       };
    }
    KeyConnector() {
        value.vkey = -1;
        value.ctrl = false;
        value.alt = false;
        value.shift = false;
        action = [](const KeyValue& kv) {
            WindowOperation::getInstance().DoKey(kv);
            };
    }

};
struct MouseKeyValue {
    POINT point;
    unsigned char state;
    bool left;
};
class MouseKeyConnector :public EventConnector<MouseKeyValue> {
public:
    void Up() {
        value.state = TouchUp;
        execute();
    }
    void Down() {
        value.state = TouchDown;
        execute();
    }
    void Click() {
        value.state = TouchClick;
        execute();
    }
    void Press() {
        value.state = TouchPress;
        execute();
    }
    void DoDefault() {
        execute();
    }
    void ChangePos(float x,float y) {
        value.point.x = x;
        value.point.y = y;
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
                {"Pos",nlohmann::json::array({value.point.x,value.point.y})},
                {"Key",value.left},
                {"State",value.state}
        };
        return j;
    };
    bool Deserialization(nlohmann::json j)override {
        try {
            std::vector<float> helper = j["Pos"].get<std::vector<float>>();
            this->value.point.x = helper[0];
            this->value.point.y = helper[1];
            this->value.left = j["Key"].get<int>();
            this->value.state = j["State"].get<unsigned char>();
        }
        catch (...) {
            return false;
        }
    };

    MouseKeyConnector(float x, float y,bool left=true, unsigned char state=TouchClick) {
        value.left = left;
        ChangePos(x,y);
        value.state = state;
        action = [](const MouseKeyValue& kv) {
            WindowOperation::getInstance().DoMouseKey(kv);
            };
    }
    MouseKeyConnector() {
        value.left = true;
        value.state = TouchClick;
        action = [](const MouseKeyValue& kv) {
            WindowOperation::getInstance().DoMouseKey(kv);
            };
    }
};
struct MouseMoveValue {
    float x, y;
    float speed;
};
class MouseMoveConnector :public EventConnector<MouseMoveValue> {
public:
    
    void ChangeDirection(float x, float y) {
        float sqr = sqrtf(x * x + y * y);
        value.x = x/sqr;
        value.y = y / sqr;

    }
    void ChangeSpeed(float speed) {
        value.speed = speed;
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
                {"PAS",nlohmann::json::array({value.x,value.y,value.speed})},
        };
        return j;
    };
    bool Deserialization(nlohmann::json j)override {
        try {
            std::vector<float> helper = j["PAS"].get<std::vector<float>>();
            this->value.x = helper[0];
            this->value.y = helper[1];
            this->value.speed = helper[2];
        }
        catch (...) {
            return false;
        }
    };
};
class EmptyConnector :public EventConnector<char> {
public:
    nlohmann::json Serialization() override {
        nlohmann::json j;
        return j;
    };
    bool Deserialization(nlohmann::json value)override {
        return true;
    };
};
class ContentConnector:public EventConnector<std::string> {
public:
    nlohmann::json Serialization() override {
        nlohmann::json j = { { "CName", value} };
        return j;
    };
    bool Deserialization(nlohmann::json value)override {
        if (value.contains("CName"))
        {
            this->value = value["CName"].get<std::string>();
            return true;
        }
        else {
            return false;
        }
    };
    ContentConnector(std::string name) {
        value = name;
        action = [](const std::string& kv) {
            ContentManager::getInstance().SwitchContent(kv);
            };
    }
};
using ConnectorVariant = std::variant<KeyConnector,MouseKeyConnector,MouseMoveConnector,ContentConnector,EmptyConnector>;
inline ConnectorVariant InitCV(int type, nlohmann::json json) {
    ConnectorVariant result;
    switch (type) {
        case 0:
            result = KeyConnector();
            std::get<KeyConnector>(result).Deserialization(json);
            break;
        case 1:
            result = MouseKeyConnector();
            std::get<MouseKeyConnector>(result).Deserialization(json);
            break;
        case 2:
            result = MouseMoveConnector();
            std::get<MouseMoveConnector>(result).Deserialization(json);
            break;
        case 3:
            result = ContentConnector("");
            std::get<ContentConnector>(result).Deserialization(json);
            break;
        case 4:
            result = EmptyConnector();
            break;

    }
    return result;
}
// ���������ࣨ���ͣ�������ǵ��õĽӿڡ�
template <typename T>
class Event_InterpreterBase :public Jsonable {//���л�Ҫ��ϸ�̶�д��
public:
    virtual ~Event_InterpreterBase() = default;

    // ��������ֵ��ִ�ж�Ӧ����
    virtual ConnectorVariant GetInterpretConnector(T input) = 0;

    // δֵ֪�Ĵ����ɱ����า�ǣ�
    virtual void handleUnknownValue(T input) {};
};
