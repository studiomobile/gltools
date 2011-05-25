#import <QuartzCore/QuartzCore.h>
#import "TextureManager.h"

@implementation TextureManager

- (id)init {
    self = [super init];
    if (self) {
        capacity = 16;
        textures = malloc (sizeof(textures[0]) * capacity);
        count = 0;
    }
    return self;
}

- (NSUInteger)size { return count; }

- (BOOL)addTexture:(Texture*)texture {
    if (count >= capacity) {
        capacity *= 2;
        textures = realloc (textures, sizeof(textures[0]) * capacity);
    }
    
    glGenTextures(1, &textures[count]);
    if (glGetError() != GL_NO_ERROR) return FALSE;
    ++count;
    
    return [self replaceTexture:count-1 withTexture:texture] || --count;
}

- (BOOL)replaceTexture:(NSUInteger)index withTexture:(Texture*)texure {
    return [self bindTexture:index] && [texure writeGL];
}

- (BOOL)bindTexture:(NSUInteger)index {
    if (index >= count) return NO;
    glBindTexture(GL_TEXTURE_2D, textures[index]);
    return glGetError() == GL_NO_ERROR;
}

- (void)clear
{
    glDeleteTextures(count, textures);
    count = 0;
}

- (void)dealloc
{
    [self clear];
    free(textures);
    [super dealloc];
}

@end
