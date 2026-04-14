#pragma once

#include "Worker.hpp"
#include "DispatchQueueWorkerCreator.hpp"
#include <memory>

namespace dispatch {

/**
 * @brief 调度队列 - 统一的任务提交入口
 * @details
 * DispatchQueue 是调度队列的顶层接口，封装了不同策略的Worker实现。
 * 用户通过此接口提交任务，无需关心底层具体实现。
 * 
 * 使用工厂模式:
 * - 构造函数接收 DispatchStrategy 策略参数
 * - 通过 DispatchQueueWorkerCreator 单例创建对应的 Worker 实例
 * - 所有任务通过 Worker::Push 转发到底层实现
 */
class DispatchQueue {
public:
    /**
     * @brief 构造函数 - 根据策略创建对应的Worker
     * @param strategy 调度策略 (Serial/Concurrent/Stealing/Bounded)
     */
    explicit DispatchQueue(DispatchStrategy strategy);
    
    /// 析构函数 - 停止并销毁Worker
    ~DispatchQueue();

    /**
     * @brief 向队列提交任务
     * @param task 要执行的任务 (std::function<void()>)
     */
    void Push(Worker::Task task);

    /// 停止队列
    void Stop();

    /// 禁止拷贝
    DispatchQueue(const DispatchQueue&) = delete;
    DispatchQueue& operator=(const DispatchQueue&) = delete;

private:
    /// 底层的Worker实现
    std::unique_ptr<Worker> worker_;
};

} // namespace dispatch