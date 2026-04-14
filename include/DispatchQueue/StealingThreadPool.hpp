#pragma once

#include "TWorker.hpp"
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>

namespace dispatch {

/**
 * @brief 工作窃取线程池 - 每个线程拥有独立任务队列
 * @details
 * 每个工作线程拥有自己的本地任务队列(LIFO)，空闲时从其他线程队列头部窃取任务(FIFO)。
 * 
 * 特点:
 * - 减少锁竞争：每个线程操作自己的队列只需加锁
 * - 工作窃取：空闲线程从其他线程队列获取任务
 * - 负载均衡：自动平衡各线程的工作量
 * 
 * @note 适用于任务执行时间差异较大的场景
 */
class StealingThreadPool final : public TWorker<StealingThreadPool> {
public:
    /**
     * @brief 构造函数
     * @param threadCount 工作线程数量（默认4）
     */
    explicit StealingThreadPool(size_t threadCount = 4);
    ~StealingThreadPool() override;

    /**
     * @brief 添加任务到队列（轮询分配）
     * @param task 要执行的任务
     */
    void PushTask(Task task);

    /// 工作线程的事件循环
    void RunLoop();

    /// 停止所有线程
    void StopTask();

private:
    /// 本地队列结构
    struct LocalQueue {
        std::deque<Task> queue;   /// 双端队列
        std::mutex mutex;         /// 互斥锁
    };

    /// 创建所有工作线程
    void CreateWorkerThreads();

    /**
     * @brief 尝试从其他线程队列窃取任务
     * @param task 输出参数，窃取到的任务
     * @return bool 是否成功窃取到任务
     */
    bool TrySteal(Task& task);

    /// 当前线程的本地队列索引（thread_local）
    static thread_local size_t localQueueIdx_;

    size_t threadCount_;                    /// 线程数量
    std::vector<LocalQueue> queues_;        /// 本地队列数组
    std::vector<std::thread> threads_;     /// 工作线程数组
    std::atomic<bool> running_{false};     /// 运行标志
    std::atomic<size_t> roundRobin_{0};     /// 轮询索引
};

} // namespace dispatch