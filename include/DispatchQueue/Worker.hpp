#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace dispatch {

/**
 * @brief 抽象Worker基类 - 任务执行的接口定义
 * @details
 * Worker是任务调度的抽象基类，定义了任务队列的统一接口。
 * 所有具体的Worker实现必须继承此类并实现相关方法。
 * 
 * 设计模式: Template Method
 * - Push/Run/Stop 由 TWorker<Derived> 以 final 形式实现
 * - 子类只需实现 PushTask/RunLoop/StopTask
 */
class Worker {
public:
    /// 任务类型: 无返回值无参数的可调用对象
    using Task = std::function<void()>;

    virtual ~Worker() = default;

    /**
     * @brief 向工作队列中添加一个任务
     * @param task 要执行的任务（std::function<void()>）
     * @note 纯虚函数，由 TWorker<Derived>::Push() final 调用子类的 PushTask()
     */
    virtual void Push(Task task) = 0;

    /**
     * @brief 启动工作线程的事件循环
     * @note 纯虚函数，由 TWorker<Derived>::Run() final 调用子类的 RunLoop()
     *       此方法会阻塞直到 worker 被停止
     */
    virtual void Run() = 0;

    /**
     * @brief 停止Worker，唤醒所有等待的线程
     * @note 纯虚函数，由 TWorker<Derived>::Stop() final 调用子类的 StopTask()
     */
    virtual void Stop() = 0;

protected:
    /// 默认构造函数
    Worker() = default;
    
    /// 禁止拷贝
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;
};

} // namespace dispatch