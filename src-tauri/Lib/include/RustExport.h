#pragma once

extern "C" {
    // 定义一个函数指针类型，接受一个整数参数，返回一个整数
    typedef unsigned short (*RustCallback)(unsigned long long tag);
    // 导出一个函数，用于注册回调函数
    __declspec(dllexport) void __stdcall register_callback(RustCallback callback);
    __declspec(dllexport) void __stdcall register_filter(const unsigned long long tag, RustCallback callback);
    __declspec(dllexport) bool __stdcall iskeyintag(const unsigned long long tag, const int key);

}

