#pragma once
#include"Event_Interpreter.h"
#include <string>
class Event_Int_Interpreter:public Event_InterpreterBase<int> {
    std::unordered_map<int, ConnectorVariant> connector;
public:
    ConnectorVariant GetInterpretConnector(int input) override {
        if (connector.find(input) != connector.end()) {
            return  connector[input] ;
        }
        return  EmptyConnector() ;
    };
    bool AddOne(int index, ConnectorVariant cv) {
        if (connector.find(index) == connector.end()) {
            connector[index] = cv;
            return true;
        }
        return false;
    }
    bool AddOne(int index) {
        if (connector.find(index) == connector.end()) {
            connector[index] = EmptyConnector();
            return true;
        }
        return false;
    }
    nlohmann::json Serialization() override {
        nlohmann::json j;
        for (auto it = connector.begin(); it != connector.end(); ++it) {
            std::visit([&j, it](auto&& arg) {  // 自动推导当前类型
                j.push_back({{ "Index",it->first }, { "Type", it->second.index() }, { "Data", arg.Serialization()}});
            }, it->second);
        }
        return j;
    };
    bool Deserialization(nlohmann::json j)override {
        try {
            for (const nlohmann::json& item : j) {
                connector[item["Index"].get<int>()] = InitCV(item["Type"], item["Data"]);
            }
        }
        catch (...) {
            return false;
        }
    };
};
