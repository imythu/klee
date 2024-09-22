#pragma once

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

// ==================================== 生产消费模型 Start ========================================= //

// 虚基类，用于消费者接口
class IPCInter
{
public:
    virtual void Consumer() = 0;
};

// 生产者-消费者线程类，负责消费数据
class PCThread
{
public:
    explicit PCThread(IPCInter* pc) : m_pc(pc), m_stop(false) {}

    ~PCThread() {
        Stop();
    }

    // 线程主函数
    void Main() {
        while (!m_stop) {
            if (m_pc) {
                m_pc->Consumer();
            }
        }
    }

    void Start() {
        m_thread = std::thread(&PCThread::Main, this);
    }

    void Stop() {
        m_stop = true;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    bool IsStopping() const {
        return m_stop;
    }

private:
    IPCInter* m_pc;
    std::thread m_thread;
    std::atomic<bool> m_stop;
};

// 生产者-消费者类模板
template <typename T>
class ProducerConsumer : public IPCInter
{
public:
    explicit ProducerConsumer(size_t maxcnt) : max_count(maxcnt), stop(false) {
        m_pc_thread = new PCThread(this);
        if (m_pc_thread) {
            m_pc_thread->Start();
        }
    }

    ~ProducerConsumer() {
        if (m_pc_thread) {
            stop = true;
            cv.notify_all();  // 通知消费者停止
            m_pc_thread->Stop();
            delete m_pc_thread;
        }
    }

    // 消费者函数
    virtual void Consumer() override {
        std::unique_lock<std::mutex> lock(m_pc_cs);
        cv.wait(lock, [this]() { return !m_info_list.empty() || stop; });

        if (!m_info_list.empty()) {
            T item = m_info_list.front();
            m_info_list.pop_front();
            lock.unlock();

            Consume(item);
        }
    }

    // 生产者函数
    void Produce(const T& item) {
        std::unique_lock<std::mutex> lock(m_pc_cs);

        if (m_info_list.size() < max_count) {
            m_info_list.push_back(item);
            cv.notify_one();  // 通知消费者有新数据可消费
        }
        else {
            std::cerr << "Buffer is full, item discarded." << std::endl;
        }
    }

protected:
    // 消费数据的虚函数，子类需要实现
    virtual void Consume(const T& item) = 0;

private:
    std::list<T> m_info_list;           // 用于存储生产的元素
    std::mutex m_pc_cs;                 // 用于保护队列的互斥锁
    std::condition_variable cv;         // 条件变量，用于线程同步
    PCThread* m_pc_thread;              // 生产者-消费者线程
    const size_t max_count;             // 最大缓冲区大小
    std::atomic<bool> stop;             // 停止标志
};

// ==================================== 生产消费模型 End ========================================= //

