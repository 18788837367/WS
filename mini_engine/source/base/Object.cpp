#include "Object.h"
#include <iostream>

namespace ME {

Object::Object()
{
    
}

Object::~Object()
{
}

Object* Object::internal_retain(Object* obj)
{
    if(obj) {
        obj->m_RefCount.fetch_add(1,std::memory_order_relaxed);
    }
    return obj;
}

void Object::internal_release(Object* obj)
{
    if(obj) {
        auto ov=obj->m_RefCount.fetch_sub(1, std::memory_order_acq_rel);
        if(ov<=1) {
            delete obj;
        }
    }
}

Object* Object::internal_assign(Object* obj)
{
    return obj;
}

}
