//
//  MERenderBase.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderBase_h
#define MERenderBase_h
#include "MEObject.h"

class MERenderBase : public MEObject {
public:
    static me_sp<MERenderBase> create();
    
public:
    virtual void pause()=0;
    virtual void resume()=0;
    virtual void stop()=0;
    
    
    
    
};




#endif /* MERenderBase_h */
