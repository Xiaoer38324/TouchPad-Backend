#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_map>
#include"Jsonable.h"
struct TextureData {
    GLuint id=0;
    int width=0;
    int height=0;
    int channel=0;
};
//class Area_Material {
// //废案
//private:
//    unsigned char next_region_id = 1; // 区域ID从1开始（0可保留为背景）
// public:
//        int width;
//        int height;
//        std::vector<char> area;
//        inline  uint32_t color_to_key(const unsigned char* pixel, int channels) {
//            uint32_t key = 0;
//            channels = (((3) < (channels)) ? (3) : (channels));
//            for (int i = 0; i < channels; ++i) { // 只取前3通道（RGB）
//                key = (key << 8) | pixel[i];
//            }
//            return key;
//        }
//        inline unsigned char Size() const { return next_region_id;};
//        Area_Material() {
//
//        };
//        std::unordered_map<uint32_t, unsigned char> Load(const std::string& path);
//        unsigned char GetIndex( float x,  float y);
//};
class Material {
public:
    TextureData texture;
    std::vector<bool> mask;
    bool Throughable(int x, int y) {
        if (mask.size() <= 0) {
            return false;
        }
        if (y * texture.width + x < mask.size())
            return mask[y * texture.width + x];
        else
            return false;
    }
    void DebugMask() {
        for (int i = 0;i < texture.height;i++) {
            for (int j = 0;j < texture.width;j++) {
                if (mask[j * texture.width + i])
                    std::cout << "*";
                else
                    std::cout << ".";
            }
            std::cout << std::endl;
        }
    }
};
enum MaskGenerType {
    ALPHA, ANOTHER, NONE
};
class MaterialInfo:public Jsonable {
public:
    std::string path;
    MaskGenerType _type=MaskGenerType::NONE;
    GLubyte alpha=0;
    std::string mask;
    bool yes = false;
    MaterialInfo(const std::string& path, const MaskGenerType& _type, const GLubyte& alpha, const std::string& mask="")
        : path(path), _type(_type), alpha(alpha), mask(mask)
    {
    }

    bool load(Material& ma) {
        return LoadMaterial(ma);

    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
            {"Path",path},
            {"Type",_type},
            {"Alpha",alpha},
            {"Mask",mask}
        };
        return j;
    }
    bool Deserialization(nlohmann::json j) override {
        try {
            path = j["Path"].get<std::string>();
            _type = j["Type"].get<MaskGenerType>();
            alpha = j["Alpha"].get<GLubyte>();
            mask = j["Mask"].get<std::string>();
            return true;
        }
        catch (...) {
            return false;
        }


    };
private:
    bool LoadMaterial(Material& material);
public:

    MaterialInfo() = default;
};

class Material_Resources:public Jsonable {
public:
    static Material_Resources& getInstance() {
        static Material_Resources instance;  // C++11保证静态局部变量初始化线程安全
        return instance;
    }
    //TODO 填数据，拉数据，清空内存数据。
    bool Add(MaterialInfo ma,bool cover = false) {
        if (cover) {
            json_database[ma.path] = { ma,Material() };
            return true;
        }
        if (json_database.find(ma.path) == json_database.end()) {
            json_database[ma.path] = { ma,Material() };
            return true;
        }
        return false;
    }
    bool Get(const std::string& name, Material& ma) {
        if (json_database.find(name) != json_database.end()) {
            if (!json_database[name].first.yes)
            {
                if (!json_database[name].first.load(json_database[name].second)) {
                    return false;
                };
                json_database[name].first.yes = true;
            }
            ma= json_database[name].second;
            return true;
        }
        return false;
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
            
        };
        for (auto it = json_database.begin();it != json_database.end();it++) {
            j.push_back(it->second.first.Serialization());
        }
        return j;
    }
    bool Deserialization(nlohmann::json j) override {
        try {
            for (auto it = j.begin();it != j.end();it++) {
                MaterialInfo mi;
                mi.Deserialization(*it);
                Add(mi);
            }
        }
        catch (...) {
            return false;
        }
        return true;
    }
private:
    void Clear() {
        json_database.clear();
    }
    std::unordered_map<std::string, std::pair<MaterialInfo,Material>> json_database;
    Material_Resources() = default;
};

  
