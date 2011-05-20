#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import "Texture.h"

typedef void (*renderer_fn_t)(CGContextRef, CGSize, void*);

@implementation Texture

@synthesize data, width, height;

- (void)_renderTexture:(renderer_fn_t)renderer withSize:(CGSize)textureSize userData:(void*)userData
{
    width = textureSize.width;
    height = textureSize.height;
    if (!data) {
        data = [NSMutableData dataWithCapacity:width * height * 4];
    } else {
        [data setLength:width * height * 4];
    }
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef ctx = CGBitmapContextCreate([data mutableBytes], width, height, 8, width*4, colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    renderer(ctx, textureSize, userData);
    CGContextRelease(ctx);
}

void renderImage(CGContextRef ctx, CGSize textureSize, void *data)
{
    UIImage *image = (UIImage*)data;
    CGRect frame = CGRectMake(0, 0, image.size.width, image.size.height);
    CGContextDrawImage(ctx, frame, image.CGImage);
}

- (void)loadFromImage:(UIImage*)image
{
    [self _renderTexture:renderImage withSize:image.size userData:image];
}

void renderView(CGContextRef ctx, CGSize textureSize, void *data)
{
    UIView *view = (UIView*)data;
    CGContextTranslateCTM(ctx, 0, textureSize.height);
    CGContextScaleCTM(ctx, 1, -1);
    [view.layer renderInContext:ctx];
}

- (void)loadFromView:(UIView*)view withSize:(CGSize)size
{
    [self _renderTexture:renderView withSize:size userData:view];
}

- (BOOL)writeGL
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.bytes);
    
    return glGetError() == GL_NO_ERROR;
}

- (void)dealloc
{
    [data release];
    [super dealloc];
}

@end
