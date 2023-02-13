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
    void renderDraw();
    
public:
    void performDraw() override;
    void performClear() override;
    
    void newStaticBuffer() override;
    void newSharedBuffer() override;
};

#endif /* MERenderDeviceContextOpenGL_h */
