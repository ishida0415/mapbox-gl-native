#ifndef MBGL_RENDERER_GL
#define MBGL_RENDERER_GL

#include <string>

#if __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_IPHONE_SIMULATOR
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_OS_MAC
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl.h>
    #else
        #error Unsupported Apple platform
    #endif
#elif MBGL_USE_GLES2
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

namespace mbgl {
namespace gl {

// GL_KHR_debug
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
#define GL_DISPLAY_LIST                   0x82E7
#define GL_VERTEX_ARRAY                   0x8074
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TEXTURE                        0x1702
#define GL_RENDERBUFFER                   0x8D41
#define GL_FRAMEBUFFER                    0x8D40
typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, const void *userParam);
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC) (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
typedef void (APIENTRYP PFNGLGETPOINTERVPROC) (GLenum pname, void **params);
typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC) (GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC) (void);
typedef void (APIENTRYP PFNGLOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC) (const void *ptr, GLsizei length, const GLchar *label);
typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC) (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
extern PFNGLDEBUGMESSAGECONTROLPROC DebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC DebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKPROC DebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGPROC GetDebugMessageLog;
extern PFNGLGETPOINTERVPROC GetPointerv;
extern PFNGLPUSHDEBUGGROUPPROC PushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC PopDebugGroup;
extern PFNGLOBJECTLABELPROC ObjectLabel;
extern PFNGLGETOBJECTLABELPROC GetObjectLabel;
extern PFNGLOBJECTPTRLABELPROC ObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELPROC GetObjectPtrLabel;

typedef void (* PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (* PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void (* PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
typedef GLboolean (* PFNGLISVERTEXARRAYPROC) (GLuint array);
extern PFNGLBINDVERTEXARRAYPROC BindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
extern PFNGLISVERTEXARRAYPROC IsVertexArray;


// Debug group markers, useful for debuggin on iOS
#if __APPLE__ && defined(DEBUG) && defined(GL_EXT_debug_marker)
// static int indent = 0;
inline void start_group(const std::string &str) {
    glPushGroupMarkerEXT(0, str.c_str());
    // fprintf(stderr, "%s%s\n", std::string(indent * 4, ' ').c_str(), str.c_str());
    // indent++;
}

inline void end_group() {
    glPopGroupMarkerEXT();
    // indent--;
}
#else
inline void start_group(const std::string &) {}
inline void end_group() {}
#endif


struct group {
    inline group(const std::string &str) { start_group(str); }
    ~group() { end_group(); };
};
}
}

#ifdef GL_ES_VERSION_2_0
    #define glClearDepth glClearDepthf
    #define glDepthRange glDepthRangef
#endif

void _CHECK_GL_ERROR(const char *cmd, const char *file, int line);

#define _CHECK_ERROR(cmd, file, line) \
    cmd; \
    do { _CHECK_GL_ERROR(#cmd, file, line); } while (false);

#define CHECK_ERROR(cmd) _CHECK_ERROR(cmd, __FILE__, __LINE__)

#endif
