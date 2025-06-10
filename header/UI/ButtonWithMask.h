#pragma once
#include"../UICreatable.h"
#include <iostream>
#include "../Material.h"
#include <imgui_internal.h>
#include "../OutputEvent.h"
#include "nlohmann/json.hpp"
#include"../InputMask.h"
#include"../ContentManager.h"
class ButtonWithMask :public UICreateable,public TouchFiliter {
protected:
    ImTextureID textureID;
    Material imageData;
public:
    bool accept_input = true;
    OutputEvent* _event=nullptr;
    struct Params : public UIParams {
        std::string path;
        std::vector<bool> input_action;
        Params(const std::string& imageData, float x, float y, float w, float h, std::vector<bool> input_action = { false,false,false,false })
            : path(imageData), UIParams(x, y, w, h), input_action(input_action)
        {

        }
        Params():UIParams(0, 0, 0, 0) {
            
        }
        nlohmann::json Serialization() override {
            nlohmann::json j=UIParams::Serialization();
            j["Material"] = path;
            j["actions"] = input_action;
            return j;
        }
        bool Deserialization(nlohmann::json j) override {
            try {
                if (!UIParams::Deserialization(j))
                    return false;
                 path = j["Material"].get<std::string>();
                 input_action = TryOrDefault<std::vector<bool>>(j, "actions", input_action);
            }
            catch (...) {
                return false;
            }
            
            return true;
        };
    };
    static UICreateable* Create(const UIParams& params){//一定要实现，如果你需要通过UIManager生成。
        const auto& p = dynamic_cast<const Params&>(params);
        Material ma;
        if (Material_Resources::getInstance().Get(p.path, ma) == false) {
            return nullptr;
        };
        return new ButtonWithMask(ma,p.input_action);
    }
    ButtonWithMask(Material imageData, std::vector<bool> actions):UICreateable(MAXUINUMBER) {
        this->imageData = imageData;
        //imageData.DebugMask();
        // 加载图片为 OpenGL 纹理
        this->textureID = (ImTextureID)(intptr_t)(imageData.texture.id);
        accept_out_up = actions[0];
        accept_nodown_press = actions[1];
        accept_out_press = actions[2];
        accept_nodown_up = actions[3];
    }
    bool IsHitable(int x, int y) override {
        if (!accept_input)
            return false;
        x -= info->bounds.left;
        y -= info->bounds.top;
        TouchINT pixelX = static_cast<int>(x / info->bounds.width * imageData.texture.width);
        TouchINT pixelY = static_cast<int>(y / info->bounds.height * imageData.texture.height);
        return !imageData.Throughable(pixelX, pixelY);
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
            TouchINT pixelX = static_cast<int>(uv.x / info->bounds.width * imageData.texture.width);
            TouchINT pixelY = static_cast<int>(uv.y / info->bounds.height * imageData.texture.height);
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
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        // 计算按钮的边界框
        ImRect bb(info->bounds.left, info->bounds.top, info->bounds.getRight(), info->bounds.getBottom());
        ImGuiID id = window->GetID("##MaskedImageButton");

        // 将按钮添加到窗口中
        if (!ImGui::ItemAdd(bb, id))
            return;
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        if(ContentManager::getInstance().debug)
        draw_list->AddRect(
            bb.Min,      // 矩形左上角
            bb.Max,      // 矩形右下角
            IM_COL32(255, 0, 0, 255), // RGBA颜色（红色）
            0.0f,                   // 圆角半径
            0,                      // 圆角分段数
            2.0f                    // 线宽
        );
        // 绘制图片
        draw_list->AddImage(textureID, bb.Min, bb.Max, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
        
    }
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height)override
    {

    }
};


