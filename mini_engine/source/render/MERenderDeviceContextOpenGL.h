//
//  MERenderDeviceContextOpenGL.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderDeviceContextOpenGL_h
#define MERenderDeviceContextOpenGL_h

#include "MERenderDeviceContext.h"

class MERenderDeviceContextOpenGL : public MERenderDeviceContext {
private:
    
protected:
    void renderDraw() override;
    
public:
    void performDraw();
    void performClear();
    
    void newStaticBuffer();
    void newSharedBuffer();
};

#endif /* MERenderDeviceContextOpenGL_h */
