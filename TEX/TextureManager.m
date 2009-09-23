#import <QuartzCore/QuartzCore.h>
#import "TextureManager.h"

typedef void (*renderer_fn_t)(CGContextRef, CGSize, void*);

@implementation TextureManager

@synthesize size;

- (id)init {
    if (![super init]) return nil;
    capacity = 16;
    textures = malloc (sizeof(textures[0]) * capacity);
    size = 0;
    return self;
}


- (int)genTexture {
    if (size >= capacity) {
        capacity *= 2;
        textures = realloc (textures, sizeof(textures[0]) * capacity);
    }
    glGenTextures(1, &textures[size]);
    if (glGetError() != GL_NO_ERROR) return -1;
    return size++;
}


- (BOOL)renderTexture:(renderer_fn_t)renderer withSize:(CGSize)textureSize userData:(void*)userData {
    size_t width = textureSize.width;
    size_t height = textureSize.height;
    GLubyte *data = (GLubyte*) calloc (width * height, 4);
    if (!data) return NO;

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef ctx = CGBitmapContextCreate(data, width, height, 8, width*4, colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    renderer(ctx, textureSize, userData);
    CGContextRelease(ctx);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    BOOL created = glGetError() == GL_NO_ERROR;
    free(data);
    return created;
}


void renderImage(CGContextRef ctx, CGSize textureSize, void *data) {
    UIImage *image = (UIImage*)data;
    CGRect frame = CGRectMake(0, 0, image.size.width, image.size.height);
    CGContextDrawImage(ctx, frame, image.CGImage);
}

- (BOOL)loadTextureFromImage:(UIImage*)image {
    BOOL loaded = [self replaceTexture:[self genTexture] fromImage:image];
    if (!loaded) --size;
    return loaded;
}

- (BOOL)replaceTexture:(NSInteger)index fromImage:(UIImage*)image {
    if (index < 0 || index >= size) return NO;
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    return [self renderTexture:renderImage withSize:image.size userData:image];
}


void renderView(CGContextRef ctx, CGSize textureSize, void *data) {
    UIView *view = (UIView*)data;
    CGContextTranslateCTM(ctx, 0, textureSize.height);
    CGContextScaleCTM(ctx, 1, -1);
    [view.layer renderInContext:ctx];
}

- (BOOL)loadTextureWithSize:(CGSize)textureSize fromView:(UIView*)view {
    BOOL loaded = [self replaceTexture:[self genTexture] withSize:textureSize fromView:view];
    if (!loaded) --size;
    return loaded;
}

- (BOOL)replaceTexture:(NSInteger)index withSize:(CGSize)textureSize fromView:(UIView*)view {
    if (index < 0 || index >= size) return NO;
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    return [self renderTexture:renderView withSize:textureSize userData:view];
}


- (BOOL)bindTexture:(NSInteger)index {
    if (index < 0 || index >= size) return NO;
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    return glGetError() == GL_NO_ERROR;
}


@end
