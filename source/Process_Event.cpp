#include "../header/Process_Event.h"
#include "../header/VTP_Type.h"
class Normal_Process :public Process_Event {

    /*
    Init
    full ui uiobject;
    InitVar
    Begin
    Update
    Destory
    */
    //�����UI�������̣���Ҫ������йء�
public:
    int InitVar(std::vector<DataPool<InputMeta*>>& input) override//input�Ƿ���ͳһ�����������Ķ�����Ҫ����Ϣ�ֱ����ֵ��ȥ��
    {
        //Donot change it
        for (UIINT i = 0;i < uiobject->size();i++) {
            //�������
            UIUpdateInfo uui;
            uui.points = &(input.at(i));
            uui.enable_render = true;
            uiobject_update_info->push_back(uui);
        }
        return 0;
    }
    int Init()override
    {
        return 0;
    }
    int Begin()override
    {
        try {
            for (UIBehavior* object : *uiobject) {
                object->Start();
            }
        }
        catch (...) {
            return -1;
        }
        return 0;
    }
    int Update(UIINT i)override
    {
        if (i >= uiobject->size()) {
            return 1;
        }
        try {
            uiobject->at(i)->Update(uiobject_update_info->at(i));
            if (uiobject_update_info->at(i).enable_render)
                uiobject->at(i)->Render();
            
        }
        catch (...) {
            std::cout << "error"<< std::endl;
            return 1;
        }
    }
    int Destory()override
    {
        try {
            for (UIBehavior* object : *uiobject) {
                object->Destory();
            }

        }
        catch (...) {
            return 1;
        }
        for (UIBehavior* ptr : *uiobject) {
            delete ptr; // �ͷ�ÿ��UI����
        }
        delete uiobject;
        delete uiobject_update_info;
    }
    int GetLength() {
        return uiobject->size();
    }
    UIINT GiveUI(UIBehavior* uib) {
        uiobject->push_back(uib);
        return uiobject->size() - 1;
    }
    Normal_Process() {
        uiobject = new std::vector<UIBehavior*>();
        uiobject_update_info = new std::vector<UIUpdateInfo>();
    }
};
