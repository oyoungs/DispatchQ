#pragma once

#include "Worker.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

namespace dispatch {

/**
 * @brief 调度策略枚举
 * @details 定义了四种可用的调度策略:
 * - Serial: 串行执行，单线程
 * - Concurrent: 动态线程池，线程数可动态增减
 * - Stealing: 工作窃取线程池，每个线程有独立队列
 * - Bounded: 有界线程池，队列有最大任务数限制
 */
enum class DispatchStrategy {
    Serial,      /// 串行队列
    Concurrent,  /// 动态线程池
    Stealing,    /// 工作窃取线程池
    Bounded      /// 有界线程池
};

/**
 * @brief Worker创建器 - 单例工厂类
 * @details
 * 负责注册和创建不同策略的Worker实例。
 * 使用单例模式确保全局唯一性。
 * 
 * 设计模式: Singleton + Factory
 * - 构造函数中自动注册默认的Worker实现
 * - 支持运行时动态注册新的策略
 */
class DispatchQueueWorkerCreator {
public:
    /// Worker创建函数类型
    using WorkerCreator = std::function<std::unique_ptr<Worker>()>;

    /**
     * @brief 获取单例实例
     * @return DispatchQueueWorkerCreator& 全局唯一实例
     */
    static DispatchQueueWorkerCreator& Instance() {
        static DispatchQueueWorkerCreator instance;
        return instance;
    }

    /**
     * @brief 注册Worker创建函数（按名称）
     * @param name 策略名称
     * @param creator 创建函数
     */
    void Register(const std::string& name, WorkerCreator creator) {
        creators_[name] = std::move(creator);
    }

    /**
     * @brief 注册Worker创建函数（按策略枚举）
     * @param strategy 调度策略
     * @param creator 创建函数
     */
    void Register(DispatchStrategy strategy, WorkerCreator creator) {
        auto name = StrategyToString(strategy);
        creators_[name] = std::move(creator);
    }

    /**
     * @brief 创建Worker实例（按名称）
     * @param name 策略名称
     * @return std::unique_ptr<Worker> 创建的Worker实例，未注册返回nullptr
     */
    std::unique_ptr<Worker> Create(const std::string& name) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second();
        }
        return nullptr;
    }

    /**
     * @brief 创建Worker实例（按策略枚举）
     * @param strategy 调度策略
     * @return std::unique_ptr<Worker> 创建的Worker实例
     */
    std::unique_ptr<Worker> Create(DispatchStrategy strategy) {
        return Create(StrategyToString(strategy));
    }

    /**
     * @brief 检查策略是否存在（按名称）
     * @param name 策略名称
     * @return bool 是否已注册
     */
    bool HasStrategy(const std::string& name) const {
        return creators_.find(name) != creators_.end();
    }

    /**
     * @brief 检查策略是否存在（按枚举）
     * @param strategy 调度策略
     * @return bool 是否已注册
     */
    bool HasStrategy(DispatchStrategy strategy) const {
        return HasStrategy(StrategyToString(strategy));
    }

private:
    /// 构造函数 - 在此注册默认的Worker实现
    DispatchQueueWorkerCreator();

    /// 策略枚举转字符串
    static std::string StrategyToString(DispatchStrategy strategy) {
        switch (strategy) {
            case DispatchStrategy::Serial: return "serial";
            case DispatchStrategy::Concurrent: return "concurrent";
            case DispatchStrategy::Stealing: return "stealing";
            case DispatchStrategy::Bounded: return "bounded";
        }
        return "unknown";
    }

    /// 策略名称到创建函数的映射
    std::unordered_map<std::string, WorkerCreator> creators_;
};

} // namespace dispatch