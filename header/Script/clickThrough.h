#pragma once
#include"../header/Logic_Script.h"
#include"../UI/Button_Empty.h"
class clickThroughEvent;
class clickThrough :public ScriptBehavior//�߼��ű���ģ�塣
{
public:
    //��������ڵ���ʱ�����ݽṹ����Ҳ���Ե���ʵ�����ݴ洢�ṹʹ�á�
    struct Params : public LogicParams {
    public:
        Button_Empty::Params params;//�������һ������ButtonWithMask�ǻ������
        Params(Button_Empty::Params params)
            :params(params), LogicParams(name, "clickThrough")
        {

        }
        Params() : LogicParams("unknow", "clickThrough") {

        }
        nlohmann::json Serialization() override {//���л����ݣ�����������Ա����ת��������߼��������Ҫ�����ݡ�
            //std::string baby = params.Serialization();//���Ҳ��ʵ�������л��ġ�
            nlohmann::json j = LogicParams::Serialization();
            j["Button"] = params.Serialization();
            return j;
        }
        bool Deserialization(nlohmann::json j) override {//�����л����ݡ�
            
            try {
                if (LogicParams::Deserialization(j) == false)
                    return false;
                return params.Deserialization(j["Button"].get<nlohmann::json>());  
            }
            catch (...) {
                return false;
            }
            return true;
        }
    };
    //һ��Ҫʵ�֣��������Ҫͨ��UIManager����
    //����������������UIManager�������Զ��������߼����ʵ���ġ�
    static ScriptBehavior* Create(LogicParams* params) {

        Params* p = dynamic_cast<Params*>(params);
        return new clickThrough(p);
    }
    //һ��Ҫʵ�֣��������Ҫͨ��UIManager����
    //����������������UIManager�������Զ��������߼��������ʵ���ġ�
    static LogicParams* GetParam() {

        return new Params();
    }
    //���캯�������������Զ��壬ֻ��Ҫ��Crate�����������ṩ���ɡ�
    clickThrough(Params* params) :ScriptBehavior(params->name)
    {
         this->mydata = params;
    }
private:
    ImVec2 size;
    //ButtonWithMask* mybaby = nullptr;//����߼���������Ļ������
    bool Start(ComponentAdder* euc) override;//��ʼ��ʱ����һ�Σ������������ʼ�����ݣ���ӻ��������
    void Destory();//����ʱ����
    void Enter() override;//ÿ���л�����߼������������ʱ����
    void Leave() override;//ÿ���˳����߼����������ʱ����
    virtual nlohmann::json Serialization() override;//���л����ݣ��ṩ���������ౣ��ʱ�á�һ�������л�Param������������Ӷ����߼�
    virtual bool Deserialization(nlohmann::json value) override;//�����л�������Ҫд�������л�����Param�ڲ����ú������á�
    Button_Empty* empty;
    clickThroughEvent* xxx;
    Params* mydata;//Param���ݣ�����ѡ��ֱ��ʹ�ñ������ݵ��ɸ��߼�������ݣ�Ҳ���Բ��á�ֻҪ�ڱ����ʱ����ת�����С�
    void Update() override;//ÿһ֡������
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height) override;
};


