#pragma once

#include "Object.h"

namespace ME {

class RenderBase : public Object {
public:
    static me_sp<RenderBase> create();
    
public:
    virtual void pause()=0;
    virtual void resume()=0;
    virtual void stop()=0;
    
    
    
    
};

}


