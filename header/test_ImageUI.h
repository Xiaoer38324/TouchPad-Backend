#pragma once
#include"UIBehavior.h"
#include <iostream>
#include "Material.h"
#include <imgui_internal.h>
class ImageTest:public UIBehavior,public UISize {
    ImTextureID textureID;
    Material imageData;
    float width, height;
    std::string name;
    inline Material LoadImageData(const char* filepath);
public:
    ImageTest(const char* filepath,std::string name, UIRenderInfo* info);
    void Update(const UIUpdateInfo& uinfo) override;
    void Render() override;
    void Start() override {

    };
    void SizeChange(float xMagnification, float yMagnification, int new_width, int new_height)override;
    void Destory() override {

    };
};
