#pragma once

#include "RenderResource.h"
#include <memory.h>

namespace ME {

class RenderBuffer : public RenderResource {
public:
    enum class Type {
        Vertex,
        Index,
        _Count,
    };
    inline Type getType() const {
        return m_Type;
    }
    inline size_t getSize() const {
        return m_Size;
    }
    inline size_t getOffset() const {
        return m_Offset;
    }
protected:
    RenderBuffer(Type type, void* data, size_t size, size_t offset);
    virtual ~RenderBuffer() override;
private:
    Type m_Type=Type::Vertex;
    void* m_Data=nullptr;
    size_t m_Size=0;
    size_t m_Offset=0;
};

class RenderStaticBuffer : public RenderBuffer {
public:
//    RenderStaticBuffer(Type type, const void* data, size_t size, size_t offset, );
    
    
    
};

class RenderSharedBuffer : public RenderBuffer {
    
};



}
