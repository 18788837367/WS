#include "MEObject.h"
#include <iostream>



MEObject::MEObject()
{
    
}

MEObject::~MEObject()
{
}

MEObject* MEObject::internal_retain(MEObject* obj)
{
    if(obj) {
        obj->m_RefCount.fetch_add(1,std::memory_order_relaxed);
    }
    return obj;
}

void MEObject::internal_release(MEObject* obj)
{
    if(obj) {
        auto ov=obj->m_RefCount.fetch_sub(1, std::memory_order_acq_rel);
        if(ov<=1) {
            delete obj;
        }
    }
}

MEObject* MEObject::internal_assign(MEObject* obj)
{
    return obj;
}

