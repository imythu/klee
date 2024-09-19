#pragma once

#include "utils.h"
#include "predefine.h"

#include <windows.h>
#include <set>

class KeyMonitor : public ProducerConsumer<unsigned long long> {
private:
    // 私有构造函数，防止直接创建对象
    KeyMonitor(size_t maxcnt) : ProducerConsumer<unsigned long long>(maxcnt){
        std::cout << "Singleton instance created." << std::endl;
        m_key_thread = std::thread(&KeyMonitor::StartHookKey, this);
        m_key_thread.detach();
    }

public:
    // 禁止拷贝构造函数和赋值操作符，防止复制对象
    KeyMonitor(const KeyMonitor&) = delete;
    KeyMonitor& operator=(const KeyMonitor&) = delete;

    // 提供一个静态方法获取唯一实例
    static KeyMonitor& getInstance(size_t maxcnt = 10000) {
        static KeyMonitor instance(maxcnt);
        return instance;
    }

    // 静态的键盘钩子回调函数
    static LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            // 获取当前的 Singleton 实例
            KeyMonitor& instance = KeyMonitor::getInstance();

            // 将回调事件转发到非静态的成员函数进行处理
            return instance.HandleKeyEvent(nCode, wParam, lParam);
        }

        // 调用下一个钩子
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    // 非静态的成员函数，用于处理键盘事件
    LRESULT HandleKeyEvent(int nCode, WPARAM wParam, LPARAM lParam) {
        std::lock_guard<std::mutex> lock(m_key_set_mutex);
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vkCode = kbStruct->vkCode;
            if (m_key_set.find(vkCode) == m_key_set.end()) {
                m_key_set.insert(vkCode);
                this->Produce(GenKeyDownTag());
            }
        }
        else if (wParam == WM_KEYUP) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD vkCode = kbStruct->vkCode;
            if (m_key_set.find(vkCode) != m_key_set.end()) {
                m_key_set.erase(vkCode);
                this->Produce(GenKeyDownTag());
            }
        }

        // 继续传递钩子事件
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    void StartHookKey() {
        std::cout << "Singleton instance called." << std::endl;

        HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, this->HookCallback, 0, 0);

        if (hhkLowLevelKybd == NULL) {
            return;
        }

        // 消息循环
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 卸载钩子
        UnhookWindowsHookEx(hhkLowLevelKybd);
    }

    bool IsKeyInTag(const unsigned long long& tag, KeyCode code) {
        bool flag = false;
        for (int i = 0; i < 4; i++) {
            if (!((static_cast<unsigned long long>(code) << (8 * i))
                ^ ((static_cast<unsigned long long>(0xFF) << (8 * i)) & tag))) {
                flag = true;
                break;
            }
            
        }
        return flag;
    }

private:
    unsigned long long GenKeyDownTag() {
        unsigned long long res = 0;
        unsigned short cnt = 0;
        for (auto it = m_key_set.begin(); it != m_key_set.end(); ++it) {
            if (cnt++ >= 4) {
                break;
            }
            if (!(*it)) {
                continue;
            }
            res = (res << 8) + *it;
        }
        return res;
    }


protected:
    // 消费数据的虚函数，子类需要实现
    virtual void Consume(const unsigned long long& item) {
        /*if (IsKeyInTag(item, KEY_LCONTROL)) {
            std::cout << "Control in down !" << std::endl;
        }*/
    }

private:
    std::thread m_key_thread;
    std::set<DWORD> m_key_set; // 按键的集合
    std::mutex m_key_set_mutex;
};
