#pragma once
#include"UIBehavior.h"
#include "UIBehavior_Operation.h"
class UICreateable :public UIBehavior, public UISize,public UIBehavior_Operation,public InputMask {
public:
    /// <summary>
    /// ��Ҫ���޸ĸ����ԡ�ͨ��UIManagerͳһ����,��Ȼ���ܳ�����Ⱦ���жϲ�һ�µ��������
    /// </summary>
    /// <returns></returns>
    UIRenderInfo* GetInfo()//�������������ȫ����ζ��������Ҳ���Է��ʺ��޸���Ⱦ��Ϣ��
    {
        return info;
    }
    bool SetInfo(UIRenderInfo* info,const UIINT& name, UIOperation* operation)//�������������ȫ����ζ��������Ҳ���Է��ʺ��޸���Ⱦ��Ϣ,��������Ϊ���Ӻ������Ȼ�ȡʵ����Ȼ���ʼ�����ݡ�
    {
        if (this->info != nullptr||info ==nullptr)
            return false;
        this->info = info;
        this->name = name;
        this->operation = operation;
        return true;
    }
protected:
    UIRenderInfo* info=nullptr;
    UICreateable(UIINT name) {//�ṩ��UI��Ϣ
        this->name = name;
    }
};

