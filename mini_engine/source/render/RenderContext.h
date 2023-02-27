#pragma once

#include "Object.h"

namespace ME {

class RenderContext : public Object {
public:
    virtual void loadProgram()=0;
    
    
    virtual void newStaticBuffer()=0;
    virtual void newSharedBuffer()=0;
    
    virtual void performDraw()=0;
    virtual void performClear()=0;
    
    
    
};

}
