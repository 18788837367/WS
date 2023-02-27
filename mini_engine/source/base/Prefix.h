#pragma once


#if !defined(ME_OS_IOS) && !defined(ME_OS_ANDROID) && !defined(ME_OS_MACOS) && !defined(ME_OS_WINDOWS)

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if defined(_WIN32) || defined(__SYMBIAN32__)
    #define ME_OS_WINDOWS
#elif defined(ANDROID) || defined(__ANDROID__)
    #define ME_OS_ANDROID
#elif TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #define ME_OS_IOS
#else
    #define ME_OS_MACOS
#endif

#endif






#define ME_DISABLE_COPY(Class) \
private: \
    Class(const Class&)=delete; \
    Class& operator=(const Class&)=delete;

#define ME_DISABLE_COPY_MOVE(Class) \
    ME_DISABLE_COPY(Class); \
    Class(Class &&)=delete; \
    Class& operator=(Class &&)=delete;

#define ME_DISABLE_NEW_DELETE() \
private: \
    void* operator new(size_t size)=delete; \
    void operator delete(void* p)=delete; \
    void* operator new[](size_t size)=delete; \
    void operator delete[](void* p)=delete;
