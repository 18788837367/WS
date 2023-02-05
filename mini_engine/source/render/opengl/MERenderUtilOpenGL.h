//
//  MERenderUtilOpenGL.h
//  WS
//
//  Created by zrf on 2023/1/29.
//

#ifndef MERenderUtilOpenGL_h
#define MERenderUtilOpenGL_h

#include "MEObject.h"
#include <functional>
#include <unordered_set>

#if defined(ME_OS_MACOS)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

class MERenderOpenGLUtil final {
public:
    static void makeCurrentContext(void* context);
    
    
};

class MERenderOpenGLContext;
class MERenderOpenGLObj : public MEObject {
public:
    MERenderOpenGLObj(MERenderOpenGLContext* context);
    virtual ~MERenderOpenGLObj()=0;
    void onUnMount();
private:
    MERenderOpenGLContext* m_Context=nullptr;
};

class MERenderOpenGLObjProgram : public MERenderOpenGLObj {
public:
    MERenderOpenGLObjProgram(MERenderOpenGLContext* context);
    virtual ~MERenderOpenGLObjProgram();
    
private:
    GLuint m_ID=0;
};


class MERenderOpenGLContext : public MEObject {
public:
    MERenderOpenGLContext(void* context);
    ~MERenderOpenGLContext();
    
    void makeCurrent();
    me_sp<MERenderOpenGLObj> createObject(const std::function<me_sp<MERenderOpenGLObj>(MEObject* context)>& createor);
    void destoryObject(MERenderOpenGLObj* obj);
    
private:
    void* m_Context=nullptr;
    std::unordered_set<MERenderOpenGLObj*> m_Objects;
};

#endif /* MERenderUtilOpenGL_h */
