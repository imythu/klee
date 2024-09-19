#pragma once

#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

// ==================================== ��������ģ�� Start ========================================= //

// ����࣬���������߽ӿ�
class IPCInter
{
public:
    virtual void Consumer() = 0;
};

// ������-�������߳��࣬������������
class PCThread
{
public:
    explicit PCThread(IPCInter* pc) : m_pc(pc), m_stop(false) {}

    ~PCThread() {
        Stop();
    }

    // �߳�������
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

// ������-��������ģ��
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
            cv.notify_all();  // ֪ͨ������ֹͣ
            m_pc_thread->Stop();
            delete m_pc_thread;
        }
    }

    // �����ߺ���
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

    // �����ߺ���
    void Produce(const T& item) {
        std::unique_lock<std::mutex> lock(m_pc_cs);

        if (m_info_list.size() < max_count) {
            m_info_list.push_back(item);
            cv.notify_one();  // ֪ͨ�������������ݿ�����
        }
        else {
            std::cerr << "Buffer is full, item discarded." << std::endl;
        }
    }

protected:
    // �������ݵ��麯����������Ҫʵ��
    virtual void Consume(const T& item) = 0;

private:
    std::list<T> m_info_list;           // ���ڴ洢������Ԫ��
    std::mutex m_pc_cs;                 // ���ڱ������еĻ�����
    std::condition_variable cv;         // ���������������߳�ͬ��
    PCThread* m_pc_thread;              // ������-�������߳�
    const size_t max_count;             // ��󻺳�����С
    std::atomic<bool> stop;             // ֹͣ��־
};

// ==================================== ��������ģ�� End ========================================= //

