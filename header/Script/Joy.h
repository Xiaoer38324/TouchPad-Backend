#pragma once
#include"../Logic_Script.h"
#include"../UI/ButtonWithMask.h"
#include "../Event_Range_Interpreter.h"
#include <algorithm>
/// <summary>
/// 0 �ǽ���
/// 1 �ǵ���
/// 2 ��̧��
/// 3 ���뿪
/// ��4��ʼ�������Զ��巶Χ����
/// </summary>
class Joy :public ScriptBehavior, OutputEvent//�߼��ű���ģ�塣
{
public:
    //��������ڵ���ʱ�����ݽṹ����Ҳ���Ե���ʵ�����ݴ洢�ṹʹ�á�
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
        nlohmann::json Serialization() override {//���л����ݣ�����������Ա����ת��������߼��������Ҫ�����ݡ�
            nlohmann::json point_ser = point.Serialization();//���Ҳ��ʵ�������л��ġ�
            nlohmann::json background_ser = background.Serialization();//���Ҳ��ʵ�������л��ġ�
            nlohmann::json j = LogicParams::Serialization();
                j["Name"] = name;//�������Ǳ�������� �߼����ʵ���������Լ��������֡�
                j["Type"] = type_name;
                j["Point"] = point_ser;
                j["Background"] = background_ser;
                j["Rate"] = rate;
                j["R"]=circle_r;
            
            return j;
        }
        bool Deserialization(nlohmann::json j) override {//�����л����ݡ�
            try {
                name = j["Name"].get<std::string>();//������������ڣ���Ȼ�����
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
    //һ��Ҫʵ�֣��������Ҫͨ��UIManager����
    //����������������UIManager�������Զ��������߼����ʵ���ġ�
    static ScriptBehavior* Create(LogicParams* params) {

        Params* p = dynamic_cast<Params*>(params);
        return new Joy(p);
    }
    //һ��Ҫʵ�֣��������Ҫͨ��UIManager����
    //����������������UIManager�������Զ��������߼��������ʵ���ġ�
    static LogicParams* GetParam() {

        return new Params();
    }
    //���캯�������������Զ��壬ֻ��Ҫ��Crate�����������ṩ���ɡ�
    Joy(Params* params) :ScriptBehavior(params->name)
    {
        this->mydata = params;
    }
    ~Joy() {
        delete mydata;
    }
private:
    ButtonWithMask* point,*background = nullptr;//����߼���������Ļ������
    bool Start(ComponentAdder* euc) override;//��ʼ��ʱ����һ�Σ������������ʼ�����ݣ���ӻ���������������false��˵����ʼ��ʧ�ܣ���script�ᱻɾ����
    void Destory();//����ʱ����
    void Enter() override;//ÿ���л�����߼������������ʱ����
    void Leave() override;//ÿ���˳����߼����������ʱ����
    virtual nlohmann::json Serialization() override;//���л����ݣ��ṩ���������ౣ��ʱ�á�һ�������л�Param������������Ӷ����߼�
    virtual bool Deserialization(nlohmann::json value) override;//�����л�������Ҫд�������л�����Param�ڲ����ú������á�
    //var_1....var_n
    std::set<int> current;
    std::set<int> last;
    Event_Range_Interpreter eri;
    ImVec2 background_lt;
    ImVec2 center;
    ImVec2 point_pos;
    inline void PointGo(const ImVec2& pos);
    Params* mydata;//Param���ݣ�����ѡ��ֱ��ʹ�ñ������ݵ��ɸ��߼�������ݣ�Ҳ���Բ��á�ֻҪ�ڱ����ʱ����ת�����С�
    void Update() override;//ÿһ֡������
    inline void DoIt(int index, ButtonState state) {
        ConnectorVariant cv = eri.GetInterpretConnector(index);
        std::visit([this, state](auto&& arg) {  // �Զ��Ƶ���ǰ����
            using T = std::decay_t<decltype(arg)>;//��ͬ�Ľ��в�ͬ������
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

        // ����A�Ƿ���Բ�ڣ������߽磩
        if (L_squared <= r_squared) {
            result.x = c;
            result.y = d;
            return;
        }

        // �������ֱ��BA��Բ�Ľ��㣨�����췽��
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
        PointGo(point_pos);//TODO�����Ż�,��ʱ������Բ�εġ�
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
                //�뿪�ˡ�
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

