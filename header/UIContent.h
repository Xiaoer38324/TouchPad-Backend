#pragma once
#include"UICreatable.h"
class ScriptBehavior;
class UIContent {
    //UI�����ģ�ʵ���Ͼ���һ��UI���档���ǿ����кܶ��UI���档�����л�ʹ�á�
    //����һ�������࣬��Ҫ�Ǹ��������ĵ��¼���
public:
    virtual void Init() = 0;//��ʼ��
    virtual void Enter() = 0;//����������
    virtual void Update() = 0;//������ѭ��
    virtual void Leave() = 0;//�뿪������
    virtual void Quit()=0;//�˳������ģ�һ����ζ�����ٻ�ж�ء�
    
};
class ComponentAdder {
    //������߼�����ͻ�������¼��Ľӿڡ�
public:
    virtual UIINT Add(UICreateable* ui, const UIParams& params) = 0;//���һ��UIBehavior��
    virtual bool Remove(UIINT) = 0;//ɾ��һ��UIBehavior��
    virtual bool AddLogic(ScriptBehavior*)=0;
    virtual bool RemoveLogic(std::string ui) = 0;//ɾ��һ���߼��ű���
};
