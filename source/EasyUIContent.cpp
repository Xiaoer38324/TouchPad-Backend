#include "../header/EasyUIContent.h"
#include "nlohmann/json.hpp"
#include"../header/UIManager.h"
void PlayingUIContent::Init()
{
    pe.Init();
    for (auto sc = scripts.begin();sc != scripts.end();) {
        if (!(*sc)->Start(this)) {//如果初始化逻辑组件发现失败了，我们就直接删掉。不要影响运行。
            (*sc)->Destory();
            //失败日志
            std::cout << "某个逻辑组件失效" << std::endl;
            sc = scripts.erase(sc);
        }
        else {
            sc++;
        }
    }
    pe.InitVar(message);//这个方法需要再添加完物品再触发
    
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
    //输入事件处理。
    input->Update(message);
    for (int i = 0;i < pe.GetLength();i++) {
        pe.Update(i);
        input->ClearState(i, message);
    }
    //TouchPenetration::getInstance().Apply(*op);点击穿透用组件
    for (ScriptBehavior* sc : scripts) {
        sc->Update();
    }
    //最后处理脚本逻辑
   
}

void PlayingUIContent::Leave()
{
    for (ScriptBehavior* sc : scripts) {
        sc->Leave();
    }
}

nlohmann::json PlayingUIContent::Serialization()
{
    //这段代码一般是EditorUIContent的，游玩的不用序列化保存，只需要载入。
    std::vector<nlohmann::json> jsonvalue;
    
    for (ScriptBehavior* one:scripts) {
        try {
            jsonvalue.push_back(one->Serialization());
            //平级转换，默认是有的。logic必定要保存的。
        }
        catch (const std::bad_cast& e) {
            //失败了.这个组件我们不报错。以后要加报错日志系统
            std::cout << "布局组件初始化失败:" << e.what() << std::endl;
        }
    }
    nlohmann::json j = jsonvalue;

    return j;
}
static bool priority_cmp(const nlohmann::json& v1, nlohmann::json& v2) {
    //如果第一元素相等，则比较第二元素
    unsigned short p1= v1["Priority"].get<unsigned short>();
    unsigned short p2= v2["Priority"].get<unsigned short>();
    return p1 < p2;
}
bool PlayingUIContent::Deserialization(nlohmann::json j)
{
    //载入
    std::vector<nlohmann::json> jsonvalue = j.get<std::vector<nlohmann::json>>();
    std::sort(jsonvalue.begin(), jsonvalue.end(), priority_cmp);
    for (const nlohmann::json& json : jsonvalue) {
        std::string logic_type = json["Type"].get<std::string>();
        LogicParams* param= UIManager::CreateLogicParam(logic_type);
        if (param->Deserialization(json)) {
            std::cout << param->name<<"导入成功" << std::endl;
            UIManager::CreateLogic(this,logic_type,param);
        }
        else {
            std::cout << param->name<<":导入失败" << std::endl;
            //添加失败，日志报错
            
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

