#pragma once
#include "RenderDeviceContextOpenGL.h"

namespace ME {

class RenderPlatformDeviceContextOpenGL : public RenderDeviceContextOpenGL {

protected:
    void renderStart();
    bool renderUpdate();
    void renderDraw();
    
};


}
