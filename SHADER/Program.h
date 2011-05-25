#import "Shader.h"
#import <OpenGLES/ES2/gl.h>

@interface Program : NSObject {
    GLuint binding;
    Shader *vertex;
    Shader *fragment;
}
@property (nonatomic, readonly) GLuint binding;

+ (Program*)programWithVertexShader:(Shader*)vsh fragmetShader:(Shader*)fsh;

- (GLuint)bindWithAttributes:(NSDictionary*)attrs error:(NSError**)error;
- (void)unbind;

- (BOOL)validate:(NSError**)error;

- (int)uniformLocationForName:(NSString*)name;

@end
