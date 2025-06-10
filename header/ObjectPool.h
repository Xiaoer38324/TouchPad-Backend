#pragma once
#include <mutex>
#include <queue>
#include <functional>
#include <unordered_set>
#include <memory>

template <typename T>
class ObjectPool {
public:
    // 构造函数，可指定初始大小和对象创建函数
    explicit ObjectPool(size_t initialSize = 0,
        std::function<T* ()> creator = nullptr)
        : creator_(creator ? creator : [] { return new T(); })
    {
        for (size_t i = 0; i < initialSize; ++i) {
            pool_.push(creator_());
        }
    }

    // 析构函数：销毁所有对象
    ~ObjectPool() {
        clear();
    }

    // 禁用拷贝和赋值
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    // 从池中获取一个对象
    T* acquire() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (pool_.empty()) {
            // 池为空，创建新对象
            T* newObj = creator_();
            active_.insert(newObj);
            return newObj;
        }

        // 从池中取出对象
        T* ptr = pool_.front();
        pool_.pop();
        active_.insert(ptr);
        return ptr;
    }

    // 归还对象到池中
    void release(T* ptr) {
        if (!ptr) return;

        std::unique_lock<std::mutex> lock(mutex_);
        if (active_.count(ptr)) {
            pool_.push(ptr);
            active_.erase(ptr);
        }
        else {
            // 非本池管理的指针，直接删除防止内存泄漏
            delete ptr;
        }
    }

    // 获取当前池中可用对象数量
    size_t available() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return pool_.size();
    }

    // 获取当前借出的对象数量
    size_t borrowed() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return active_.size();
    }

    // 清空对象池（销毁所有对象）
    void clear() {
        std::unique_lock<std::mutex> lock(mutex_);

        // 销毁池中对象
        while (!pool_.empty()) {
            delete pool_.front();
            pool_.pop();
        }

        // 销毁活跃对象（强制回收）
        for (T* ptr : active_) {
            delete ptr;
        }
        active_.clear();
    }

    // 强制归还所有借出的对象
    void releaseAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        for (T* ptr : active_) {
            pool_.push(ptr);
        }
        active_.clear();
    }

private:
    std::queue<T*> pool_;
    std::unordered_set<T*> active_;  // 跟踪所有借出的对象
    mutable std::mutex mutex_;
    std::function<T* ()> creator_;    // 对象创建函数
};
