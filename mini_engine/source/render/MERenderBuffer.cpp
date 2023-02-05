//
//  MERenderBuffer.cpp
//  WS
//
//  Created by zrf on 2023/1/29.
//

#include "MERenderBuffer.h"

MERenderBuffer::MERenderBuffer(Type type, void* data, size_t size, size_t offset)
:MERenderResource(), m_Type(type), m_Data(data), m_Size(size), m_Offset(offset)
{
    
}

MERenderBuffer::~MERenderBuffer()
{
    
}

