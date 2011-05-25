#import "Texture.h"
#import "GLES.h"

@interface TextureManager : NSObject {
    GLuint *textures;
    NSUInteger count;
    NSUInteger capacity;
}
@property (nonatomic, readonly) NSUInteger size;

- (BOOL)addTexture:(Texture*)texture;

- (BOOL)replaceTexture:(NSUInteger)index withTexture:(Texture*)texture;

- (BOOL)bindTexture:(NSUInteger)index;

- (void)clear;

@end
