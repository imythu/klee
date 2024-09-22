#include "RustExport.h"
#include "winkeyhook.h"

void __stdcall register_callback(RustCallback callback)
{
	KeyMonitor::getInstance().register_callback(callback);
}

void __stdcall register_filter(const unsigned long long tag, RustCallback callback)
{
	KeyMonitor::getInstance().register_filter(tag, callback);
}

bool __stdcall iskeyintag(const unsigned long long tag, const int key)
{
	return KeyMonitor::getInstance().IsKeyInTag(tag, key);
}


