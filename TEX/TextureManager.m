#import "TextureManager.h"

@implementation TextureManager

@synthesize size;

- (id)init {
    if (![super init]) return nil;
    capacity = 16;
    textures = malloc (sizeof(textures[0]) * capacity);
    size = 0;
    return self;
}


- (BOOL)createRGBATexture:(GLubyte*)data width:(size_t)width height:(size_t)height {
    if (size >= capacity) {
        capacity *= 2;
        textures = realloc (textures, sizeof(textures[0]) * capacity);
    }

    glGenTextures(1, &textures[size]);
    if (glGetError() != GL_NO_ERROR) return NO;

    glBindTexture(GL_TEXTURE_2D, textures[size]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    if (glGetError() != GL_NO_ERROR) return NO;

    ++size;
    
    return YES;
}


- (BOOL)loadTextureFromImage:(UIImage*)image {
    size_t width = image.size.width;
    size_t height = image.size.height;
    CGRect frame = CGRectMake(0, 0, width, height);
    
    GLubyte *data = (GLubyte*) malloc( width * height * 4 );
    if (!data) return NO;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGContextRef ctx = CGBitmapContextCreate(data, width, height, 8, width*4, colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);

    CGContextDrawImage(ctx, frame, image.CGImage);
    CGContextRelease(ctx);
    
    BOOL created = [self createRGBATexture:data width:width height:height];
    free(data);
    
    return created;
}


- (BOOL)bindTexture:(NSInteger)index {
    if (index < 0 || index >= size) return NO;
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    return glGetError() == GL_NO_ERROR;
}


@end
