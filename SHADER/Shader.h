#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/gl.h>

extern NSString *GL_ERROR_DOMAIN;
NSError* mkGLError(int code, NSString *message);

@interface Shader : NSObject {
    GLuint type;
    NSString *source;
    GLuint binding;
}
@property (nonatomic, readonly) GLuint type;
@property (nonatomic, readonly) GLuint binding;

+ (Shader*)vertexShaderWithSource:(NSString*)source;
+ (Shader*)fragmentShaderWithSource:(NSString*)source;

- (GLuint)bind:(NSError**)error;
- (void)unbind;

@end
