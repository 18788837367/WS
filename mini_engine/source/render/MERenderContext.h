//
//  MERenderContext.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderContext_h
#define MERenderContext_h

#include "MEObject.h"

class MERenderContext : public MEObject {
public:
    virtual void loadProgram()=0;
    
    
    virtual void newStaticBuffer()=0;
    virtual void newSharedBuffer()=0;
    
    virtual void performDraw()=0;
    virtual void performClear()=0;
    
    
    
};

#endif /* MERenderContext_h */
