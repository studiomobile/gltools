#import <Foundation/Foundation.h>
#import <OpenGLES/ES1/gl.h>
#import <UIKit/UIKit.h>

@interface TextureManager : NSObject {
    GLuint *textures;
    NSInteger size;
    NSInteger capacity;
}
@property (nonatomic, readonly) NSInteger size;

- (BOOL)loadTextureFromImage:(UIImage*)image;

- (BOOL)bindTexture:(NSInteger)index;

@end
