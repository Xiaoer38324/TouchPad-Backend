#include <vector>
#include "UIBehavior.h"
#pragma once
class Process_Event {//不是基类，没啥意义的操作。
protected:
    std::vector<UIBehavior*>* uiobject;
    std::vector<UIUpdateInfo>* uiobject_update_info;
    bool keep = false;
public:
   virtual int InitVar(std::vector<DataPool<InputMeta*>>&)=0;
   virtual int Init()=0;
   virtual int Begin()=0;
   inline virtual int Update(UIINT) = 0;
   virtual int Destory()=0;
   
};



