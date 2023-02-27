#pragma once

#include "RenderContext.h"

namespace ME {

class RenderDeviceContext : public RenderContext {
private:
    virtual void loadProgram();

protected:
    void renderStart();
    bool renderUpdate();
    void renderDraw();
    
};

}
