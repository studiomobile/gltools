#ifdef GLES2

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#else

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#define GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
#define GL_RENDERBUFFER GL_RENDERBUFFER_OES
#define GL_RENDERBUFFER_WIDTH GL_RENDERBUFFER_WIDTH_OES
#define GL_RENDERBUFFER_HEIGHT GL_RENDERBUFFER_HEIGHT_OES
#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_OES
#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES

#define glGenFramebuffers glGenFramebuffersOES
#define glBindFramebuffer glBindFramebufferOES
#define glGenRenderbuffers glGenRenderbuffersOES
#define glBindRenderbuffer glBindRenderbufferOES
#define glGetRenderbufferParameteriv glGetRenderbufferParameterivOES
#define glFramebufferRenderbuffer glFramebufferRenderbufferOES
#define glRenderbufferStorage glRenderbufferStorageOES
#define glCheckFramebufferStatus glCheckFramebufferStatusOES
#define glDeleteFramebuffers glDeleteFramebuffersOES
#define glDeleteRenderbuffers glDeleteRenderbuffersOES

#endif
