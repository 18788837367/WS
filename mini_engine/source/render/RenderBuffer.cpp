#include "RenderBuffer.h"

namespace ME {

RenderBuffer::RenderBuffer(Type type, void* data, size_t size, size_t offset)
:RenderResource(), m_Type(type), m_Data(data), m_Size(size), m_Offset(offset)
{
    
}

RenderBuffer::~RenderBuffer()
{
    
}

}
