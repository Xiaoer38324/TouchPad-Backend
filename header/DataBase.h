#pragma once
#include"imgui.h"
#include"UIOperation.h"
class DataBase:public UISize,public UIOperation{
    std::vector<UIRenderInfo*> ui_render;
    bool lock = true;//��С�ȱ�������
public:
    UIRenderInfo* GetOneUIRender(float x,float y,float width,float height){
        UIRenderInfo* ui = new UIRenderInfo();
        ui->bounds.left = x;
        ui->bounds.top = y;
        ui->bounds.width = width;
        ui->bounds.height = height;
        ui_render.push_back(ui);
        return ui;
    }
    void Move(UIINT index,ImVec2 pos) override{
        if (index < ui_render.size()) {
            UIRenderInfo* info = ui_render[index];
            info->bounds.left =pos.x - info->bounds.width / 2;
            info->bounds.top = pos.y - info->bounds.height / 2;
        }
    }
    void ReSize(UIINT index, ImVec2 size) override {
        if (index < ui_render.size()) {
            UIRenderInfo* info = ui_render[index];
            auto center = info->bounds.getCenter();
            info->bounds.left = center.x - size.x / 2;
            info->bounds.top = center.y - size.y / 2;
            info->bounds.width = size.x;
            info->bounds.height = size.y;
        }
    }
    ImVec2 GetPosition(UIINT index) override {
        if (index < ui_render.size()) {
            UIRenderInfo* info = ui_render[index];
            auto center = info->bounds.getCenter();
            return ImVec2(center.x,center.y);
        }
        else {
            return ImVec2(0,0);
        }
    };
    ImVec4 GetTransform(UIINT index) override {
        if (index < ui_render.size()) {
            UIRenderInfo* info = ui_render[index];
            auto center = info->bounds.getCenter();
            return ImVec4(center.x, center.y, info->bounds.width, info->bounds.height);
        }
        else {
            return ImVec4(0, 0,0,0);
        }
    };
    ImVec2 GetSize(UIINT index) override {
        if (index < ui_render.size()) {
            UIRenderInfo* info = ui_render[index];
            return ImVec2(info->bounds.width, info->bounds.height);
        }
        else {
            return ImVec2(0, 0);
        }
    };
    void Lock(bool lock) {
        this->lock = lock;
    };
    ~DataBase() {
        for (UIRenderInfo* ui :ui_render) {
            delete ui;
        }
    }
    inline void UI_Ancho(Vector2<float>& ui, ScreenINT old_width, ScreenINT old_height) {
        // ���㵽�ĸ�����ľ���ƽ��
        float lt = ui.x * ui.x + ui.y * ui.y;                     // ����
        float rt = powf(old_width - ui.x, 2) + powf(ui.y, 2);     // ����
        float lb = powf(ui.x, 2) + powf(old_height - ui.y, 2);    // ����
        float rb = powf(old_width - ui.x, 2) + powf(old_height - ui.y, 2); // ����

        // ���������㵽���ĵ�ľ���ƽ��
        float center_x = old_width * 0.5f;
        float center_y = old_height * 0.5f;
        float ct = powf(ui.x - center_x, 2) + powf(ui.y - center_y, 2); // ����

        // �ҳ���С����
        float min_dist = std::min({ lt, rt, lb, rb, ct });

        // ������С����ȷ��ê��
        if (min_dist == lt) {
            ui.x = 0;  // ����ê��
            ui.y = 0;
        }
        else if (min_dist == rt) {
            ui.x = 1;  // ����ê��
            ui.y = 0;
        }
        else if (min_dist == lb) {
            ui.x = 0;  // ����ê��
            ui.y = 1;
        }
        else if (min_dist == rb) {
            ui.x = 1;  // ����ê��
            ui.y = 1;
        }
        else {
            ui.x = 0.5f;  // ����ê��
            ui.y = 0.5f;
        }
    }
    inline void Ancho_position(Box<float>& value,
        const ScreenINT& old_width,ScreenINT old_height,
        const float& xMagnification, const float& yMagnification,
        const float& old_ui_width, const float& old_ui_height,  Vector2<float> ancho) {
       Vector2<float> offset=ancho;
       ancho.x = value.left+offset.x*old_ui_width;
       ancho.y = value.top+ offset.y*old_ui_height;
       value.left = (ancho.x*xMagnification)-offset.x*value.width;
       value.top = (ancho.y * yMagnification) - offset.y * value.height;
    }
    void SizeChange(float xMagnification, float yMagnification, int old_width, int old_height)override
    {
        //std::cout << "DB change" << std::endl;
        float rate = 1;
        for (UIRenderInfo* ui : ui_render) {
            if (lock) {
                rate = ui->bounds.width / ui->bounds.height;
                float old_ui_width = ui->bounds.width;
                float old_ui_height = ui->bounds.height;
                Vector2<float> ancho = ui->bounds.getCenter();
                UI_Ancho(ancho, old_width,old_height);
                if (ui->bounds.width * xMagnification < ui->bounds.height * yMagnification) {
                    ui->bounds.width *= xMagnification;
                    ui->bounds.height = ui->bounds.width * (1 / rate);
                }
                else {
                    ui->bounds.height *= yMagnification;
                    ui->bounds.width = ui->bounds.height * rate;
                }
                Ancho_position(ui->bounds, old_width, old_height, xMagnification, yMagnification,old_ui_width,old_ui_height, ancho);
            }
            else {
                ui->bounds.left *= xMagnification;
                ui->bounds.top *= yMagnification;
                ui->bounds.width *= xMagnification;
                ui->bounds.height *= yMagnification;
            }
            
        }
    }
};
