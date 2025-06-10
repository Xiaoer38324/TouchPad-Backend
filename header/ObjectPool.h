#pragma once
#include <mutex>
#include <queue>
#include <functional>
#include <unordered_set>
#include <memory>

template <typename T>
class ObjectPool {
public:
    // ���캯������ָ����ʼ��С�Ͷ��󴴽�����
    explicit ObjectPool(size_t initialSize = 0,
        std::function<T* ()> creator = nullptr)
        : creator_(creator ? creator : [] { return new T(); })
    {
        for (size_t i = 0; i < initialSize; ++i) {
            pool_.push(creator_());
        }
    }

    // �����������������ж���
    ~ObjectPool() {
        clear();
    }

    // ���ÿ����͸�ֵ
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    // �ӳ��л�ȡһ������
    T* acquire() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (pool_.empty()) {
            // ��Ϊ�գ������¶���
            T* newObj = creator_();
            active_.insert(newObj);
            return newObj;
        }

        // �ӳ���ȡ������
        T* ptr = pool_.front();
        pool_.pop();
        active_.insert(ptr);
        return ptr;
    }

    // �黹���󵽳���
    void release(T* ptr) {
        if (!ptr) return;

        std::unique_lock<std::mutex> lock(mutex_);
        if (active_.count(ptr)) {
            pool_.push(ptr);
            active_.erase(ptr);
        }
        else {
            // �Ǳ��ع����ָ�룬ֱ��ɾ����ֹ�ڴ�й©
            delete ptr;
        }
    }

    // ��ȡ��ǰ���п��ö�������
    size_t available() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return pool_.size();
    }

    // ��ȡ��ǰ����Ķ�������
    size_t borrowed() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return active_.size();
    }

    // ��ն���أ��������ж���
    void clear() {
        std::unique_lock<std::mutex> lock(mutex_);

        // ���ٳ��ж���
        while (!pool_.empty()) {
            delete pool_.front();
            pool_.pop();
        }

        // ���ٻ�Ծ����ǿ�ƻ��գ�
        for (T* ptr : active_) {
            delete ptr;
        }
        active_.clear();
    }

    // ǿ�ƹ黹���н���Ķ���
    void releaseAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        for (T* ptr : active_) {
            pool_.push(ptr);
        }
        active_.clear();
    }

private:
    std::queue<T*> pool_;
    std::unordered_set<T*> active_;  // �������н���Ķ���
    mutable std::mutex mutex_;
    std::function<T* ()> creator_;    // ���󴴽�����
};
