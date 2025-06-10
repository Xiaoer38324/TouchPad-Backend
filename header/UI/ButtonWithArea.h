#pragma once
//#include"ButtonWithMask.h"
//class ButtonWithArea:public ButtonWithMask,OutputEvent {
//    //废案，不用的，完全可以通过堆叠多个ButtonWithMask实现
//public:
//    struct AreaParams :public ButtonWithMask::Params {
//    public:
//        std::string amp;
//        Area_Material am;
//        AreaParams(std::string amp,const Material& imageData,std::string arepath, float x, float y, float w, float h)
//            : Params(imageData,x,y,w,h), amp(amp)
//        {
//            am.Load(amp);
//        }
//        AreaParams() {
//
//        }
//        nlohmann::json Serialization() override {
//            nlohmann::json j = Params::Serialization();
//            j["Area_Path"] = amp;
//            return j;
//        }
//        bool Deserialization(nlohmann::json j) override {
//            if(!Params::Deserialization(j))
//                return false;
//            try {
//                amp = j["Area_Path"].get<std::string>();
//                am.Load(amp);
//                return true;
//            }
//            catch (...) {
//                return false;
//            }
//
//
//        };
//    };
//    static UICreateable* Create(const UIParams& params) {//一定要实现，如果你需要通过UIManager生成。
//        const auto& p = dynamic_cast<const AreaParams&>(params);
//        return new ButtonWithArea(p);
//    }
//    ButtonWithArea(AreaParams ap) : ButtonWithMask(ap.imageData){
//        this->ap = ap;
//        _event.reserve(ap.am.Size());
//    }
//    void AddEvent(OutputEvent*, unsigned char index);
//    void Touch_Enter() override {
//
//    };
//    void Touch_Down(int x, int y)override {
//        float rex = x / width;
//        float rey = x / height;
//        unsigned char index=ap.am.GetIndex(rex,rey);
//        if (_event[index] != nullptr)
//            _event[index]->Touch_Down(x, y);
//    };
//    void Touch_Up(int x, int y) override {
//        float rex = x / width;
//        float rey = x / height;
//        unsigned char index = ap.am.GetIndex(rex, rey);
//        if (_event[index] != nullptr)
//            _event[index]->Touch_Up(x, y);
//    };
//    void Touch_Press(int x, int y) override {
//        float rex = x / width;
//        float rey = x / height;
//        unsigned char index = ap.am.GetIndex(rex, rey);
//        if (_event[index] != nullptr)
//            _event[index]->Touch_Press(x, y);
//    };
//    void Touch_Leave() override {
//       
//    };
//private:
//    AreaParams ap;
//    unsigned char now=255;
//    std::vector<OutputEvent*> _event;
//};
