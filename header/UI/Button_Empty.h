#pragma once
#include"../UICreatable.h"
#include "../OutPutEvent.h"
#include "nlohmann/json.hpp"
#include <imgui_internal.h>
#include"../ContentManager.h"
class Button_Empty :public UICreateable, public TouchFiliter {
protected:
public:
    bool accept_input = true;
    OutputEvent* _event = nullptr;
    struct Params : public UIParams {
        
        Params(float x, float y, float w, float h)
            :  UIParams(x, y, w, h)
        {

        }
        Params() :UIParams(0, 0, 0, 0) {

        }
        nlohmann::json Serialization() override {
            return UIParams::Serialization();
        }
        bool Deserialization(nlohmann::json value) override {
            return UIParams::Deserialization(value);


        };
    };
    static UICreateable* Create(const UIParams& params) {//一定要实现，如果你需要通过UIManager生成。
        const auto& p = dynamic_cast<const Params&>(params);
        return new Button_Empty();
    }
    Button_Empty() :UICreateable(MAXUINUMBER) {}
    bool IsHitable(int x, int y) override {
        if (!accept_input)
            return false;
        return true;
    };
    void Start() override {
        
    };
    void Destory() override {

    };
    void Update(const UIUpdateInfo& uinfo)override
    {
        char number = uinfo.points->usable <= 2 ? uinfo.points->usable : 2;
        for (char i = 0;i < number;i++) {
            TouchPOINT uv = uinfo.points->data.at(i)->points;
            //if(uv.x>=i&&uv.x<=width&&uv.y>=i&&uv.y<=height)
            //if (imageData.Throughable(pixelX, pixelY)&&
            //    uinfo.points->data.at(i)->type != TouchEnter&&
            //    uinfo.points->data.at(i)->type != TouchLeave
            //    ) {
            //  /*  if (uinfo.points->data.at(i)->type==TouchDown) {
            //        std::cout << "Penetration! Down" << std::endl;
            //    }
            //    else if (uinfo.points->data.at(i)->type == TouchUp) {
            //        std::cout << "Penetration! Up" << std::endl;
            //    }
            //    else if (uinfo.points->data.at(i)->type == TouchPress) {
            //        std::cout << "Penetration! Press" << std::endl;
            //    }*/
            //    std::cout << "button pen" << std::endl;
            //    //TouchPenetration::getInstance().PushOne(uinfo.points->data.at(i));
            //    return;
            //}
            if (!Filiter(uinfo.points->data.at(i), info->bounds.width, info->bounds.height)) {
                return;
            }
            if (uinfo.points->data.at(i)->type == TouchDown) {
                if (_event != nullptr)
                    _event->Touch_Down(uv.x, uv.y);
            }
            else if (uinfo.points->data.at(i)->type == TouchUp) {
                if (_event != nullptr)
                    _event->Touch_Up(uv.x, uv.y);
            }
            else if (uinfo.points->data.at(i)->type == TouchPress) {
                if (_event != nullptr)
                    _event->Touch_Press(uv.x, uv.y);
            }
            else if (uinfo.points->data.at(i)->type == TouchEnter) {
                if (_event != nullptr)
                    _event->Touch_Enter();
            }
            else if (uinfo.points->data.at(i)->type == TouchLeave) {
                if (_event != nullptr)
                    _event->Touch_Leave();
            }
        }
    }
    void Render()override {
        if (ContentManager::getInstance().debug) {
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            ImRect bb(info->bounds.left, info->bounds.top, info->bounds.getRight(), info->bounds.getBottom());
            draw_list->AddRect(
                bb.Min,      // 矩形左上角
                bb.Max,      // 矩形右下角
                IM_COL32(255, 0, 0, 255), // RGBA颜色（红色）
                0.0f,                   // 圆角半径
                0,                      // 圆角分段数
                2.0f                    // 线宽
            );

        }
    }
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height)override
    {
    }
};
