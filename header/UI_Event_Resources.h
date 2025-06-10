#pragma once
#include <unordered_map>
#include <string>
#include"Jsonable.h"
#include "nlohmann/json.hpp"
class UI_Event_Resources:public Jsonable//单例，用于UI数据的拉数据的。
{
public:
    static UI_Event_Resources& getInstance() {
        static UI_Event_Resources instance;  // C++11保证静态局部变量初始化线程安全
        return instance;
    }
    //TODO 填数据，拉数据，清空内存数据。
    bool Add(std::string name,nlohmann::json json,bool cover=false) {
        if (cover) {
            json_database[name] = json;
            return true;
        }
        if (json_database.find(name)==json_database.end()) {
            json_database[name] = json;
            return true;
        }
        return false;
    }
    nlohmann::json Get(std::string name) {
        nlohmann::json j;
        if (json_database.find(name) != json_database.end()) {
            if (json_database[name] != nullptr) {
                j = json_database[name];
            }
        }
        
        return j;
    }
    nlohmann::json Serialization() override {
        nlohmann::json j = {
            {"UIData",json_database}
        };
        return j;
    }
    bool Deserialization(nlohmann::json j) override {
        try {
            json_database = j["UIData"].get<std::unordered_map<std::string, nlohmann::json>>();
        }
        catch (...) {
            return false;
        }
        return true;
    }
    void Clear() {
        json_database.clear();
    }
private:
    ~UI_Event_Resources() = default;
   
    std::unordered_map<std::string, nlohmann::json> json_database;
    UI_Event_Resources() = default;
};

