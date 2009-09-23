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
- (BOOL)replaceTexture:(NSInteger)index fromImage:(UIImage*)image;

- (BOOL)loadTextureWithSize:(CGSize)size fromView:(UIView*)view;
- (BOOL)replaceTexture:(NSInteger)index withSize:(CGSize)size fromView:(UIView*)view;

- (BOOL)bindTexture:(NSInteger)index;

@end
