#import <UIKit/UIKit.h>
#import "GLES.h"

@class EAGLContext;

@interface EAGLView : UIView {
@private
    GLint framebufferWidth;
    GLint framebufferHeight;
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderBuffer;
}
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, readonly) GLint framebufferWidth;
@property (nonatomic, readonly) GLint framebufferHeight;

- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end
