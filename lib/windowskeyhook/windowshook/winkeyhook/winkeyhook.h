#pragma once

#include "utils.h"
#include "predefine.h"

#include <windows.h>
#include <set>

class KeyMonitor : public ProducerConsumer<unsigned long long> {
private:
    // ˽�й��캯������ֱֹ�Ӵ�������
    KeyMonitor(size_t maxcnt) : ProducerConsumer<unsigned long long>(maxcnt){
        std::cout << "Singleton instance created." << std::endl;
        m_key_thread = std::thread(&KeyMonitor::StartHookKey, this);
        m_key_thread.detach();
    }

public:
    // ��ֹ�������캯���͸�ֵ����������ֹ���ƶ���
    KeyMonitor(const KeyMonitor&) = delete;
    KeyMonitor& operator=(const KeyMonitor&) = delete;

    // �ṩһ����̬������ȡΨһʵ��
    static KeyMonitor& getInstance(size_t maxcnt = 10000) {
        static KeyMonitor instance(maxcnt);
        return instance;
    }

    // ��̬�ļ��̹��ӻص�����
    static LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            // ��ȡ��ǰ�� Singleton ʵ��
            KeyMonitor& instance = KeyMonitor::getInstance();

            // ���ص��¼�ת�����Ǿ�̬�ĳ�Ա�������д���
            return instance.HandleKeyEvent(nCode, wParam, lParam);
        }

        // ������һ������
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    // �Ǿ�̬�ĳ�Ա���������ڴ�������¼�
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

        // �������ݹ����¼�
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    void StartHookKey() {
        std::cout << "Singleton instance called." << std::endl;

        HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, this->HookCallback, 0, 0);

        if (hhkLowLevelKybd == NULL) {
            return;
        }

        // ��Ϣѭ��
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // ж�ع���
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
    // �������ݵ��麯����������Ҫʵ��
    virtual void Consume(const unsigned long long& item) {
        /*if (IsKeyInTag(item, KEY_LCONTROL)) {
            std::cout << "Control in down !" << std::endl;
        }*/
    }

private:
    std::thread m_key_thread;
    std::set<DWORD> m_key_set; // �����ļ���
    std::mutex m_key_set_mutex;
};
