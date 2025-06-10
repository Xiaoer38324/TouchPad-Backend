#pragma once
#include"UIOperation.h"
#include"UIContent.h"
#include "Jsonable.h"
class LogicParams:public Jsonable {
public:
    virtual ~LogicParams() = default;
    std::string name;
    std::string type_name;
    unsigned short priority=0;//���ȶȡ�ԽСԽ�����棬Խ����յ���¼�����ȾԽ�硣
    
    LogicParams(const std::string& name,const std::string type_name)
        : name(name),type_name(type_name)
    {
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
            {"Priority",priority},
            {"Name",name },
            {"Type",type_name}
        };
        return j;
    }
    bool Deserialization(nlohmann::json j) override {
        try {
            name = j["Name"].get<std::string>();
            type_name = j["Type"].get<std::string>();
            priority = j["Priority"].get<unsigned short>();
        }
        catch (...) {
            return false;
        }
        return true;
    }
};
class ScriptBehavior :public UISize,public Jsonable {//Jsonableһ����������̳У�����Ϊ�˷��������Ĺ�����ScriptBehavior�����ˡ�
protected:
    std::string name;//�ű�UI�����Ǵ��߼������UI��������ϻ���UIʹ�á�Ҳ�����û��ܼ�����UI������ֱ�ӻ��á�
    //��Ϊ���̳�˫��
public:
    virtual void Enter() = 0;
    virtual void Leave() = 0;
    virtual void Update() = 0;//ÿһ֡�ĸ���
    virtual bool Start(ComponentAdder*) = 0;//��ʼ������
    virtual void Destory() = 0;//�������
    virtual ~ScriptBehavior() = default;
    ScriptBehavior(const std::string& name)
        : name(name)
    {
    }
};



