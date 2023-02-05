//
//  MERenderPlatformDeviceContextOpenGL.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderPlatformDeviceContextOpenGL_h
#define MERenderPlatformDeviceContextOpenGL_h

#include "MERenderDeviceContextOpenGL.h"

class MERenderPlatformDeviceContextOpenGL : public MERenderDeviceContextOpenGL {

protected:
    void renderStart();
    bool renderUpdate();
    void renderDraw();
    
};

#endif /* MERenderPlatformDeviceContextOpenGL_h */
