#include "../header/EasyUIContent.h"
#include "nlohmann/json.hpp"
#include"../header/UIManager.h"
void PlayingUIContent::Init()
{
    pe.Init();
    for (auto sc = scripts.begin();sc != scripts.end();) {
        if (!(*sc)->Start(this)) {//�����ʼ���߼��������ʧ���ˣ����Ǿ�ֱ��ɾ������ҪӰ�����С�
            (*sc)->Destory();
            //ʧ����־
            std::cout << "ĳ���߼����ʧЧ" << std::endl;
            sc = scripts.erase(sc);
        }
        else {
            sc++;
        }
    }
    pe.InitVar(message);//���������Ҫ���������Ʒ�ٴ���
    
}

void PlayingUIContent::Enter()
{
    pe.Begin();
    for (ScriptBehavior* sc : scripts) {
        sc->Enter();
    }
}

void PlayingUIContent::Quit()
{
    for (ScriptBehavior* sc : scripts) {
        sc->Destory();
    }
    pe.Destory();
    
}

void PlayingUIContent::Update()
{
    //�����¼�����
    input->Update(message);
    for (int i = 0;i < pe.GetLength();i++) {
        pe.Update(i);
        input->ClearState(i, message);
    }
    //TouchPenetration::getInstance().Apply(*op);�����͸�����
    for (ScriptBehavior* sc : scripts) {
        sc->Update();
    }
    //�����ű��߼�
   
}

void PlayingUIContent::Leave()
{
    for (ScriptBehavior* sc : scripts) {
        sc->Leave();
    }
}

nlohmann::json PlayingUIContent::Serialization()
{
    //��δ���һ����EditorUIContent�ģ�����Ĳ������л����棬ֻ��Ҫ���롣
    std::vector<nlohmann::json> jsonvalue;
    
    for (ScriptBehavior* one:scripts) {
        try {
            jsonvalue.push_back(one->Serialization());
            //ƽ��ת����Ĭ�����еġ�logic�ض�Ҫ����ġ�
        }
        catch (const std::bad_cast& e) {
            //ʧ����.���������ǲ������Ժ�Ҫ�ӱ�����־ϵͳ
            std::cout << "���������ʼ��ʧ��:" << e.what() << std::endl;
        }
    }
    nlohmann::json j = jsonvalue;

    return j;
}
static bool priority_cmp(const nlohmann::json& v1, nlohmann::json& v2) {
    //�����һԪ����ȣ���Ƚϵڶ�Ԫ��
    unsigned short p1= v1["Priority"].get<unsigned short>();
    unsigned short p2= v2["Priority"].get<unsigned short>();
    return p1 < p2;
}
bool PlayingUIContent::Deserialization(nlohmann::json j)
{
    //����
    std::vector<nlohmann::json> jsonvalue = j.get<std::vector<nlohmann::json>>();
    std::sort(jsonvalue.begin(), jsonvalue.end(), priority_cmp);
    for (const nlohmann::json& json : jsonvalue) {
        std::string logic_type = json["Type"].get<std::string>();
        LogicParams* param= UIManager::CreateLogicParam(logic_type);
        if (param->Deserialization(json)) {
            std::cout << param->name<<"����ɹ�" << std::endl;
            UIManager::CreateLogic(this,logic_type,param);
        }
        else {
            std::cout << param->name<<":����ʧ��" << std::endl;
            //���ʧ�ܣ���־����
            
        }
        
    }
    
    return true;
}

UIINT PlayingUIContent::Add(UICreateable* ui, const UIParams& params)
{
    try {
    ui->SetInfo(db.GetOneUIRender(params.x, params.y, params.w, params.h),index,&db);
    ui->GetInfo()->bounds.mask = ui;
    input->AddBounds(&(ui->GetInfo()->bounds), index);
    pe.GiveUI(ui);
    DataPool<InputMeta*> dp;
    message.push_back(dp);
    WindowOperation::getInstance().AddUISize(ui);
    index++;
    }
    catch (...) {
        return MAXUINUMBER;
    }
    return index-1;
}
bool PlayingUIContent::Remove(UIINT ui)
{
    return false;
}
bool PlayingUIContent::AddLogic(ScriptBehavior* behavior)
{
    if (behavior == nullptr)
        return false;
    this->scripts.push_back(behavior);
    WindowOperation::getInstance().AddUISize(behavior);
    return true;
}

bool PlayingUIContent::RemoveLogic(std::string ui)
{
    return false;
}

