#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/gl.h>

@interface Mesh : NSObject {
    NSData *points;
    NSData *indexes;
    NSData *uvs;
    GLuint buffers[2];
}

+ (Mesh*)meshWithPoints:(NSData*)points indexes:(NSData*)indexes uvs:(NSData*)uvs;

- (void)bind;
- (void)unbind;

- (void)draw;

@end
