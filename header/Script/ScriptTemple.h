#pragma once
#include"../Logic_Script.h"
#include"../UI/ButtonWithMask.h"
class ScriptTemple :public ScriptBehavior//�߼��ű���ģ�塣
{
public:
    //��������ڵ���ʱ�����ݽṹ����Ҳ���Ե���ʵ�����ݴ洢�ṹʹ�á�
    struct Params : public LogicParams {
    public:
        /*ButtonWithMask::Params params;//�������һ������ButtonWithMask�ǻ������
        Params(ButtonWithMask::Params params)
            :params(params), LogicParams(name, "Your Logic Class Name")
        {

        }*/
        Params() : LogicParams("unknow", "Your Logic Class Name") {

        }
        nlohmann::json Serialization() override {//���л����ݣ�����������Ա����ת��������߼��������Ҫ�����ݡ�
            
            nlohmann::json j = LogicParams::Serialization();
            //�������
            return j;
        }
        bool Deserialization(nlohmann::json j) override {//�����л����ݡ�
            //�ָ��������
            return LogicParams::Deserialization(j);//params.Deserialization(j["Button1"].get<std::string>());//���»ָ�ButtonWithMask�������ݡ�
        }
    };
    //һ��Ҫʵ�֣��������Ҫͨ��UIManager����
    //����������������UIManager�������Զ��������߼����ʵ���ġ�
    static ScriptBehavior* Create(LogicParams* params) {
        
        Params* p = dynamic_cast<Params*>(params);
        return new ScriptTemple(p);
    }
    //һ��Ҫʵ�֣��������Ҫͨ��UIManager����
    //����������������UIManager�������Զ��������߼��������ʵ���ġ�
    static LogicParams* GetParam() {
        
        return new Params();
    }
    //���캯�������������Զ��壬ֻ��Ҫ��Crate�����������ṩ���ɡ�
    ScriptTemple(Params* params) :ScriptBehavior(params->name)
    {
        
       // this->mydata = params;
    }
private:
    //ButtonWithMask* mybaby = nullptr;//����߼���������Ļ������
    void Start(ComponentAdder* euc) override;//��ʼ��ʱ����һ�Σ������������ʼ�����ݣ���ӻ��������
    void Destory();//����ʱ����
    void Enter() override;//ÿ���л�����߼������������ʱ����
    void Leave() override;//ÿ���˳����߼����������ʱ����
    virtual nlohmann::json Serialization() override;//���л����ݣ��ṩ���������ౣ��ʱ�á�һ�������л�Param������������Ӷ����߼�
    virtual bool Deserialization(nlohmann::json value) override;//�����л�������Ҫд�������л�����Param�ڲ����ú������á�
    //var_1....var_n
    //Params* mydata;//Param���ݣ�����ѡ��ֱ��ʹ�ñ������ݵ��ɸ��߼�������ݣ�Ҳ���Բ��á�ֻҪ�ڱ����ʱ����ת�����С�
    void Update() override;//ÿһ֡������
};

