//
//  Object.cpp
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include <stdio.h>
#include <atomic>
#include "Prefix.h"

namespace ME {

template <class T>
class me_sp;

class Object {
    template<class T>
    friend class me_sp;
    
    ME_DISABLE_COPY_MOVE(Object);
protected:
    Object();
    virtual ~Object();
public:
    template<class T, typename... Args>
    static T* alloc(Args&&... args) {
        return Object::internal_alloc<T>(std::forward<Args>(args)...);
    }
    
    template<class T>
    static T* retain(T* obj) {
        return reinterpret_cast<T*>(Object::internal_retain(obj));
    }
    
    template<class T>
    static void release(T* obj) {
        Object::internal_release(obj);
    }
    
    template<class T>
    static T* assign(T* obj) {
        return Object::internal_assign(obj);
    }
    
private:
    template<class T, typename... Args>
    static T* internal_alloc(Args&&... args) {
        auto obj=new T(std::forward<Args>(args)...);
        if(obj) {
            internal_retain(obj);
        }
        return obj;
    }
    
    static Object* internal_retain(Object* obj);
    static void internal_release(Object* obj);
    static Object* internal_assign(Object* obj);
private:
    std::atomic_uint32_t m_RefCount=0;
};

#define ME_ALLOC(class,...) Object::alloc<class>(__VA_ARGS__)

#define ME_RETAIN(obj) Object::retain((obj))

#define ME_RELEASE(obj) Object::release((obj))


template<class T>
class me_sp final {
ME_DISABLE_NEW_DELETE();
public:
    constexpr me_sp() : m_Ptr(nullptr) {}
    constexpr me_sp(std::nullptr_t) : m_Ptr(nullptr) {}

    me_sp(const me_sp<T>& other):m_Ptr(Object::retain(other.m_Ptr)){}
    
    me_sp<T>& operator=(const me_sp<T> &other) {
        if(this != other) {
            Object::retain<T>(other.m_Ptr);
            Object::release<T>(m_Ptr);
            m_Ptr=other.m_Ptr;
        }
        return *this;
    }
    
    me_sp(me_sp<T> &&other):m_Ptr(other.m_Ptr){}
    
    me_sp<T>& operator=(me_sp<T> &&other) {
        if(this != other) {
            Object::release(m_Ptr);
            m_Ptr=other.m_Ptr;
        }
        return *this;
    }
  
    ~me_sp() {
        Object::release(m_Ptr);
    }
    
public:
    template<typename... Args>
    static me_sp<T> alloc(Args&&... args) {
        return me_sp(Object::alloc<T, Args&&...>(std::forward<Args>(args)...));
    }
    
    static me_sp<T> assign(T* obj){
        return Object::assign(obj);
    }
    
    static me_sp<T> retain(T* obj) {
        return me_sp(Object::retain(obj));
    }
    
    static void release(T* obj) {
        Object::release(obj);
    }
    
public:
    void retain() {
        Object::retain(m_Ptr);
    }
    void release() {
        Object::release(m_Ptr);
    }
    void assignment(T* obj) {
        if(obj!=m_Ptr) {
            Object::release(m_Ptr);
        }
        m_Ptr=Object::assign(obj);
    }
    
    void hold(T* obj) {
        Object::retain(obj);
        Object::release(m_Ptr);
        m_Ptr=obj;
    }
    
    void clear() {
        Object::retain(m_Ptr);
        m_Ptr=nullptr;
    }
    
    T* get() const {return reinterpret_cast<T*>(m_Ptr);}
    
    T* operator->() const {return reinterpret_cast<T*>(m_Ptr);}
    
    explicit operator bool() const {return m_Ptr!=nullptr;}
    
    bool isNull() const {return m_Ptr==nullptr;}
    
    bool isNotNull() const {return m_Ptr!=nullptr;}
    
private:
    me_sp(T* ptr):m_Ptr(ptr){}
private:
    Object* m_Ptr=nullptr;
};

#define ME_SP_ALLOC(class,...) me_sp<class>::alloc(__VA_ARGS__)

#define ME_SP_RETAIN(class,ptr,obj) me_sp<class> ptr=me_sp<class>::retain((obj))

#define ME_SP_RELEASE(obj) Object::release((obj))

}
