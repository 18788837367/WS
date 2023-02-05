//
//  MERenderBuffer.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderBuffer_h
#define MERenderBuffer_h
#include "MERenderResource.h"
#include <memory.h>

class MERenderBuffer : public MERenderResource {
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
    MERenderBuffer(Type type, void* data, size_t size, size_t offset);
    virtual ~MERenderBuffer() override;
private:
    Type m_Type=Type::Vertex;
    void* m_Data=nullptr;
    size_t m_Size=0;
    size_t m_Offset=0;
};

class MERenderStaticBuffer : public MERenderBuffer {
public:
//    MERenderStaticBuffer(Type type, const void* data, size_t size, size_t offset, );
    
    
    
};

class MERenderSharedBuffer : public MERenderBuffer {
    
};





#endif /* MERenderBuffer_h */
