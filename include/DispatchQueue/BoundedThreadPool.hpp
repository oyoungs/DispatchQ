#pragma once

#include "TWorker.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>

namespace dispatch {

/**
 * @brief 有界线程池 - 队列大小受限的固定线程池
 * @details
 * 创建固定数量的工作线程，任务队列有最大长度限制。
 * - PushTask时如果队列满则等待
 * - 线程从公共队列获取任务执行
 * 
 * 特点:
 * - 线程数固定
 * - 队列有界，防止内存无限增长
 * - 生产者等待机制
 * 
 * @note 适用于需要控制并发数量和队列大小的场景
 */
class BoundedThreadPool final : public TWorker<BoundedThreadPool> {
public:
    /**
     * @brief 构造函数
     * @param threadCount 工作线程数量
     * @param maxQueueSize 任务队列最大长度
     */
    BoundedThreadPool(size_t threadCount, size_t maxQueueSize);
    ~BoundedThreadPool() override;

    /**
     * @brief 添加任务到队列（队列满时等待）
     * @param task 要执行的任务
     */
    void PushTask(Task task);

    /// 工作线程的事件循环
    void RunLoop();

    /// 停止所有线程
    void StopTask();

private:
    size_t threadCount_;            /// 工作线程数量
    size_t maxQueueSize_;           /// 队列最大长度
    std::queue<Task> queue_;        /// 公共任务队列
    std::mutex mutex_;              /// 互斥锁
    std::condition_variable cv_;    /// 条件变量
    std::atomic<bool> running_{false};  /// 运行标志
    std::vector<std::thread> threads_;  /// 工作线程数组
};

} // namespace dispatch