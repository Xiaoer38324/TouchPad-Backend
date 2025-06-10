#pragma once
#include"Event_Int_Interpreter.h"
#include <set>
#include<iostream>
class Event_Range_Interpreter :public Event_InterpreterBase<int>
{
private:
    Event_Int_Interpreter eii;
    // 定义区间结构体
    struct Interval {
        int start;
        int end;
        int variant;
    };

    // 存储添加的区间
    std::vector<Interval> intervals_;
    // 是否处于循环区间模式
    bool cyclic_;
    // 循环区间的最小值与最大值（例如 0 和 360）
    int cyclic_min_, cyclic_max_;
    // 区间范围的长度，如360-0=360
    int range_;

    // 将数值归一化到 [cyclic_min_, cyclic_max_) 内
    int normalize(int val) const {
        if (!cyclic_) {
            return val;
        }
        int modVal = (val - cyclic_min_) % range_;
        if (modVal < 0) {
            modVal += range_;
        }
        return cyclic_min_ + modVal;
    }

    // 判断给定的归一化数值 query 是否落在区间内
    bool contains(const Interval& interval, int query) const {
        if (!cyclic_) {
            // 非循环区间时，假设 start <= end
            return (query >= interval.start && query <= interval.end);
        }
        else {
            // 处理循环区间：
            // 如果区间没有跨界（start <= end），则直接判断
            if (interval.start <= interval.end) {
                return (query >= interval.start && query <= interval.end);
            }
            else {
                // 当区间跨过临界点时，比如 (355, 10)
                // 则查询落在区间：[start, cyclic_max_) 或 [cyclic_min_, end]
                return (query >= interval.start || query <= interval.end);
            }
        }
    }
public:
    Event_Range_Interpreter() : cyclic_(false), cyclic_min_(0), cyclic_max_(0), range_(0) {}

    // 设置循环区间，如设置为(0, 360)，注意要求 cyclic_max > cyclic_min
    void setCyclicRange(int cyclic_min, int cyclic_max) {
        if (cyclic_max <= cyclic_min) {
            std::cout << "cyclic_max必须大于cyclic_min" << std::endl;
            //日志
            return;
        }
        cyclic_ = true;
        cyclic_min_ = cyclic_min;
        cyclic_max_ = cyclic_max;
        range_ = cyclic_max_ - cyclic_min_; // 如(0,360) range_ 为360
    }

    // 添加区间，若已设置循环区间，则对区间端点进行归一化
    // 例如 (355, 370) 在[0,360)中会被转换为 (355,10)
    void Add(int start, int end, const int& index,const ConnectorVariant& variant) {
        if (cyclic_) {
            start = normalize(start);
            end = normalize(end);
        }
        intervals_.push_back({ start, end, index });
        eii.AddOne(index,variant);
    }

    // 检索方法：输入一个整数，返回所有所在区间的 ConnectorVariant（可以有重叠）
    void GetRangeConnector(int query, std::set<int>& new_set) {
        int normalizedQuery = query;
        if (cyclic_) {
            normalizedQuery = normalize(query);
        }
        new_set.clear();
        for (const auto& interval : intervals_) {
            if (contains(interval, normalizedQuery)) {
                new_set.insert(interval.variant);
            }
        }
    }
    ConnectorVariant GetInterpretConnector(int index) override {
        return eii.GetInterpretConnector(index);
    }
    nlohmann::json Serialization() override {
        nlohmann::json j;
        j["cyclic"] = cyclic_;
        j["cyclic_min"] = cyclic_min_;
        j["cyclic_max"] = cyclic_max_;
        j["intervals"] = nlohmann::json::array();
        for (const auto& interval : intervals_) {
            nlohmann::json j_interval;
            j_interval["start"] = interval.start;
            j_interval["end"] = interval.end;
            j_interval["index"] = interval.variant;
            j["intervals"].push_back(j_interval);
        }
        j["eii"] = eii.Serialization();
        return j;
    };
    bool Deserialization(nlohmann::json j)override {
        try {
            if (!eii.Deserialization(j["eii"]))
            {
                return false;
            }
            cyclic_ = j.at("cyclic").get<bool>();
            cyclic_min_ = j.at("cyclic_min").get<int>();
            cyclic_max_ = j.at("cyclic_max").get<int>();
            if (cyclic_) {
                range_ = cyclic_max_ - cyclic_min_;
            }
            else {
                range_ = 0;
            }
            intervals_.clear();
            for (const auto& j_interval : j.at("intervals")) {
                Interval interval;
                interval.start = j_interval.at("start").get<int>();
                interval.end = j_interval.at("end").get<int>();
                interval.variant= j_interval.at("index").get<int>();
                intervals_.push_back(interval);
            }
        }
        catch (const std::exception& e) {
            std::cout << "Deserialization error: " << e.what() << std::endl;
            //日志
            return false;
        }
        return true;
    };

  
};

