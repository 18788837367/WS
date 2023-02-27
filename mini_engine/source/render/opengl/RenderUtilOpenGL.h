#pragma once

#include "Object.h"
#include <functional>
#include <unordered_set>

#if defined(ME_OS_MACOS)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

namespace ME {
class RenderOpenGLUtil final {
public:
    static void makeCurrentContext(void* context);
    
    
};

class RenderOpenGLContext;
class RenderOpenGLObj : public Object {
public:
    RenderOpenGLObj(RenderOpenGLContext* context);
    virtual ~RenderOpenGLObj()=0;
    void onUnMount();
private:
    RenderOpenGLContext* m_Context=nullptr;
};

class RenderOpenGLObjProgram : public RenderOpenGLObj {
public:
    RenderOpenGLObjProgram(RenderOpenGLContext* context);
    virtual ~RenderOpenGLObjProgram();
    
private:
  
};


class RenderOpenGLContext : public Object {
public:
    RenderOpenGLContext(void* context);
    ~RenderOpenGLContext();
    
    void makeCurrent();
    me_sp<RenderOpenGLObj> createObject(const std::function<me_sp<RenderOpenGLObj>(Object* context)>& createor);
    void destoryObject(RenderOpenGLObj* obj);
    
private:
    void* m_Context=nullptr;
    std::unordered_set<RenderOpenGLObj*> m_Objects;
};

}
