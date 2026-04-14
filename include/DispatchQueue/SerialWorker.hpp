#pragma once

#include "TWorker.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

namespace dispatch {

/**
 * @brief 串行Worker - 单线程任务执行器
 * @details
 * 只有一个工作线程，所有任务按提交顺序串行执行。
 * 适用于需要严格顺序执行的场景。
 * 
 * 特点:
 * - 单线程处理所有任务
 * - 任务FIFO顺序执行
 * - 线程安全
 */
class SerialWorker final : public TWorker<SerialWorker> {
public:
    /// 构造函数 - 启动工作线程
    SerialWorker();
    
    /// 析构函数 - 停止工作线程
    ~SerialWorker() override;

    /**
     * @brief 添加任务到队列
     * @param task 要执行的任务
     */
    void PushTask(Task task);

    /// 工作线程的事件循环
    void RunLoop();

    /// 停止工作线程
    void StopTask();

private:
    std::queue<Task> queue_;           /// 任务队列
    std::mutex mutex_;                  /// 互斥锁
    std::condition_variable cv_;         /// 条件变量
    std::atomic<bool> running_{false}; /// 运行标志
    std::thread thread_;                 /// 工作线程
};

} // namespace dispatch