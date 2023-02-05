//
//  MERenderDeviceContext.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderDeviceContext_h
#define MERenderDeviceContext_h

#include "MERenderContext.h"

class MERenderDeviceContext : public MERenderContext {
private:
    virtual void loadProgram();

protected:
    void renderStart();
    bool renderUpdate();
    virtual void renderDraw();
    
};

#endif /* MERenderDeviceContext_h */
