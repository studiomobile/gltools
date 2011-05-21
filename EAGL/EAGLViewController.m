#import <QuartzCore/QuartzCore.h>
#import "EAGLViewController.h"
#import "EAGLView.h"

@interface EAGLViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation EAGLViewController

@synthesize delegate;
@synthesize animating;
@synthesize animationFrameInterval;
@synthesize context;
@synthesize displayLink;


- (void)_setup
{
    if (context) {
        [EAGLContext setCurrentContext:context];
        return;
    }

    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
    
    animating = FALSE;
    animationFrameInterval = 1;
    
    [self.displayLink invalidate];
    self.displayLink = nil;
	
    EAGLView *view = (EAGLView *)self.view;
    [view setContext:context];
    [view setFramebuffer];
    
    [delegate setupGL];
}

- (void)_teardown
{
    if (!context) return;

    if ([EAGLContext currentContext] == context) {
        [delegate teardownGL];
        [EAGLContext setCurrentContext:nil];
    }
	self.context = nil;	
}

- (void)_drawFrame
{
    EAGLView *view = (EAGLView *)self.view;
    [view setFramebuffer];
    [delegate drawFrameSize:CGSizeMake(view.framebufferWidth, view.framebufferHeight)];
    [view presentFramebuffer];
}

- (void)dealloc
{
    [self _teardown];
    [super dealloc];
}

#pragma mark - View lifecycle

- (void)loadView
{
    self.view = [[EAGLView new] autorelease];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
    [self _teardown];
    [super viewDidUnload];
}

#pragma mark - Actions

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    if (frameInterval >= 1) {
        animationFrameInterval = frameInterval;
        if (animating) {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating) {
        if (delegate) [self _setup];
        CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(_drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating) {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

@end
