#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@protocol EAGLViewControllerDelegate <NSObject>
- (void)setupGL;
- (void)teardownGL;
- (void)drawFrameSize:(CGSize)size;
@end

@interface EAGLViewController : UIViewController {
    IBOutlet id<EAGLViewControllerDelegate> delegate;
@private
    EAGLContext *context;
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
}
@property (nonatomic, assign) id<EAGLViewControllerDelegate> delegate;
@property (nonatomic, readonly, getter=isAnimating) BOOL animating;
@property (nonatomic, assign) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;

@end
