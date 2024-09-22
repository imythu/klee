#pragma once

#include "utils.h"
#include "RustExport.h"

#include <windows.h>
#include <set>
#include <vector>

#include<unordered_map>

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

    static LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            KeyMonitor& instance = KeyMonitor::getInstance();
            return instance.HandleKeyEvent(nCode, wParam, lParam);
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

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

        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    void StartHookKey() {
        std::cout << "Singleton instance called." << std::endl;

        HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, this->HookCallback, 0, 0);

        if (hhkLowLevelKybd == NULL) {
            return;
        }

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(hhkLowLevelKybd);
    }

    bool IsKeyInTag(const unsigned long long& tag, int code) {
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

    void register_callback(RustCallback& callback) {
        std::lock_guard<std::mutex> lock(m_cb_mutex);
        m_rust_cbs.push_back(callback);
    }

    void register_filter(const unsigned long long& item, RustCallback& callback) {
        std::lock_guard<std::mutex> lock(m_cb_mutex);
        m_filter_cbs[item].push_back(callback);
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
    virtual void Consume(const unsigned long long& item) {
        std::lock_guard<std::mutex> lock(m_cb_mutex);
        for (int i = 0; i < m_rust_cbs.size(); i++) {
            m_rust_cbs[i](item);
        }
        if (m_filter_cbs.find(item) != m_filter_cbs.end()) {
            std::vector<RustCallback>& cb_list = m_filter_cbs[item];
            for (int i = 0; i < cb_list.size(); i++) {
                cb_list[i](item);
            }
        }
    }

private:
    std::thread m_key_thread;
    std::set<DWORD> m_key_set; 
    std::vector<RustCallback> m_rust_cbs;
    std::mutex m_key_set_mutex;
    std::mutex m_cb_mutex;
    std::unordered_map<unsigned long long, std::vector<RustCallback>> m_filter_cbs;

};
