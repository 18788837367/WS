#include "RenderUtilOpenGL.h"

#ifdef ME_OS_MACOS
//#import <Cocoa/Cocoa.h>
#endif

namespace ME {

void RenderOpenGLUtil::makeCurrentContext(void *context) {
//#if defined(ME_OS_MACOS)
//    if(context) {
//        [(__bridge NSOpenGLContext*)context makeCurrentContext];
//    }
//#endif

}



RenderOpenGLObj::RenderOpenGLObj(RenderOpenGLContext* context):m_Context(context) {
}

void RenderOpenGLObj::onUnMount() {
    if(m_Context) {
        m_Context->destoryObject(this);
    }
}

RenderOpenGLObjProgram::RenderOpenGLObjProgram(RenderOpenGLContext* context)
    :RenderOpenGLObj(context) {
    
    
        
        
}

RenderOpenGLObjProgram::~RenderOpenGLObjProgram() {
    
}





RenderOpenGLContext::RenderOpenGLContext(void* context) :m_Context(context) {
    
}

RenderOpenGLContext::~RenderOpenGLContext() {
    
}

void RenderOpenGLContext::makeCurrent()
{
    RenderOpenGLUtil::makeCurrentContext(m_Context);
}

me_sp<RenderOpenGLObj> RenderOpenGLContext::createObject(const std::function<me_sp<RenderOpenGLObj>(Object* context)>& createor)
{
    auto obj=createor(this);
    if(obj.isNotNull()) {
        m_Objects.insert(obj.get());
    }
    return obj;
}

void RenderOpenGLContext::destoryObject(RenderOpenGLObj* obj)
{
    if(m_Objects.find(obj)!=m_Objects.end()) {
        m_Objects.erase(obj);
    }
}

}
