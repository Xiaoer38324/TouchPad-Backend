#pragma once
#include "UIContent.h"
#include "DataBase.h"
#include <imgui.h>
#include "../source/Process_Event.cpp"
#include "Logic_Script.h"
class PlayingUIContent :public ComponentAdder,public UIContent,public Jsonable//����ʱ�õ�������������༭ʱ����һ����
{
    DataBase db;
    //��λ����������ϵͳ�Ķ���ع���
    std::vector<DataPool<InputMeta*>> message;
    Normal_Process pe;
    InputSystem* input;
    std::vector<ScriptBehavior*> scripts;
    UIINT index=0;
    bool register_by_deser = false;
public:
     void Init() override;//��ʼ��
     void Enter() override;//����������
     void Quit()override;//�˳�������
     void Update()override;//ѭ��������
     void Leave()override;//�뿪������
     nlohmann::json Serialization()override;//���л�
     bool Deserialization(nlohmann::json value)override;//�����л�
     UIINT Add(UICreateable* ui, const UIParams& params)override;//������
     bool Remove(UIINT ui) override;//ɾ�����
     bool AddLogic(ScriptBehavior*)override;
     bool RemoveLogic(std::string ui)override;//ɾ��һ���߼��ű���
    PlayingUIContent(InputInstance* input_ins) {
        ImVec2 window_size = WindowOperation::getInstance().GetSize();
        input=new InputSystem(input_ins, window_size.x, window_size.y);
        WindowOperation::getInstance().AddUISize(&db);
        WindowOperation::getInstance().AddUISize(input);
    };
    ~PlayingUIContent()
    {
        delete input;
        for (auto v : scripts) {
            delete v;
        }
        
    }
};

