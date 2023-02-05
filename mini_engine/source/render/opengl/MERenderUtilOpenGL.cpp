//
//  MERenderUtilOpenGL.cpp
//  WS
//
//  Created by zrf on 2023/1/29.
//

#include "MERenderUtilOpenGL.h"

#ifdef ME_OS_MACOS
//#import <Cocoa/Cocoa.h>
#endif

void MERenderOpenGLUtil::makeCurrentContext(void *context) {
//#if defined(ME_OS_MACOS)
//    if(context) {
//        [(__bridge NSOpenGLContext*)context makeCurrentContext];
//    }
//#endif

}



MERenderOpenGLObj::MERenderOpenGLObj(MERenderOpenGLContext* context):m_Context(context) {
}

void MERenderOpenGLObj::onUnMount() {
    if(m_Context) {
        m_Context->destoryObject(this);
    }
}

MERenderOpenGLObjProgram::MERenderOpenGLObjProgram(MERenderOpenGLContext* context)
    :MERenderOpenGLObj(context) {
    
    
        
        
}

MERenderOpenGLObjProgram::~MERenderOpenGLObjProgram() {
    
}





MERenderOpenGLContext::MERenderOpenGLContext(void* context) :m_Context(context) {
    
}

MERenderOpenGLContext::~MERenderOpenGLContext() {
    
}

void MERenderOpenGLContext::makeCurrent()
{
    MERenderOpenGLUtil::makeCurrentContext(m_Context);
}

me_sp<MERenderOpenGLObj> MERenderOpenGLContext::createObject(const std::function<me_sp<MERenderOpenGLObj>(MEObject* context)>& createor)
{
    auto obj=createor(this);
    if(obj.isNotNull()) {
        m_Objects.insert(obj.get());
    }
    return obj;
}

void MERenderOpenGLContext::destoryObject(MERenderOpenGLObj* obj)
{
    if(m_Objects.find(obj)!=m_Objects.end()) {
        m_Objects.erase(obj);
    }
}
