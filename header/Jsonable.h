#pragma once
#include <string>
#include "nlohmann/json.hpp"
class Jsonable {
    //���л��뷴���л��ӿڡ�

public:
    virtual nlohmann::json Serialization() = 0;
    virtual bool Deserialization(nlohmann::json value) = 0;
    virtual ~Jsonable()
    {

    }
   template<typename  T>
   inline T TryOrDefault(const nlohmann::json& value, const std::string& name,T default_val) {
       if (value.contains(name)) {
           return value[name].get<T>();
        }
       return default_val;
    }
};
