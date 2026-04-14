#pragma once

#include "TWorker.hpp"
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>

namespace dispatch {

/**
 * @brief 动态线程池 - 线程数可动态调整的线程池
 * @details
 * 线程数在[minThreads, maxThreads]范围内动态调整。
 * - 初始创建minThreads个工作线程
 * - 任务繁忙时动态创建新线程（直到maxThreads）
 * - 空闲超时后回收多余线程
 * 
 * 特点:
 * - 线程数动态伸缩
 * - 空闲超时自动回收
 * - thread_local标记可回收线程
 */
class DynamicThreadPool final : public TWorker<DynamicThreadPool> {
public:
    /**
     * @brief 构造函数
     * @param minThreads 最小线程数
     * @param maxThreads 最大线程数
     * @param idleTimeout 空闲超时时间（默认10秒）
     */
    DynamicThreadPool(size_t minThreads, size_t maxThreads, 
                      std::chrono::seconds idleTimeout = std::chrono::seconds(10));
    ~DynamicThreadPool() override;

    /**
     * @brief 添加任务到队列
     * @param task 要执行的任务
     */
    void PushTask(Task task);

    /// 工作线程的事件循环
    void RunLoop();

    /// 停止所有线程
    void StopTask();

    /**
     * @brief 设置空闲超时时间
     * @param timeout 新的超时时间
     */
    void SetIdleTimeout(std::chrono::seconds timeout);

private:
    /**
     * @brief 创建新线程
     * @param recyclable 线程是否可回收
     */
    void CreateThread(bool recyclable);

    /// 收集并清理已结束的线程
    void CollectDeadThreads();

    /// 当前线程是否可回收（thread_local）
    static thread_local bool recyclable_;
    /// 当前线程对象指针（thread_local）
    static thread_local std::thread* currentThread_;

    size_t minThreads_;           /// 最小线程数
    size_t maxThreads_;           /// 最大线程数
    std::chrono::seconds idleTimeout_;  /// 空闲超时时间

    std::queue<Task> queue_;      /// 公共任务队列
    std::mutex mutex_;            /// 互斥锁
    std::condition_variable cv_;  /// 条件变量

    std::atomic<bool> running_{false};   /// 运行标志
    std::atomic<size_t> totalThreads_{0};   /// 当前总线程数
    std::atomic<size_t> idleCount_{0};       /// 空闲线程数

    std::vector<std::thread> threads_;  /// 线程存储
};

} // namespace dispatch