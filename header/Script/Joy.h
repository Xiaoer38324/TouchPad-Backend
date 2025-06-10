#pragma once
#include"../Logic_Script.h"
#include"../UI/ButtonWithMask.h"
#include "../Event_Range_Interpreter.h"
#include <algorithm>
/// <summary>
/// 0 是进入
/// 1 是点下
/// 2 是抬起
/// 3 是离开
/// 从4开始，就能自定义范围数据
/// </summary>
class Joy :public ScriptBehavior, OutputEvent//逻辑脚本的模板。
{
public:
    //这个是用于导入时的数据结构，你也可以当成实际数据存储结构使用。
    struct Params : public LogicParams {
    public:
        float circle_r = 10;
        float rate = 0.1;
        ButtonWithMask::Params background;
        ButtonWithMask::Params point;
        Params(ButtonWithMask::Params background, ButtonWithMask::Params point,float rate,float circle_r)
            :background(background),point(point),rate(rate),circle_r(circle_r), LogicParams(name, "Joy")
        {

        }
        Params() : LogicParams("unknow", "Joy") {

        }
        nlohmann::json Serialization() override {//序列化数据，在这里你可以保存或转换你这个逻辑组件所需要的数据。
            nlohmann::json point_ser = point.Serialization();//组件也是实现了序列化的。
            nlohmann::json background_ser = background.Serialization();//组件也是实现了序列化的。
            nlohmann::json j = LogicParams::Serialization();
                j["Name"] = name;//这两条是必须包含的 逻辑组件实例的名字以及类型名字。
                j["Type"] = type_name;
                j["Point"] = point_ser;
                j["Background"] = background_ser;
                j["Rate"] = rate;
                j["R"]=circle_r;
            
            return j;
        }
        bool Deserialization(nlohmann::json j) override {//反序列化数据。
            try {
                name = j["Name"].get<std::string>();//这两条必须存在，不然会出错。
                type_name = j["Type"].get<std::string>();
                rate = j["Rate"].get<float>();
                circle_r = j["R"].get<float>();
                if(point.Deserialization(j["Point"].get<nlohmann::json>())==false|| background.Deserialization(j["Background"].get<nlohmann::json>())==false)
                    return false;
            }
            catch (...) {
                return false;
            }
            return true;
        }
    };
    //一定要实现，如果你需要通过UIManager生成
    //这个方法是用于配合UIManager工厂来自动化生成逻辑组件实例的。
    static ScriptBehavior* Create(LogicParams* params) {

        Params* p = dynamic_cast<Params*>(params);
        return new Joy(p);
    }
    //一定要实现，如果你需要通过UIManager生成
    //这个方法是用于配合UIManager工厂来自动化生成逻辑组件参数实例的。
    static LogicParams* GetParam() {

        return new Params();
    }
    //构造函数，参数可以自定义，只需要在Crate方法里面能提供即可。
    Joy(Params* params) :ScriptBehavior(params->name)
    {
        this->mydata = params;
    }
    ~Joy() {
        delete mydata;
    }
private:
    ButtonWithMask* point,*background = nullptr;//这个逻辑组件包含的基础组件
    bool Start(ComponentAdder* euc) override;//初始化时触发一次，可以在这里初始化数据，添加基础组件。如果返回false，说明初始化失败，该script会被删除。
    void Destory();//销毁时触发
    void Enter() override;//每次切换会该逻辑组件的上下文时触发
    void Leave() override;//每次退出该逻辑组件上下文时触发
    virtual nlohmann::json Serialization() override;//序列化数据，提供给上下文类保存时用。一般是序列化Param。可以自行添加额外逻辑
    virtual bool Deserialization(nlohmann::json value) override;//反序列化，不需要写。反序列化是在Param内部。该函数无用。
    //var_1....var_n
    std::set<int> current;
    std::set<int> last;
    Event_Range_Interpreter eri;
    ImVec2 background_lt;
    ImVec2 center;
    ImVec2 point_pos;
    inline void PointGo(const ImVec2& pos);
    Params* mydata;//Param数据，可以选择直接使用保存数据当成该逻辑组件数据，也可以不用。只要在保存的时候能转换就行。
    void Update() override;//每一帧触发。
    inline void DoIt(int index, ButtonState state) {
        ConnectorVariant cv = eri.GetInterpretConnector(index);
        std::visit([this, state](auto&& arg) {  // 自动推导当前类型
            using T = std::decay_t<decltype(arg)>;//不同的进行不同操作。
            if constexpr (std::is_same_v<T, KeyConnector>) {
                switch (state)
                {
                case Up:
                    arg.Up();
                    break;
                case Down:
                    arg.Down();
                    break;
                case Click:
                    arg.Click();
                    break;
                default:
                    break;
                }

            }
            else if constexpr (std::is_same_v<T, MouseKeyConnector>) {
                switch (state)
                {
                case Up:
                    arg.Up();
                    break;
                case Down:
                    arg.Down();
                    break;
                case Click:
                    arg.Click();
                    break;
                default:
                    break;
                }
            }
            }, cv);
    }
    //event
    void Touch_Enter() override {
        DoIt(0, ButtonState::Click);
    };
    void Touch_Down(int x, int y)override {
        x += background_lt.x;
        y += background_lt.y;
        //std::cout << "Touch Down:" << x << "," << y << std::endl;
        DoIt(1, ButtonState::Click);

    };
    void Touch_Up(int x, int y) override {
        x += background_lt.x;
        y += background_lt.y;
        PointGo(center);
       // std::cout << "Touch Up:"<<x<<","<<y << std::endl;
        DoIt(2, ButtonState::Click);
        for (const int& val : last) {
            DoIt(val, ButtonState::Up);
        }
        last.clear();
    };
    inline void circle_line_intersection(float c, float d,ImVec2& result) {
        const float dx = c - center.x;
        const float dy = d - center.y;
        const float L_squared = dx * dx + dy * dy;
        const float r_squared = mydata->circle_r * mydata->circle_r;

        // 检查点A是否在圆内（包括边界）
        if (L_squared <= r_squared) {
            result.x = c;
            result.y = d;
            return;
        }

        // 否则计算直线BA与圆的交点（仅延伸方向）
        const float L = std::sqrt(L_squared);
        const float scale = mydata->circle_r / L;
        const float x = center.x + dx * scale;
        const float y = center.y + dy * scale;

        result.x = x;
        result.y = y;
        return;
    }
    void Touch_Press(int x, int y) override {
        x += background_lt.x;
        y += background_lt.y;
        circle_line_intersection(x,y, point_pos);
        PointGo(point_pos);//TODO轮廓优化,暂时我们用圆形的。
        x -= center.x;
        y -= center.y;
        float xita_f = x / sqrtf(x * x + y * y);
        xita_f = acosf(xita_f)* 57.2957f;
        if (y < 0)
            xita_f =-xita_f+360;
        eri.GetRangeConnector(xita_f, current);
        std::set<int>new_item;
        std::set_symmetric_difference(current.begin(), current.end(), last.begin(), last.end(), std::inserter(new_item,new_item.begin()));
        /*std::cout << "N:";
        for (const int& val : new_item) {
            std::cout  << val<<",";
        }
        std::cout << std::endl;*/
        /*std::cout << "L:";
        for (const int& val : last) {
            std::cout << val << ",";
        }
        std::cout << std::endl;
        std::cout << "C:";
        for (const int& val : current) {
            std::cout << val << ",";
        }
        std::cout << std::endl;*/
        //std::cout <<"C:" << last << std::endl;
        for (const int& val:new_item) {
            if (last.count(val) != 0) {
                //离开了。
                //std::cout <<"L:" << val << std::endl;
                DoIt(val, ButtonState::Up);
            }
            else {
                //std::cout << "E:" << val << std::endl;
                DoIt(val, ButtonState::Down);
            }
        }
        last = current;
       // std::cout << name << ":" << "Touch Press" << x << "," << y << std::endl;
    };
    void Touch_Leave() override {
       // std::cout << name << ":" << "Baby Leave" << std::endl;
        DoIt(3, ButtonState::Click);
    };
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height) override;
};

