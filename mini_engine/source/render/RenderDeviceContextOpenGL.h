#pragma once
#include "RenderDeviceContext.h"

namespace ME {

class RenderDeviceContextOpenGL : public RenderDeviceContext {
private:
    
protected:
    void renderDraw();
    
public:
    void performDraw() override;
    void performClear() override;
    
    void newStaticBuffer() override;
    void newSharedBuffer() override;
};

}
