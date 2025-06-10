#pragma once
#include"Event_Int_Interpreter.h"
#include <set>
#include<iostream>
class Event_Range_Interpreter :public Event_InterpreterBase<int>
{
private:
    Event_Int_Interpreter eii;
    // ��������ṹ��
    struct Interval {
        int start;
        int end;
        int variant;
    };

    // �洢��ӵ�����
    std::vector<Interval> intervals_;
    // �Ƿ���ѭ������ģʽ
    bool cyclic_;
    // ѭ���������Сֵ�����ֵ������ 0 �� 360��
    int cyclic_min_, cyclic_max_;
    // ���䷶Χ�ĳ��ȣ���360-0=360
    int range_;

    // ����ֵ��һ���� [cyclic_min_, cyclic_max_) ��
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

    // �жϸ����Ĺ�һ����ֵ query �Ƿ�����������
    bool contains(const Interval& interval, int query) const {
        if (!cyclic_) {
            // ��ѭ������ʱ������ start <= end
            return (query >= interval.start && query <= interval.end);
        }
        else {
            // ����ѭ�����䣺
            // �������û�п�磨start <= end������ֱ���ж�
            if (interval.start <= interval.end) {
                return (query >= interval.start && query <= interval.end);
            }
            else {
                // ���������ٽ��ʱ������ (355, 10)
                // ���ѯ�������䣺[start, cyclic_max_) �� [cyclic_min_, end]
                return (query >= interval.start || query <= interval.end);
            }
        }
    }
public:
    Event_Range_Interpreter() : cyclic_(false), cyclic_min_(0), cyclic_max_(0), range_(0) {}

    // ����ѭ�����䣬������Ϊ(0, 360)��ע��Ҫ�� cyclic_max > cyclic_min
    void setCyclicRange(int cyclic_min, int cyclic_max) {
        if (cyclic_max <= cyclic_min) {
            std::cout << "cyclic_max�������cyclic_min" << std::endl;
            //��־
            return;
        }
        cyclic_ = true;
        cyclic_min_ = cyclic_min;
        cyclic_max_ = cyclic_max;
        range_ = cyclic_max_ - cyclic_min_; // ��(0,360) range_ Ϊ360
    }

    // ������䣬��������ѭ�����䣬�������˵���й�һ��
    // ���� (355, 370) ��[0,360)�лᱻת��Ϊ (355,10)
    void Add(int start, int end, const int& index,const ConnectorVariant& variant) {
        if (cyclic_) {
            start = normalize(start);
            end = normalize(end);
        }
        intervals_.push_back({ start, end, index });
        eii.AddOne(index,variant);
    }

    // ��������������һ������������������������� ConnectorVariant���������ص���
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
            //��־
            return false;
        }
        return true;
    };

  
};

