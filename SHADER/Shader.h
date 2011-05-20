#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/gl.h>

extern NSString *GL_ERROR_DOMAIN;
NSError* mkGLError(int code, NSString *message);

@interface Shader : NSObject {
    GLuint type;
    NSString *path;
    GLuint binding;
}
@property (nonatomic, readonly) GLuint type;
@property (nonatomic, readonly) GLuint binding;

+ (Shader*)vertexShaderWithFile:(NSString*)path;
+ (Shader*)fragmentShaderWithFile:(NSString*)path;

- (GLuint)bind:(NSError**)error;
- (void)unbind;

@end
